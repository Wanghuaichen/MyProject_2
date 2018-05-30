#include "../lib/stdlib.h"
#include "../lib/string.h"
#include "../lib/schedule.h"
#include "../lib/watchdog.h"
#include "../lib/typedef.h"
#include "../comm/send_msg.h"
#include "../comm/netsend.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/per_func_test_vcp_flow.h"
#include "../net/net_def.h"
#include "../usx/fake_usx.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"

#define WAIT_TIME_NET 999

NET_SEND_MESSAGE MsgBuffer;
NET_SEND_MESSAGE MsgBufferCopy;

unsigned char msgTimeoutTime;
#define MSG_TIMEOUT  5

static BOOLEAN handled;
/*
void SendDbgMessage(unsigned char id, unsigned char msg)
{
  unsigned char  buffer[MESS_LEN(DbgMessRes)+2];
  PerFuncTest    *PerFuncTestPtr;

  void          *DbgMessResPtr = (void *)(buffer + 4);
  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = DbgMessResId;
   
  ((DbgMessRes *)DbgMessResPtr)->DbgMessErrorNr = id;
  ((DbgMessRes *)DbgMessResPtr)->DbgMlkState = msg;

  SendMessage( ConLow, GetNodeAddress(0), 0, MESS_LEN(DbgMessRes)+2, buffer);
}
*/

// =============================================================================
void HndlNetSend(void)
{
  TxIdType *txid_ptr;
  int status;
  unsigned short Counter0=0, Counter1=0, Counter2=0, Counter3=0;
  handled = TRUE;
  
  msgTimeoutTime = MSG_TIMEOUT;

  while(1)
  {
  	WatchDog();
  	
    // previous message handled?
    if (handled == TRUE){
      // Yes; check for new
      UsxGetRes(NET_BUF_RES);
      if (MsgBuffer.Channel == 1){
        memcpy(&MsgBufferCopy, &MsgBuffer, sizeof(MsgBufferCopy));
        MsgBuffer.Channel = 0;                       // indicates buffer free
        Counter0=0, Counter1=0, Counter2=0, Counter3=0;
        handled = FALSE;
      }
      UsxRelRes(NET_BUF_RES);
    }

    // send a message to signal communication loss
//    if ((handled==TRUE) && (msgTimeoutTime < 100) && (dbgSend == FALSE)){
//    	SendDbgMessage(FNC_COMM_LOSS, MlkState);
//    	dbgSend = TRUE;
//    }
    
    while(handled==FALSE){
      txid_ptr = NetSend(1, MsgBufferCopy.SrcAddress, MsgBufferCopy.DestAddress, MsgBufferCopy.Type,
                            MsgBufferCopy.Length, (msgTimeoutTime==0?1:msgTimeoutTime), (unsigned char *)(MsgBufferCopy.Data));


      /* bepaal aan de hand van de status wat er gedaan moet worden -----------------------------------------------*/
      while(handled==FALSE){
        status = NetSendStat(txid_ptr);                 // TxIdPntr->Status

        switch (status) {
          case  Busy:
                TaskDelay(1);
                WatchDog();
                if (++Counter0 >= WAIT_TIME_NET) {
                  if (msgTimeoutTime) msgTimeoutTime--;
                  NetSendRel(txid_ptr);
                  handled = TRUE;
                  TaskExit();
                }
            break;   
          case  Ready:
                msgTimeoutTime = MSG_TIMEOUT;
                NetSendRel(txid_ptr);
                handled = TRUE;
                TaskExit();
            break;
          case  TxNoBuf:
                TaskDelay(1);
                WatchDog();
                if (++Counter1 == WAIT_TIME_NET) {
                  NetSendRel(txid_ptr);
                  handled = TRUE;
                  TaskExit();
                }
            break;
          case  RxTimeOut:
                TaskDelay(1);
                WatchDog();
                if (++Counter2 >= ((TxIdType *)txid_ptr->Destin == 0 ? WAIT_TIME_NET : WAIT_TIME_NET/3)) {
                  NetSendRel(txid_ptr);
                  TaskExit();
                  handled = TRUE;
                }
            break;
          case  RxNoBuf:
                TaskDelay(1);
                WatchDog();
                if (++Counter3 == WAIT_TIME_NET) {
                  NetSendRel(txid_ptr);
                  handled = TRUE;
                  TaskExit();
                }
            break;
  
          case  ApplSendTimeout:
//                NetSendRel(txid_ptr);       // useless; returns pointer to flash
                if (msgTimeoutTime) msgTimeoutTime--;
                handled = TRUE;
                TaskExit();
            break;
          case  MsgTooLong:
                NetSendRel(txid_ptr);
                handled = TRUE;
                TaskExit();
            break;
          case  CallError:
                handled = TRUE;
                TaskExit();
            default:
                NetSendRel(txid_ptr);
                handled = TRUE;
                TaskExit();
             break;
        }
      }
    } // end while(handled)
    TaskExit();
  } // end while(1)
}
