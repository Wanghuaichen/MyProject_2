/*******************************************************************/
/*                                                                 */
/*     ext_net.  h                                                 */
/*                                                                 */
/*     External declaration of netwerk interface procedures        */
/*                                                                 */
/*     Author :  L.A. Boulogne                                     */
/*     Date   :  21-10-1991                                        */
/*                                                                 */
/*******************************************************************/
#include "../net/net_def.h"

#ifndef EXT_NET_INCLUDED
#define EXT_NET_INCLUDED        1


enum net_message_type {
  ConLo = 1,          /* Confirmed Low priority */
  ConHi,              /* Confirmed High priority */
  UncLo,              /* Unconfirmed Low priority */
  UncHi,              /* Unconfirmed High priority */
  TstConLo,           /* Test Confirmed Low priority */
  TstConHi,           /* Test Confirmed High priority */
  TstUncLo,           /* Test Unconfirmed Low priority */
  TstUncHi,           /* Test Unconfirmed High priority */
  LoopTst             /* Loop Test */
  } ;

typedef signed short NET_MESSAGE_TYPE;

enum net_status {
  Ready = 0,          /* Opdracht met succes uitgevoerd */
  Busy,               /* Opdracht in behandeling */
  TxNoBuf,            /* Zender heeft geen buffer */
  RxTimeOut,          /* Ontvanger reageert niet, time out */
  RxNoBuf,            /* Ontvanger heeft geen buffer */
  MsgTooLong,         /* Ontvanger buffer kleiner dan bericht lengte */
  CallError,          /* Fout in functie aanroep */
  InternNetState,     /* internal status         */
  ApplSendTimeout     /* application timeout     */
  } ;
typedef signed short NET_STATUS;

#define         Master          1;
#define         Slave           0;


extern void          NetIni(void);
extern signed short  NetInit(signed short Channel,
                             signed short Buffers,
                             signed short BufSize,
                             signed short Node,
                             signed short Mode,
                             signed char *NodeList);

extern signed short  NetOpen(signed short Channel);
extern signed short  NetClose(signed short Channel);
extern void          NetInitStat(signed short Channel);
extern ErrStatType  *NetErrStat(signed short Channel);

extern TxIdType     *NetSend(signed short Channel,
                             signed short SrcAddress,
                             signed short DestAddress,
                             NET_MESSAGE_TYPE Type,
                             signed short Length,
                             unsigned short Timeout,
                             const unsigned char *DataPntr);

extern signed short  NetSendStat(TxIdType *TxIdPntr);
extern void          NetSendRel(TxIdType *TxIdPntr);
extern signed short  NetReceive(signed short Channel,
                                signed short *AddressSrc,
                                signed short *AddressDest,
                                signed short *Type,
                                unsigned char *Message);

extern signed short NetCheckSending(short Channel);

#endif

