/*******************************************************************/
/*                                                                 */
/*    AMICE  4.0                                                   */
/*                                                                 */
/*    external declaration of global variables                     */
/*                                                                 */
/*******************************************************************/
#ifndef LOCAL_NET_INCLUDED
#define LOCAL_NET_INCLUDED 1


#include "../net/net_def.h"

extern NetType             Net;
extern unsigned long       NetApplSendTimeout;

extern const short         NetSize;
extern const unsigned char TokenCode[64];
extern const unsigned char PriorCode[10];
extern const unsigned char TypeCode[10];
extern const TxIdType      TxStsReady;
extern const TxIdType      TxStsNoBuf;
extern const TxIdType      TxStsError;
extern const TxIdType      TxStsApplTimeout;

extern void                SendNextToken(void);
extern signed char         CheckToken(signed char Token);
extern signed char         CheckNode(signed char Node);

extern RxIdType       *GetFreeRxId(NetType *NetPntr);
extern RxIdType       *GetUsedRxId(RxCbType *RxCbPntr);
extern void            PutUsedRxId(NetType *NetPntr);
extern void            PutFreeRxId(NetType *NetPntr,RxIdType *RxIdPntr);
extern void            IntPutFreeRxId(NetType *NetPntr,RxIdType *RxIdPntr);

extern TxIdType       *GetFreeTxId(NetType *NetPntr);
extern void            PutUsedTxId(TxCbType *TxCbPntr, TxIdType *TxIdPntr);
extern void            PutFreeTxId(NetType *NetPntr, TxIdType *TxIdPntr);
extern void            IntPutFreeTxId(NetType *NetPntr, TxIdType *TxIdPntr);

extern TxIdType       *FindTxReq(NetType *NetPntr);
extern TxIdType       *FreeTxReq(NetType *NetPntr);


extern void            NNetRx(void);
extern void            NetTx(void);
extern void            NetRx(void);

extern void            NetTimerTick(void);
extern void            Crc12(unsigned char Byte,unsigned short *CrcPntr);
extern non_banked short           CertifyRelease(signed char *);


extern unsigned char UartIntMaskStatus;
extern unsigned char TimerIntMaskStatus; 

#endif 

