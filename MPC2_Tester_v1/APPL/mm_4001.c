#include "mm_4001.h"
#include "mmmodes.h"
#include "../comm/flowctrl.h"
#include "../lib/time.h"
#include "../lib/string.h"
#include "../lib/eehc11.h"
#include "../appl/milk.h"

extern signed short VflowTemperature;

#if DEBUG_VP4001
const MlkAmntRef TEST_IDLE_DATA[5] = {{  94, 368, 10000,  9800,  9600,  9400,  9200},
                                      {  95, 368, 10200, 10000,  9800,  9600,  9400},
                                      {  98, 368, 10800, 10600, 10400, 10200, 10000},
                                      {  99, 368, 11000, 10800, 10600, 10400, 10200},
                                      { 100, 368, 11200, 11000, 10800, 10600, 10400}};

const MlkAmntRef TEST_MEAS_DATA[45] = {{ 101, 368, 11200, 11200, 11200, 11200, 11200},
                                       { 102, 368, 11200, 11200, 11200, 11200, 11200},
                                       { 103, 368, 11600, 11200, 11000, 10800, 10600},
                                       { 104, 368, 12000, 11600, 11200, 11000, 10800},
                                       { 105, 368, 12400, 12000, 11600, 11200, 11000},
                                       { 106, 368, 12800, 12400, 12000, 11600, 11200},
                                       { 107, 368, 13200, 12800, 12400, 12000, 11600},
                                       { 108, 368, 13600, 13200, 12800, 12400, 12000},
                                       { 109, 368, 14000, 13600, 13200, 12800, 12400},
                                       { 110, 368, 14400, 14000, 13600, 13200, 12800},
                                       { 111, 368, 14800, 14400, 14000, 13600, 13200},
                                       { 112, 368, 15200, 14800, 14400, 14000, 13600},
                                       { 113, 368, 15600, 15200, 14800, 14400, 14000},
                                       { 114, 368, 16000, 15600, 15200, 14800, 14400},
                                       { 115, 368, 16400, 16000, 15600, 15200, 14800},
                                       { 116, 368, 16800, 16400, 16000, 15600, 15200},
                                       { 117, 368, 17200, 16800, 16400, 16000, 15600},
                                       { 118, 368, 17600, 17200, 16800, 16400, 16000},
                                       { 119, 368, 18000, 17600, 17200, 16800, 16400},
                                       { 120, 368, 18400, 18000, 17600, 17200, 16800},
                                       { 121, 368, 18800, 18400, 18000, 17600, 17200},
                                       { 122, 368, 19200, 18800, 18400, 18000, 17600},
                                       { 123, 368, 19600, 19200, 18800, 18400, 18000},
                                       { 124, 368, 20000, 19600, 19200, 18800, 18400},
                                       { 125, 368, 20400, 20000, 19600, 19200, 18800},
                                       { 126, 368, 20800, 20400, 20000, 19600, 19200},
                                       { 127, 368, 21200, 20800, 20400, 20000, 19600},
                                       { 128, 368, 21600, 21200, 20800, 20400, 20000},
                                       { 129, 368, 22000, 21600, 21200, 20800, 20400},
                                       { 130, 368, 22200, 22000, 21600, 21200, 20800},
                                       { 131, 368, 22350, 22200, 22000, 21600, 21200},
                                       { 132, 368, 22400, 22350, 22200, 22000, 21600},
                                       { 133, 368, 22400, 22400, 22350, 22200, 22000},
                                       { 134, 368, 22400, 22400, 22400, 22350, 22200},
                                       { 135, 368, 22400, 22400, 22400, 22400, 22350},
                                       { 136, 368, 22400, 24400, 22400, 22400, 22400},
                                       { 137, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 138, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 139, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 140, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 141, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 142, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 143, 368, 22400, 22400, 22400, 22400, 22400},
                                       { 144, 368, 22400, 22400, 22400, 22400, 22400}};

unsigned char msgCntr;

#endif

unsigned short lastMsgCntr;
unsigned short MlkAmountLast;
unsigned short VP4001MlkAmountStart;
unsigned short VP4001MlkAmountRestart;

#if DEBUG_VP4001
typedef struct {
  void           *msglnk;             /* USX link to next message in mailbox */
  unsigned char   msgpri;             /* USX priority for message in mailbox */
  unsigned char   channel;            /* NETWORK channel of sender */
  unsigned char   address_src;        /* NETWORK address of sender of the message */
  unsigned char   address_dest;       /* NETWORK address of receiver*/
  unsigned char   message_length;     /* length of message */
  unsigned short  message_id;         /* message identifier */
  unsigned short  MlkAmntMsgCntr;
  unsigned short  MlkTemperature;
  struct {
  	unsigned short   MlkAmnt;
  } Array[5];                         /* sizeof(Array) = 10 */
} NETIO_MAIL_MlkAmntRef;

NETIO_MAIL_MlkAmntRef MlkAmntRefBuf;
#endif


// =============================================================================
// isMsgCntrGreater
// =============================================================================
unsigned char isMsgCntrGreater(unsigned short actMsgCntr, unsigned short lastMsgCntr)
{
  if (((signed short)(actMsgCntr - lastMsgCntr )) > 0) return 1;
  else                                                 return 0;
}

// =============================================================================
// HandleMlkAmntRef
// Handles received message
// =============================================================================
void HandleMlkAmntRef(NETIO_MAIL *MailPtr)
{
  unsigned short actMsgCntr;
  unsigned short amnt = 0; 
    signed char  i, j;
	
  MlkAmntRef *MlkAmntRefPtr = (MlkAmntRef *)&(MailPtr->message_start);

  actMsgCntr = MlkAmntRefPtr->MlkAmntMsgCntr;
  VflowTemperature = MlkAmntRefPtr->MlkTemperature;
  
  if (isMilkingActive()) {
    // Put milk amount into milk-table. The table expects an entry every sec. The amounts received
    // are with 5 sec interval. So write the table several times per new entry.
    // The message holds the last 5 measured values, skip already handled values. 
    for (i = 4; i >= 0; i--) {
    	if (isMsgCntrGreater(actMsgCntr - i, lastMsgCntr)) { 
//      if ((actMsgCntr - i) > lastMsgCntr) {
        // VP4001 measures in gramms.
        amnt = MlkAmntRefPtr->Array[i].MlkAmnt;        // gramms
        amnt = (amnt >= VP4001MlkAmountStart) ? (amnt-VP4001MlkAmountStart) : ((0xffff-VP4001MlkAmountStart)+amnt);
        amnt = ((unsigned long)EECorVFlow*(unsigned long)amnt)/100;
        for (j = 0; j < 5; j++) {
      	  PutMlkTbl(amnt,100);           // will calculate flow
        }
      }
    }
  }
  // last received MlkAmount (gramms)
//  if (isMsgCntrGreater(actMsgCntr, lastMsgCntr)) {
  	// MlkAmountLast is used to calc the MlkAmount
    MlkAmountLast = MlkAmntRefPtr->Array[0].MlkAmnt;
    lastMsgCntr = actMsgCntr;
//  }
}

// =============================================================================
// VP4001MMInit
// Initiate varables used for the VP4001 milkmeter.
// =============================================================================
void VP4001MMInit(void)
{
  VP4001MlkAmountRestart = VP4001MlkAmountStart;
  lastMsgCntr = 0;
}

// =============================================================================
// VP4001MMMeasUpdate
// update 0: does not update the milk-amount, only the start amount
//        1: update the milk-amount 
// =============================================================================
void VP4001MMMeasUpdate(unsigned char update)
{
  unsigned short amnt;

  if (update) {
    amnt = MlkAmountLast;
    amnt = (amnt >= VP4001MlkAmountStart) ? ( amnt-VP4001MlkAmountStart) : ((0xffff-VP4001MlkAmountStart)+amnt);
    amnt = ((unsigned long)EECorVFlow*(unsigned long)amnt)/100;
 
    if (isMilkingActive()) {
      MlkAmount = amnt;
      MlkAmountKg = (MlkAmount+50)/100;
    }
  } else {
    VP4001MlkAmountStart = MlkAmountLast;
  }
}

// =============================================================================
// VP4001MMMeas
// update milk-amount 
// =============================================================================
void VP4001MMMeas(void)
{
  VP4001MMMeasUpdate(1);
  UpdateAndSendMlkState(MlkAmount, MlkState);
}

// =============================================================================
// VP4001MMContinue
// milk mode: MM_MEAS(1)
// handles milk states: MLK_CONTINUE(9) 
// =============================================================================
void VP4001MMContinue(void)
{
  VP4001MlkAmountStart = VP4001MlkAmountRestart;
}

// =============================================================================
// VP4001MMDone
// update milk-amount start 
// =============================================================================
void VP4001MMDone(void)
{
  VP4001MMMeasUpdate(0);
  UpdateAndSendMlkState(MlkAmount, MlkState);
}

// =============================================================================
// VP4001MMCleaning
// update milk-amount start 
// =============================================================================
void VP4001MMCleaning(void)
{
  VP4001MMMeasUpdate(0);
}

// =============================================================================
// VP4001MMDefault
// update milk-amount start 
// =============================================================================
void VP4001MMDefault(void)
{
  VP4001MMMeasUpdate(0);
}

#if DEBUG_VP4001
// =============================================================================
// InitMsgCntr
// =============================================================================
void InitMsgCntr(void)
{
  msgCntr = 0;
}

// =============================================================================
// TestMsgCounter
// Test if msg must be used or not; think overrun etc.
// isMsgCntrGreater(actual, last)
// =============================================================================
void TestMsgCounter(void)
{
	unsigned char rval;
	
  rval = isMsgCntrGreater(0x7fff, 0x8000);
  if (rval) {
    msgCntr++;
  } else {
    msgCntr--;      // should return this
  }
  rval = isMsgCntrGreater(0x8000, 0x8000);
  if (rval) {
    msgCntr++;
  } else {
    msgCntr--;      // should return this
  }
  rval = isMsgCntrGreater(0x8001, 0x8000);
  if (rval) {
    msgCntr++;      // should return this
  } else {
    msgCntr--;
  }
  rval = isMsgCntrGreater(0x0000, 0xffff);
  if (rval) {
    msgCntr++;      // should return this
  } else {
    msgCntr--;
  }
  rval = isMsgCntrGreater(0xffff, 0x0000);
  if (rval) {
    msgCntr++;
  } else {
    msgCntr--;      // should return this
  }

}

// =============================================================================
// TestIdleVP4001
// =============================================================================
void TestIdleVP4001(void) 
{
  MlkAmntRef *MlkAmntRefPtr = (MlkAmntRef *)(&MlkAmntRefBuf.MlkAmntMsgCntr);
//	TestMsgCounter();

  if (msgCntr < (sizeof(TEST_IDLE_DATA)/sizeof(MlkAmntRef))) {
    memcpy(MlkAmntRefPtr, (const void *)&(TEST_IDLE_DATA[msgCntr]), sizeof(MlkAmntRef));
    HandleMlkAmntRef((NETIO_MAIL *)&MlkAmntRefBuf);
    msgCntr++;
  }
}

// =============================================================================
// TestMeasureVP4001
// =============================================================================
void TestMeasureVP4001(void) 
{
  MlkAmntRef *MlkAmntRefPtr = (MlkAmntRef *)(&MlkAmntRefBuf.MlkAmntMsgCntr);
  
  if (msgCntr < ((sizeof(TEST_MEAS_DATA)/sizeof(MlkAmntRef))-1)) {
    memcpy(MlkAmntRefPtr, (const void *)&(TEST_MEAS_DATA[msgCntr]), sizeof(MlkAmntRef));
    HandleMlkAmntRef((NETIO_MAIL *)&MlkAmntRefBuf);
    msgCntr++;
  }
}

#endif
