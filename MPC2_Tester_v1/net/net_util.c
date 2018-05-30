/*******************************************************************/
/*                                                                 */
/*    AMICE  4.0  -  68HC11                                        */
/*                                                                 */
/*    Utilities                                                    */
/*                                                                 */
/*******************************************************************/

#include "ext_net.h"
#include "local_net.h"
#include "net_target.h"
#include "net_def.h"
#include "net_state_nrs.h"


#pragma codeseg(EEPAGE)


/**** external function prototypes ****/

/**** local function prototypes ****/

/*************************************************************************/
/*                                                                       */
/*  Send next token from arbitration list                                */
/*                                                                       */
/*  Parameter:   *NetPntr   - pointer to current struct Net              */
/*                                                                       */
/*************************************************************************/

void SendNextToken(void)
{

  if (Net.NetStat.Mode == ArbiMaster) {     /* arbitration master: time out on token */
    StartTimeout(TimeoutToken);
  } else {
    WakeUpModeOn();                         /* arbitration slave: select on 9th bit */
  }
/*DisableRx(); */
/*EnableTx();  */
  DisableTx();
  SetBit9();
  WriteUart(TokenCode[Net.ArbNode]);            /* send token with 9th bit = 1 */
  EnableTxE();
}

/*************************************************************************/
/*                                                                       */
/*  Check token from arbitration list                                    */
/*                                                                       */
/*************************************************************************/

signed char CheckToken(signed char Token)
{
  signed char *p;

  p = Net.NetStat.AnswList;
  while (*p != -1) {
    if (*p == Token) {
      break;
    } else {
      p++;
    }
  }
  return(*p);
}

/*************************************************************************/
/*                                                                       */
/*  Check node  from arbitration list                                    */
/*                                                                       */
/*************************************************************************/

signed char CheckNode(signed char Node)
{
  signed char c;

  c = CheckToken(TokenCode[Node & AddressMask]);
  return((c == -1) ? -1 : c & AddressMask);
}

/*************************************************************************/
/*                                                                       */
/*  Network time out timer, call every millisecond                       */
/*                                                                       */
/*************************************************************************/

void NetTimerTick(void) {

  StopTimeout();                            /* stop counter */
  if ((Net.TxState == TxReset) &&           /* token time out? */
    (Net.NetStat.Mode == ArbiMaster)) {     /* arbitration master? */
    Net.ErrStat.TokTo++;                    /* increment token time out counter */
    Net.TxState = TxReset;                  /* reset Tx state machine */
    SendNextToken();                        /* send token */
  } else {
    if ((Net.TxState == TxStatWait) ||      // time out on status packet? 
       (Net.TxState == TxSyncWaitStat)) {   // time out on sync status packet? 
      Net.StatusPacket.Data = StatusTimeOut;
      Net.ErrStat.MsgTo++;                  /* count message time out */
      Net.TxState = TxStatValid;            /* continue on time out */
      NetTx();
    } else {                                /* time out on bus activity */
      EnableTx();
      NetTx();                              /* continue Tx */
    }
  }
}

/*************************************************************************/
/*                                                                       */
/*  Calculate CRC     THE ASM version is used instead of this one        */
/*                                                                       */
/*  Parameter:   Byte - item to add to CRC                               */
/*               Crc  - last CRC                                         */
/*  Returnvalue: Crc  - new CRC                                          */
/*************************************************************************/
/*
void Crc12c(unsigned char Byte, unsigned int *CrcPntr)
{
  signed char Bit;
  signed char Count;
  int  Crc;

  Crc = *CrcPntr ^ Byte;

  for (Count = 0 ; Count < 8 ; Count++) {
    Bit = Crc & 0x0001;
    Crc = Crc >> 1;
    if (Bit) {
      Crc ^= Polynomial;
    }
  }
  *CrcPntr = Crc;
}
*/
