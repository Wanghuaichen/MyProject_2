/*******************************************************************/
/*                                                                 */
/*    AMICE 4.0  -  68HC11                                         */
/*                                                                 */
/*    Network interface procedures (packetnumbers)                 */
/*                                                                 */
/*    author:  L.A. Boulogne                                       */
/*    date:    21-10-91                                            */
/*                                                                 */
/*                                                                 */
/*******************************************************************/

#include "../usx/resource.h"
#include "ext_net.h"
#include "local_net.h"
#include "net_target.h"
#include "net_state_nrs.h"
#include "../usx/ext_usx.h"

//#include "../appl/milk.h"

extern unsigned char msgTimeoutTime;

#pragma codeseg(EEPAGE)

/*************** external function prototypes *****************/

/*************** local function prototypes ********************/

#define NULL (void *)0


/*******************************************************************/
/*                                                                 */
/* Procedure  NetInit()                                            */
/*                                                                 */
/* Initiate buffers pointers and ID's for a channel                */
/*                                                                 */
/*******************************************************************/

signed short  NetInit(signed short Channel, signed short Buffers,
                      signed short BufSize, signed short Node,
                      signed short Mode, signed char *NodeList)
{
  RxIdFifoType *HlpRxFiP;
  TxIdFifoType *HlpTxFiP;
  signed char   Error;
  signed char  *HlpPntr;
  int           MemSize,ListSize;
  int           i;
  int           BaseAddress;

  Channel = 1;                         /* Channel 1  = 1   */
  NetApplSendTimeout = 15;
  Error = 0;                           /* default no error */

/************** check parameter values *************/

  if ( Buffers >  127      || Buffers < 2    ||
       BufSize >  256      || BufSize < 1    ||
       Node    >   61      || Node    < 0    ||
       Mode    !=   0                          ) {
   Error = -1;
  }

                                      /* determine length of NodeList */
  HlpPntr = NodeList;
  if (*HlpPntr == (signed char)-1) { Error = -1;}
  for (ListSize = 0 ; ListSize < NodeListLengthMax ; ListSize++) {
    if (*HlpPntr == (signed char)-1) {
      break;
    } else {
      if (*HlpPntr < 0 || *HlpPntr > 61 ) {      /* valid token address ?  */
        Error = -1;
      }
    }
    HlpPntr++;
  }
  if (++ListSize > NodeListLengthMax) {          /* valid length    */
    Error = -1;
  }

  ListSize = (ListSize+1)&0xFFFE;                /* even value for ListSize */
  BufSize  = (BufSize +1)&0xFFFE;                /* even value for BufSize */

  if (!Error) {

/**** release memory currently assigned to this channel ****/

    if (Net.NetStat.State != Off) {         /* not a start from reset? */

      UartDisReal();                          /* disable Uart Tx,Rx */
      if (UsxRelMem((void *)Net.MemPntr)) { /* release requested memory */
        Error = -1;
      }
      Net.NetStat.State = Off;
    }
  }
  if (!Error) {

/****************** request memory *********************/

    MemSize = (((NodeListLengthMax + 2) +1) & 0xFFFE) + /* arbitration list */
              2*Buffers*BufSize +                       /* Rx and Tx data buffers */
              sizeof(RxIdType)*Buffers +                /* RxId blocks      */
              sizeof(RxIdType *)*(Buffers + 1)*3 +      /* size of fifo's   */
              sizeof(TxIdType)*Buffers +                /* TxId blocks      */
              sizeof(TxIdType *)*((Buffers+1)*2+2);     /* size of fifo's   */

    BaseAddress = (int)UsxReqMem(MemSize,Wild);         /* request memory */
    if (BaseAddress == -1) {
      Error = -1;
    } else {                                            /* continue if memory available */


/************ initiate requested memory *****************/

      UartIntMaskStatus = 0x0C;
      TimerIntMaskStatus = 0;
      Net.MemPntr = BaseAddress;

      Net.NetStat.AnswList = (signed char *)BaseAddress;

      while ( *NodeList != -1) {
        *(char *)BaseAddress++ = TokenCode[*NodeList++];
      }
      *(char *)BaseAddress++ = -1;

      if (Net.NetStat.AnswList[1] == -1) {
        Net.ManyNodes = 0;
      } else {
        Net.ManyNodes = 1;
      }

      BaseAddress = (BaseAddress +1)&0xFFFE; /* even value for address */

/****** Rx ID fifo control blocks and Rx ID's ********/

      Net.RxCbId.Rdp = (RxIdFifoType *)BaseAddress;
      Net.RxCbId.Beg = (RxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(RxIdType *)*(Buffers);
      Net.RxCbId.Wrp = (RxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(RxIdType *);
      Net.RxCbId.End = (RxIdFifoType *)BaseAddress;

      Net.RxCbHi.Wrp = (RxIdFifoType *)BaseAddress;
      Net.RxCbHi.Rdp = (RxIdFifoType *)BaseAddress;
      Net.RxCbHi.Beg = (RxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(RxIdType *)*(Buffers + 1);
      Net.RxCbHi.End = (RxIdFifoType *)BaseAddress;

      Net.RxCbLo.Wrp = (RxIdFifoType *)BaseAddress;
      Net.RxCbLo.Rdp = (RxIdFifoType *)BaseAddress;
      Net.RxCbLo.Beg = (RxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(RxIdType *)*(Buffers + 1);
      Net.RxCbLo.End = (RxIdFifoType *)BaseAddress;

      HlpRxFiP = Net.RxCbId.Beg;
      for (i = 0 ; i < Buffers ; i++) {
        *HlpRxFiP = (RxIdType *)BaseAddress;
        BaseAddress += sizeof(RxIdType);
        (*HlpRxFiP)->DataPntr = (signed char *)BaseAddress;
        BaseAddress += BufSize;
        HlpRxFiP++;
      }

      Net.RxDrCb.RxIdPntr = NULL;

/****** Tx ID fifo control blocks and Tx ID's ********/

      Net.TxCbId.Rdp = (TxIdFifoType *)BaseAddress;
      Net.TxCbId.Beg = (TxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(TxIdType *)*(Buffers);
      Net.TxCbId.Wrp = (TxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(TxIdType *);
      Net.TxCbId.End = (TxIdFifoType *)BaseAddress;

      Net.TxCbHi.Wrp = (TxIdFifoType *)BaseAddress;
      Net.TxCbHi.Rdp = (TxIdFifoType *)BaseAddress;
      Net.TxCbHi.Beg = (TxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(TxIdType *)*2;
      Net.TxCbHi.End = (TxIdFifoType *)BaseAddress;

      Net.TxCbLo.Wrp = (TxIdFifoType *)BaseAddress;
      Net.TxCbLo.Rdp = (TxIdFifoType *)BaseAddress;
      Net.TxCbLo.Beg = (TxIdFifoType *)BaseAddress;
      BaseAddress += sizeof(TxIdType *)*(Buffers + 1);
      Net.TxCbLo.End = (TxIdFifoType *)BaseAddress;

      HlpTxFiP = Net.TxCbId.Beg;
      for (i = 0 ; i < Buffers ; i++) {
        *HlpTxFiP = (TxIdType *)BaseAddress;
        BaseAddress += sizeof(TxIdType);
        (*HlpTxFiP)->DataPntr = (signed char *)BaseAddress;
        BaseAddress += BufSize;
        HlpTxFiP++;
      }

/******* UART address, Net status, Net Error Status ***********/

      Net.TxDrCb.TxIdPntr = NULL;
      Net.NetStat.Buffers = Buffers;
      Net.NetStat.Size = BufSize;
      Net.NetStat.Mode = Mode;
      Net.NetStat.Buffers = Buffers;
      Net.NetStat.State = StandBy;
      Net.ArbNode = Node;

      Net.NetStat.BusAct = FALSE;
      Net.NetStat.Arbit  = FALSE;

      Net.StatusPacket.Source = *Net.NetStat.AnswList; /* WTH ??? */

      Net.ErrStat.CrcCnt = 0;
      Net.ErrStat.ReSync = 0;
      Net.ErrStat.MsgTo  = 0;
      Net.ErrStat.TokTo  = 0;
      Net.ErrStat.BufOvf = 0;

/******** Tx message pool control block *********/

      for (i=0;i<TxPoolMax;i++) {
        Net.TxPool[i].Node       = EmptyPool;
        Net.TxPool[i].RetryCount = RetryMax;
        Net.TxPool[i].Next       = NULL;
        Net.TxPool[i].Last       = NULL;
      }
      Net.TxPoolIx = 1;
    }
  }
  return (Error);
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetOpen()                                            */
/*                                                                 */
/* Activate this channel and attach node to network                */
/*                                                                 */
/*******************************************************************/

signed short  NetOpen(signed short Channel)

{
  signed short   ReturnValue;
  int            i;
  unsigned char  Byte;

  Channel = 1;             /* Channel 1..Channels -> 0..Channels-1 */

  ReturnValue = -1;

  if (Net.NetStat.State == StandBy) {
    Net.NetStat.State = Active;                        /* set channel active    */

    Byte = EmptyRxUart();                              /* waste last received characters         */
    Byte = EmptyRxUart();
    UartEnbReal();                                     /* Uart interrupt enable, 68HC11 specific */

    Net.TxState = TxReset;                             /* reset Rx and Tx state */
    Net.RxState = RxReset;

    for (i = 0; i < PacketNrsMax ; i++)  {
      Net.PacketNrsRx[i] = 0;   /* reset packetnumbers   */
      Net.PacketNrsTx[i] = 0;   /* reset packetnumbers   */
    }

    SetBit9();                                      /* set Uart in 9 bit mode */

/*  if (Net.NetStat.Mode == ArbiMaster) {     */
/*    SendNextToken();                        */     /* implies wakeup mode on */
/*    WakeUpModeOffReal();                    */        /* explicit wakeup mode off */
/*  } else {                                  */
/*    WakeUpModeOnReal();                     */
/*  }                                         */

    WakeUpModeOnReal();

    ReturnValue = 0;
  } else {
    if (Net.NetStat.State == Active) {
      ReturnValue = 0;
    } else {
      ReturnValue = -1;
    }
  }
  return(ReturnValue);
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetClose()                                           */
/*                                                                 */
/* De-activate this channel and remove node from network           */
/* Buffered messages remain present.                               */
/*                                                                 */
/*******************************************************************/

signed short  NetClose(signed short Channel)

{
  int      ReturnValue;

  Channel = 1;             /* Channel 1..Channels -> 0..Channels-1 */

  ReturnValue = -1;

  switch (Net.NetStat.State) {
      case StandBy: {
        ReturnValue = 0;
        break;
      }
      case Active: {
        Net.NetStat.State = StandBy;
        StopTimeoutReal();                      /* disable timeout */
        UartDisReal();                   /* disable Uart Tx and Rx */
        ReturnValue = 0;
        break;
      }
  }
  return(ReturnValue);
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetInitStat()                                        */
/*                                                                 */
/* Clear network error counters and flags                          */
/*                                                                 */
/*******************************************************************/

void  NetInitStat(signed short Channel)
{

  Channel -=1;              /* Channel 1..Channels -> 0..Channels-1 */

    Net.NetStat.BusAct = FALSE;
    Net.NetStat.Arbit  = FALSE;
    Net.ErrStat.CrcCnt = 0;
    Net.ErrStat.ReSync = 0;
    Net.ErrStat.MsgTo  = 0;
    Net.ErrStat.TokTo  = 0;
    Net.ErrStat.BufOvf = 0;
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetErrStat()                                         */
/*                                                                 */
/* Give pointer to error status structure                          */
/*                                                                 */
/*******************************************************************/

ErrStatType  *NetErrStat(signed short Channel)
{
  Channel -=1;              /* Channel 1..Channels -> 0..Channels-1 */
  if (Net.NetStat.State != Off) {
    return(&Net.ErrStat);
  } else {
    return((ErrStatType *)NULL);
  }
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetSendStat()                                        */
/*                                                                 */
/* Return current message status                                   */
/*                                                                 */
/*******************************************************************/
signed short  NetSendStat(TxIdType *TxIdPntr)

{
  if (TxIdPntr==NULL) {
    return(-1);
  } else {
//    if ((int)TxIdPntr->Status == Busy) {
//      if ((TxIdPntr->TxIdTime + NetApplSendTimeout) < (UsxGetAbsTime())) {
//        NetSendRel(TxIdPntr);
//        return (ApplSendTimeout);
//      }
//    }
    return ((int)TxIdPntr->Status);
  }
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetSend()                                            */
/*                                                                 */
/* Get Tx Id block from stack                                      */
/* Fill Tx Id block and data buffer with data                      */
/* Put Tx Id into fifo to request transmission                     */
/*                                                                 */
/*******************************************************************/

TxIdType  *NetSend(signed short Channel,signed short SrcAddress, signed short DestAddress,
                   NET_MESSAGE_TYPE Type, signed short Length, unsigned short Timeout,
                   const unsigned char *DataPntr)
{
  TxIdType     *TxIdPntr,*TxIdReturn;
  signed char  *BufferPntr;
  signed char   Priority;
  unsigned long SendTime;

  Channel -=1;              /* Channel 1..Channels -> 0..Channels-1 */

  if (   Net.NetStat.State == Off
      || Net.NetStat.State == StandBy
      || Type <= 0   || Type > 9
      || Type == 2   || Type == 4
      || Type == 6   || Type == 8
      || SrcAddress < 0 || SrcAddress > 62
      || DestAddress < 0 || DestAddress > 62
      || SrcAddress != CheckNode(SrcAddress)
      || (SrcAddress == 62 && (Type == 1 || Type == 2 || Type == 9))
      || (Length > Net.NetStat.Size)
     ) {
    return ((TxIdType *)&TxStsError);
  }

  TxIdReturn = (TxIdType *)&TxStsReady;            /* default return status ready */

  Priority = PriorCode[Type];

  SendTime = UsxGetAbsTime() + Timeout;
  if (Priority == Low) {
    UsxGetRes(NetSendResLow);                       /* claim send low resource */
  }

  do {

    UsxGetRes(NetSendRes);                          /* claim this resource */

    /* get a free buffer -----------------------------------------------------------------*/
    if ((TxIdPntr = GetFreeTxId(&Net)) == NULL) {;   /* free ID available */
      UsxRelRes(NetSendRes);
/*    TxIdReturn = (TxIdType *)&TxStsNoBuf; WTH */    /* no, then return status no buffer */
      if ( UsxGetAbsTime() < SendTime ) {
        TxIdPntr = TxIdReturn = (TxIdType *)&TxStsNoBuf;        /* WTH; TxIdPntr->Status == TxNoBuf*/
      } else {
        TxIdPntr = TxIdReturn = (TxIdType *)&TxStsApplTimeout;  /* WTH */
      }
    } else {
       
    /* initialise TxId ---------------------------------------------------------------------*/
      TxIdPntr->TxIdSlot = UsxSltTsk(0);
      TxIdPntr->TxIdTime = UsxGetAbsTime();
      TxIdPntr->Source = SrcAddress;
      TxIdPntr->Destin = DestAddress;
      TxIdPntr->Type = Type;
      TxIdPntr->Length = Length;
      if ( Type < 3) {          /* confirmed low or high priority message */
        TxIdPntr->TxIdPacketNr = GetPacketNrNr;
      } else {
        TxIdPntr->TxIdPacketNr = 0;
      }
      
      //ABM Destination Address
//    Net.DestNode = DestAddress;
      
      /* copy message to buffer ---------------------------------------------------------------*/
      BufferPntr = TxIdPntr->DataPntr;
      while (Length--) *(BufferPntr++) = *(DataPntr++) ;

      /* determine status ---------------------------------------------------------------------*/
      if (TypeCode[Type] != 0x40) {                /* return ID pointer on confirmed (looptest) messages */
        TxIdReturn = TxIdPntr;
        TxIdPntr->Status = Busy;                   /* mark Id status 'busy' */
      } else {
        TxIdReturn = (TxIdType *)&TxStsReady;
        TxIdPntr->Status = InternNetState;         /* mark ID status 'autorelease' */
      }

      /* put TxId in fifo ---------------------------------------------------------------------*/
      PutUsedTxId(Priority == Low ? &Net.TxCbLo : &Net.TxCbHi, TxIdPntr);
      UsxRelRes(NetSendRes);
    }
  } while (TxIdPntr->Status == TxNoBuf && Priority == Low );

  if (Priority == Low) UsxRelRes(NetSendResLow);
  Net.TxDrCb.TxRequest = TRUE;

  return (TxIdReturn);
}


/*******************************************************************/
/*                                                                 */
/* Procedure  NetCheckSending(short channel)                       */
/*                                                                 */
/* Check whether this channel is sending a message                 */
/*                                                                 */
/* input: channel: don't care                                      */
/* returnvalue:                                                    */
/*        0 (FALSE)   not sending a message                        */
/*        1 (TRUE )   sending a message                            */
/*                                                                 */
/*******************************************************************/

short NetCheckSending(short Channel)
{
  Channel -= 1;

  return (msgTimeoutTime==0 ? 0 : Net.TxDrCb.TxRequest);
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetSendRel()                                         */
/*                                                                 */
/* Release current Tx message ID block and put it into the fifo.   */
/* No release is executed if:                                      */
/*   - status is the predefined 'ready' status (unconfirmed mesg)  */
/*   - status is the predefined 'no Tx buffer' status              */
/*   - status is the predefined 'error' status                     */
/*   - status is 'free' (after a previous release)                 */
/*                                                                 */
/*******************************************************************/

void  NetSendRel(TxIdType *TxIdPntr)

{

  if ((TxIdPntr != (TxIdType *)(&TxStsReady)) &&
      (TxIdPntr != (TxIdType *)(&TxStsNoBuf)) &&
      (TxIdPntr != (TxIdType *)(&TxStsApplTimeout)) &&
      (TxIdPntr != (TxIdType *)(&TxStsError)))   {

    if (TxIdPntr->Status != Free && TxIdPntr->Status != InternNetState ) { /* ID in use? */

      if (TxIdPntr->TxIdSlot == UsxSltTsk(0) ) {                           /* release only your own message */
        TxIdPntr->TxIdSlot = 0;                                            /* release TxId from task */

        if ( CertifyRelease(&TxIdPntr->Status) == 0 ) {
          TxIdPntr->Status = Free;                 /* then make status free */
          UsxGetRes(NetRelRes);          /* claim this resource */
          PutFreeTxId(&Net,TxIdPntr); /* return ID to stack */

          UsxRelRes(NetRelRes);          /* release this resource */
        }
      }
    }
  }
}

/*******************************************************************/
/*                                                                 */
/* Procedure  NetReceive()                                         */
/*                                                                 */
/*******************************************************************/


signed short  NetReceive(signed short Channel,signed short *AddressSrc,
                         signed short *AddressDest,
                         NET_MESSAGE_TYPE *Type,unsigned char *DataPntr)
{
  signed short Length, ReturnLength = 0;
  unsigned char *MsgPntr;
  RxIdType  *RxIdPntr;

  Channel -=1;              /* Channel 1..Channels -> 0..Channels-1 */

  if (Net.NetStat.State == Off ) {      /* channel initialised ? */
   return (0);
  }

  UsxGetRes(NetRecRes);                /* claim this resource */

  /* TEST wachten op versturen van de Acknodledge */
  if (Net.TxState == TxStatDest  ) {goto exit;}
  if (Net.TxState == TxStatSpace ) {goto exit;}
  if (Net.TxState == TxStatSource) {goto exit;}
  if (Net.TxState == TxStatData  ) {goto exit;}
  if (Net.TxState == TxStatFstCrc) {goto exit;}
  if (Net.TxState == TxStatSndCrc) {goto exit;}

  if ((RxIdPntr = GetUsedRxId( &(Net.RxCbHi) )) != NULL ||
          (RxIdPntr = GetUsedRxId( &(Net.RxCbLo) )) != NULL) {

    /* message received ---------------------------------------------------------------------------------------*/
    *AddressDest = RxIdPntr->Destin;
    *AddressSrc  = RxIdPntr->Source;
    *Type = RxIdPntr->Type;
    ReturnLength = Length = RxIdPntr->Length;

    /* copy message -------------------------------------------------------------------------------------------*/
    MsgPntr = (unsigned char *)(RxIdPntr->DataPntr);
    while (Length--) *(DataPntr++) = *(MsgPntr++);

    /* return RxId to free-fifo -------------------------------------------------------------------------------*/
    PutFreeRxId(&Net, RxIdPntr);
  }

exit:

  UsxRelRes(NetRecRes);                /* release this resource */
  return(ReturnLength);
}

