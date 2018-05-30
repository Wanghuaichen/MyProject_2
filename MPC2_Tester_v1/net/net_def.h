/*******************************************************************/
/*                                                                 */
/*    AMICE  4.0  - 68HC11                                         */
/*                                                                 */
/*    formal declarations (define and typedef)                     */
/*                                                                 */
/*******************************************************************/

#ifndef NET_DEF_INCLUDED
#define NET_DEF_INCLUDED  1
/*
            Summery of all (I think) message types

            x - Destination Node
            y - Source      Node
            z - Don't care  (normely zero)

Byte 0      Byte 1      Byte 2

00xx xxxx                           Token  Parity clear (odd)
01xx xxxx                           Token  Parity set   (odd)

10xx xxxx   00yy yyyy   0000 0001   Low   Long   Confirmed     PacketNr-1
10xx xxxx   00yy yyyy   0000 0010   Low   Long   Confirmed     PacketNr-2
10xx xxxx   00yy yyyy   0000 0011   Low   Long   Confirmed     PacketNr-3
10xx xxxx   01yy yyyy   0000 0000   Low   Long   Unconfirmed   PacketNr-0
1011 1110   01yy yyyy   0000 0000   Low   Long   Unconfirmed   Broadcast
10xx xxxx   10yy yyyy   zzzz z000   Low   Short  Status        Acknowledge
10xx xxxx   10yy yyyy   zzzz z001   Low   Short  Status        NotReady
10xx xxxx   10yy yyyy   zzzz z010   Low   Short  Status        SizeError
10xx xxxx   10yy yyyy   zzzz z100   Low   Short  Status        TimeOut
10xx xxxx   11yy yyyy   0000 0000   Low   Short  Looptest      Normal
10xx xxxx   11yy yyyy   0000 0010   Low   Short  Looptest      Synchronisation

11xx xxxx   00yy yyyy   0000 0001   High  Long   Confirmed     PacketNr-1
11xx xxxx   00yy yyyy   0000 0010   High  Long   Confirmed     PacketNr-2
11xx xxxx   00yy yyyy   0000 0011   High  Long   Confirmed     PacketNr-3
11xx xxxx   01yy yyyy   0000 0000   High  Long   Unconfirmed   PacketNr-0
1111 1110   01yy yyyy   0000 0000   High  Long   Unconfirmed   Broadcast
11xx xxxx   10yy yyyy   zzzz z000   High  Short  Status        Acknowledge
11xx xxxx   10yy yyyy   zzzz z001   High  Short  Status        NotReady
11xx xxxx   10yy yyyy   zzzz z010   High  Short  Status        SizeError
11xx xxxx   10yy yyyy   zzzz z100   High  Short  Status        TimeOut
11xx xxxx   11yy yyyy   0000 0000   High  Short  Looptest      Normal
11xx xxxx   11yy yyyy   0000 0011   High  Short  Looptest      Synchronisation
*/
/*fdef MHz5       */
/*efine  MILISECONDE      1250  */  /* 5 MHz versie 68HC11 */
/*ndif              */

/*fdef MHz8    */
#define  MILISECONDE      2000      /* 8 MHz versie 68HC11 */
/*ndif     */

#define  FALSE            0
#define  TRUE             1

#define  Wild             1

#define  Channels          1
#define  Low               0x00
#define  High              0x40
#define  Free              10
#define  TxPoolMax         5      /* Nr of Tx Driver Control Blocks   */
#define  RetryMax          4     /* Maximum Nr of retries permitted  */
#define  NodeListLengthMax 51     /* number of nodes to answer        */

#define  EmptyPool         0xFF

#define  ValidCrc         0
#define  InitCrc          0x0FFF
#define  Polynomial       0x080F

/* Byte 0 of frame or token both with bit 9 set */

#define  TokenFrameMask   0x80  /* 0 - Token  1 - First byte of frame */
#define  AddressMask      0x3F  /* destination node address (byte 0)  */
                                /* source      node address (byte 1)  */
#define  DataFrame        0x80

#define  PriorityMask     0x40  /* 0 - low  priority                  */
                                /* 1 - high priority                  */

/* Byte 1 of short or long frame with bit 9 clear */

#define  ShortMsgTypeMask 0x80  /* 0 - long frame                     */
                                /* 1 - short frame                    */

#define  ConfMsgTypeMask  0x40  /* 0 - long  frame unconfirmed        */
                                /*     short frame status             */
                                /* 1 - long  frame confirmed          */
                                /*     short frame looptest           */
#define  UnconfirmMsgType 0x40
#define  ConfirmMsgType   0x00

#define  SyncPacketMsk    0xC0
#define  MsgTypeMask      0xC0  /* 00 - long uncf (not used)          */
                                /* 80 - status                        */
                                /* C0 - looptest byte 3     zero      */
                                /* C0 - syncpack byte 3 non zero      */
#define  StatusMsgType    0x80
#define  LooptestMsgType  0xC0

/* Byte 2 of short frame (data byte of status packet) */

#define  StatusAcceptMask 0x07
#define  StatusAccept     0x00  /* all well (acknowledge)             */
#define  StatusNotReady   0x01  /* receiver not ready                 */
#define  StatusSizeError  0x02  /* receiver buffer too small          */
#define  StatusTimeOut    0x04  /* receiver time out                  */

#define  TimeoutToken       10*MILISECONDE
#define  TimeoutStatus       7*MILISECONDE
#define  TimeoutByte1        3*MILISECONDE
#define  TimeoutTokTok       2*MILISECONDE

#define  BroadcastAddr      62

#define  ArbiMaster       0x01

#define  Off      0
#define  StandBy  1
#define  Active   2


#define  PacketNrsMax       62               /* nr of nodes that can be addressed                    */
#define  SkipPacketNr       0x04             /* this nr marks a double received message              */
#define  SyncPacketNr       0x08             /* synchronise packetnrs before sending datapacket      */
#define  GetPacketNrNr      0x10             /* this nr marks that the packetnr is not filled in yet */
#define  AcceptPacketNr     0x20             // this nr marks that the packet to be accepted

#define  SyncPacketNrMax        0x03

/* Masks and settings for PacketNrs[] */
#define  ClearLowPriTx          0xFC
#define  ClearHighPriTx         0xF3
#define  ClearLowPriRx          0xCF
#define  ClearHighPriRx         0x3F

#define  FirstLowPriTx          0xFD
#define  FirstHighPriTx         0xF7
#define  FirstLowPriRx          0xDF
#define  FirstHighPriRx         0x7F

#define  SyncLowPriTx           0x03
#define  SyncHighPriTx          0x0C
#define  SyncLowPriRx           0x30
#define  SyncHighPriRx          0xC0

#define  SyncLowPriTxMask       0x03
#define  SyncHighPriTxMask      0x0C
#define  SyncLowPriRxMask       0x30
#define  SyncHighPriRxMask      0xC0

#define  PacketNrMask           0x03
#define  ShiftPacketNrToRx      4
#define  ShiftPacketNrToHigh    2
#define  ShiftHighRxToNormal    6
#define  ShiftLowRxToNormal     4
#define  ShiftHighTxToNormal    2

#define  SyncLowPri             0x02
#define  SyncHighPri            0x03
/************************ structure typedef ************************/

typedef struct {                       /* Local status packet area */
         unsigned char Destin;
         unsigned char Source;
         unsigned char Data;
         unsigned char StDummy;
       } StatusPacketType;

typedef struct {
         short  CrcCnt;                 /* number of CRC errors */
         short  ReSync;                 /* number of datalink resync's */
         short  MsgTo;                  /* number of time outs on status packets */
         short  TokTo;                  /* number of time outs on tokens */
         short  BufOvf;                 /* number of buffer overflows at receiver */
       } ErrStatType;

typedef struct {
         short State;                  /* current state network channel */
         short Buffers;                /* number of Tx and Rx ID's/Buffers */
         short Size;                   /* Rx and Tx buffer size */
         short Mode;                   /* arbitration mode */
         signed char  BusAct;          /* busactivity flag */
         signed char  Arbit;           /* arbitration flag */
         signed char  *AnswList;       /* (pointer to) answer list */
       } NetStatType;

typedef struct {                       /* Rx Identification block */
          unsigned short Destin;
          unsigned short Source;
          unsigned short Type;
          unsigned short Length;
          signed char               *DataPntr;
        } RxIdType;

typedef RxIdType  *RxIdPntrType;

typedef RxIdType  *RxIdFifoType;

typedef struct {                       /* type definition Rx control block */
          RxIdFifoType  *Wrp;
          RxIdFifoType  *Rdp;
          RxIdFifoType  *Beg;
          RxIdFifoType  *End;
        } RxCbType;

typedef struct {
          unsigned char  MsgType;
          short          MsgLength;
          signed char    Priority;
          unsigned char  RxDrPacketNr;
          signed char    Destin;
          signed char    Source;
          unsigned short      Crc;
          RxIdPntrType   RxIdPntr;
          signed char    *RxDataPntr;
        } RxDrCbType;

typedef struct TxId {                          /* Tx Identification block */
         struct TxId *Next;
         signed char  Status;
         signed char  Channel;
         short        TxIdSlot;
         signed char  Destin;
         signed char  Source;
         signed char  Type;
         short        Length;
         short               TxIdPoolIx;           /* index in pool for this message */
         unsigned char       TxIdPacketNr;
         unsigned long       TxIdTime;
         signed char  *DataPntr;
       } TxIdType;

typedef TxIdType  *TxIdPntrType;

typedef TxIdType  *TxIdFifoType;

typedef struct {                       /* Tx control block(s) */
         TxIdFifoType *Wrp;
         TxIdFifoType *Rdp;
         TxIdFifoType *Beg;
         TxIdFifoType *End;
       } TxCbType;

typedef struct {
         unsigned char       TxRequest;
         unsigned char       MsgType;
         short               MsgLength;            /* length of message left to be sent */
         signed char         Priority;             /* priority coded as the bits in a packet */
         unsigned char       TxDrPacketNr;         /* packetnr of this message*/
         unsigned short      Crc;                  /* temporary CRC value  */
         TxIdPntrType        TxIdPntr;             /* pointer to Id of current message */
         signed char         *TxDataPntr;
       } TxDrCbType;

typedef struct {                       /* Tx driver control block */
         unsigned char  Node;
         unsigned char       TxPlPacketNr;
         short          RetryCount;
         TxIdPntrType   Next;
         TxIdPntrType   Last;
       } TxPoolType;


/**** network channel data structure ****/

typedef struct {

         unsigned char  *UartAddr;     /* physical address UART */
         unsigned short  TimerInit;    /* Time out initial value */

         short           MemPntr;      /* pointer to allocated memory */
         NetStatType     NetStat;      /* network status */
         ErrStatType     ErrStat;      /* network error status */

         RxCbType        RxCbId;       /* Rx control block free ID's */
         RxCbType        RxCbHi;       /* Rx control block high priority */
         RxCbType        RxCbLo;       /* Rx control block low priority */

         RxDrCbType      RxDrCb;       /* Rx Driver Control Block */

         short           RxState;      /* Rx driver state variable */ 
         

         signed char    ArbNode;       /* actual arbitration node */

         TxCbType        TxCbId;       /* Tx control block free ID's */
         TxCbType        TxCbHi;       /* Tx control block high priority */
         TxCbType        TxCbLo;       /* Tx control block low priority */

         TxDrCbType      TxDrCb;       /* Tx Driver Control Block */

         short           TxPoolIx;     /* Tx pool index */
         TxPoolType      TxPool[TxPoolMax];    /* Tx Pool Blocks */

         short           TxState;      /* Tx driver state variable */
         short           ManyNodes;     /* Respond as more than one node */

         StatusPacketType StatusPacket;/* Status packet */

         unsigned char   PacketNrsRx[PacketNrsMax];  // array with last packetnumbers received
         unsigned char   PacketNrsTx[PacketNrsMax];  // array with last packetnumbers send 
       } NetType;

#endif
