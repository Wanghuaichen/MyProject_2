// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// =============================================================================

// include files ---------------------------------------------------------------

#include "../appl/conduct.h"
#include "../appl/feed.h"
#include "../appl/temp.h"
#include "../appl/tandem.h"
#include "../appl/pulsation.h"
#include "../appl/mm_4001.h"
#include "flowctrl.h"
#include "config.h"
#include "flowrcv.h"

#include "../include/romver.h"
#include "../include/cstartup.h"
#include "../include/romeehc1.h"
#include "../include/romstnet.h"
#include "../include/romdwnld.h"

#include "../lib/rti_int.h"
#include "../lib/applver.h"
#include "../lib/eeprom.h"
#include "../lib/extport.h"
#include "../lib/eehc11.h"
#include "../lib/stdlib.h"
#include "../lib/string.h"
#include "../lib/boolean.h"
#include "../lib/burnin.h"
#include "../lib/schedule.h"
#include "../lib/extport.h"
#include "../lib/watchdog.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../init/start.h"
#include "../comm/send_msg.h"
#include "../comm/startnet.h"
#include "../comm/startnet.h"
#include "../usx/fake_usx.h"
#include "../flowlib/per_func_test_vcp_flow.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../flowlib/periph_vcp_flow.h"
#include "../flowlib/system_vcp_flow.h"
#include "../flowlib/transfer_vcp_flow.h"
#include "../flowlib/separation_vcp_flow.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/activity_vcp_flow.h"
#include "../flowlib/mat_robot_vcp_flow.h"
#include "../flowlib/cond_temp_vcp_flow.h"
#include "../flowlib/disease_weight_vcp_flow.h"
#include "../flowlib/test_mpc1_flow.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/cow_herd_vcp_flow.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../key/disp.h"
#include "../key/key.h"
#include "../key/key_input.h"
#include "../net/ext_net.h"

#pragma codeseg(EEPAGE)


#define MAN_INPUT_REF    0
#define MAN_INPUT_MANUAL 1
#define MAN_INPUT_UPDATE 2


extern unsigned char msgTimeoutTime;

BOOLEAN CondMessageRcv;

// function prototype ==========================================================
void HndlSysGenAbsTimeBroadcast(NETIO_MAIL *MailPtr);
void HndlPerGenNumberRef(NETIO_MAIL *MailPtr);
void HndlPerGenRef(NETIO_MAIL *MailPtr);
void HndlPerGenInstResReq(NETIO_MAIL *MailPtr);
void HndlDldPerEraseRef(NETIO_MAIL *MailPtr);
void HndlDldPerProgRef(NETIO_MAIL *MailPtr);
void HndlDldPerProgResReq(NETIO_MAIL *MailPtr);
void HndlPerPresenceCheck(void);
void HndlMailBox(NETIO_MAIL *MailPtr);


unsigned char ConCor1;
unsigned char ConCor2;
unsigned char ConCor3;
unsigned char ConCor4;

NETIO_MAIL          *MailBox[MAX_MAIL_BOXES];

HerdConRef *p_HerdConRef;
void       *p_CowActRef[MAX_NODES];
void       *p_CowConRef[MAX_NODES];
void       *p_CowTmpRef[MAX_NODES];
void       *p_CowGenRef[MAX_NODES];
void       *p_CowMlkPlsRef[MAX_NODES];
void       *p_CowActMemAttRef[MAX_NODES];
void       *p_ExtendedMlkRes[MAX_NODES];

// ivm swing over en voeren
void       *p_CowFedParlRef[MAX_NODES];
void       *p_CowFedParlRes[MAX_NODES];

NETIO_MAIL          *p_CowSepInfoResMail[MAX_NODES];
NETIO_MAIL          *p_CowSepInfoMail[MAX_NODES];
NETIO_MAIL          *p_CowFedInfoMail[MAX_NODES];
NETIO_MAIL          *p_CowDiseaseMail[MAX_NODES];
NETIO_MAIL          *p_CowFedParlInfoMail[MAX_NODES];

unsigned char  CowDisease[MAX_NODES][14];
unsigned char  Get_Cow_Refs;
unsigned char  CowRefSide;

// =============================================================================
void init_flowctrl(void)
{
  // zet correctie factors geleidbaarheid op neutraal
  ConCor1 = 100;
  ConCor2 = 100;
  ConCor3 = 100;
  ConCor4 = 100;
  Get_Cow_Refs = 0;
}




/****************************************************************/
/*HndlPerGenRes Message                                         */
/****************************************************************/

void HndlPerGenRes(void)
{
  unsigned char Buffer[MESS_LEN(PerGenRes)];

  PerGenRes *PerGenResPtr = (PerGenRes *)(Buffer +2);


  // zend nieuw pergenref
  *((unsigned short *)Buffer) = PerGenResId;
//  PerGenResPtr->PerGenNumber = PerGenNumber;
  PerGenResPtr->PerGenBlock1  = EEDummy1;
  PerGenResPtr->PerGenBlock2  = EEMtrRow;
  PerGenResPtr->PerGenBlock3  = EEClxTime;
  PerGenResPtr->PerGenBlock4  = EEDetectPerNr;
  PerGenResPtr->PerGenBlock5  = EECal1;
  PerGenResPtr->PerGenBlock6  = EECal2;
  PerGenResPtr->PerGenBlock7  = EECal3;
  PerGenResPtr->PerGenBlock8  = EECal4;
  PerGenResPtr->PerGenBlock9  = EECal5;
  PerGenResPtr->PerGenBlock10 = EEConCor1;
  PerGenResPtr->PerGenBlock11 = EEConCor2;
  PerGenResPtr->PerGenBlock12 = EEConCor3;
  PerGenResPtr->PerGenBlock13 = EEConCor4;
  PerGenResPtr->PerGenBlock14 = (EEExtMode<<8)|EEMlkStateUpdateCntr;


  if (EEAddressCopy != 0) {
    PerGenResPtr->PerGenNumber = PerGenNumber[0];
    while (SendMessage(ConLow, EEAddressCopy, 0, sizeof(Buffer), Buffer) == 0)TaskExit();
  }

  if (EEAddress2 != 0) {
    PerGenResPtr->PerGenNumber = PerGenNumber[1];
    while(SendMessage(ConLow, EEAddress2, 0, sizeof(Buffer), Buffer) == 0)TaskExit();
  }
}

// =============================================================================
// Handle MlkRefReq
// =============================================================================
unsigned char HndlMlkRefReq(unsigned long num, unsigned char Type, unsigned char idx)
{
  unsigned char  Buffer[MESS_LEN(ExtMlkRefReq)];
  unsigned char  null;
  unsigned char rval = 1;
  unsigned short length, address;

  void *p_MlkRefReq = (void *)(Buffer + 2);

  if (msgTimeoutTime==0){
    // communication error
    FreeMlkRef(idx, FNC_COWS_ONE_TO_NONE);
    MlkRefState[idx] = COWS_NONE;
  }

  null = 0; if (Type == COWNULL_REF) {null = 1; Type = MANUAL_REF;}
  switch (Type) {
    case NORMAL_REF : *((unsigned short *)Buffer) = LONG_COW_GEN_NUMBER ? ExtMlkRefReqId       : MlkRefReqId;       break;
    case MANUAL_REF : *((unsigned short *)Buffer) = LONG_COW_GEN_NUMBER ? ExtMlkManualRefReqId : MlkManualRefReqId; break;
    default : break;
  }

  if (idx != -1) {
    if (LONG_COW_GEN_NUMBER ){
     ((ExtMlkRefReq *)p_MlkRefReq)->PerGenNumber  = p_PerGenRef[idx]->PerGenNumber;
    } else {
     ((   MlkRefReq *)p_MlkRefReq)->PerGenNumber = p_PerGenRef[idx]->PerGenNumber;
    }
    if (null) {
      length = (LONG_COW_GEN_NUMBER ? MESS_LEN(ExtMlkRefReq) : MESS_LEN(MlkRefReq)) - 2;
    } else {
      length = (LONG_COW_GEN_NUMBER ? MESS_LEN(ExtMlkRefReq) : MESS_LEN(MlkRefReq));
      if ( LONG_COW_GEN_NUMBER ){
        ((ExtMlkRefReq *)p_MlkRefReq)->CowGenNumber = num;
      } else {
        ((   MlkRefReq *)p_MlkRefReq)->CowGenNumber = (unsigned short)num;
      }
    }
    address = (LONG_COW_GEN_NUMBER ?
      ((ExtMlkRefReq *)p_MlkRefReq)->PerGenNumber :
      ((   MlkRefReq *)p_MlkRefReq)->PerGenNumber) % 100;
    rval = SendMessage( ConLow, address, 0, length, Buffer);
  }
  return rval;           // 1 == OK
}

// =============================================================================
// Handle MlkRef
// =============================================================================
void HndlMlkRef(NETIO_MAIL *MailPtr)
{
  unsigned long cow;
  unsigned char offset = 0;
    signed char idx;


  void *MlkRefPtr = ( void *)( &( MailPtr->message_start));

  idx = GetNodeIndex(MailPtr->address_dest);
  if (idx == -1) {goto exit;}

  // item (bool) added to flow; check if old flow received
  if ( (MailPtr->message_length+2) ==
      ( LONG_COW_GEN_NUMBER ? sizeof(ExtMlkRef) : sizeof(MlkRef)))  offset = 2;

  if ((MailPtr->message_length+offset) <
      ( LONG_COW_GEN_NUMBER ? sizeof(ExtMlkRef) : sizeof(MlkRef)))  cow = 0;
  else    cow =
     LONG_COW_GEN_NUMBER ? ((ExtMlkRef *)MlkRefPtr)->CowGenNumber :
                     (long)((   MlkRef *)MlkRefPtr)->CowGenNumber;

  StopScheduler();

  switch (MlkRefState[idx]) {

    case COWS_NONE :
         if (cow > 0) {
           if ( LONG_COW_GEN_NUMBER ){
              if ((p_MlkRef[idx] != NULL) && (cow != ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber) || (p_MlkRef[idx] == NULL)){
               // bewaar momemt de koe binnen komt
               CowMlkYldStartTime[idx] = AbsTime;
               CowMlkYldManInput[idx]  = (MailPtr->message_id == ExtMlkManualRefId) ? MAN_INPUT_MANUAL : MAN_INPUT_REF;
             }
           } else {
             if (( p_MlkRef[idx] != NULL) && ((unsigned short) cow != ((MlkRef **)p_MlkRef)[idx]->CowGenNumber) || (p_MlkRef[idx] == NULL)){
               // bewaar momemt de koe binnen komt
               CowMlkYldStartTime[idx] = AbsTime;
               CowMlkYldManInput[idx]  = (MailPtr->message_id == MlkManualRefId) ? MAN_INPUT_MANUAL : MAN_INPUT_REF;
             }
           }
           // MlkManualRef or Mlkref
           FreeMlkRef(idx, FNC_COWS_NONE_TO_ONE);
           while ( p_MlkRef[idx] == NULL ) { p_MlkRef[idx] = calloc(1, (MailPtr->message_length+offset)); }
           memcpy( p_MlkRef[idx], MlkRefPtr, MailPtr->message_length );
           if (offset){
             if ( LONG_COW_GEN_NUMBER ){
               ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
             } else {
               ((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
             }
           }
           MlkRefState[idx] = COWS_ONE;
         }
      break;
    case COWS_ONE :
         if ( (MailPtr->message_id != ((LONG_COW_GEN_NUMBER) ? ExtMlkRefId : MlkRefId)) || (MeasState[idx] == MEAS_IDLE) ) {
           // MlkManualRef or MlkUpdateRef
           if (cow > 0) {
             FreeMlkRef(idx, FNC_COWS_ONE_TO_ONE);
             while ( p_MlkRef[idx] == NULL ) { p_MlkRef[idx] = calloc(1, (MailPtr->message_length+offset) ); }
             memcpy( p_MlkRef[idx], MlkRefPtr, MailPtr->message_length );
             if (offset){
               if ( LONG_COW_GEN_NUMBER ){
                 ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               } else {
                 ((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               }
             }

             // koe staat er al, tijd hoeft niet te worden geupdate.
             CowMlkYldManInput[idx]  = (MailPtr->message_id == (LONG_COW_GEN_NUMBER ?
                                          ExtMlkManualRefId :
                                             MlkManualRefId) ? MAN_INPUT_MANUAL : MAN_INPUT_UPDATE);

           } else {
             FreeMlkRef(idx, FNC_COWS_ONE_TO_NONE);
             MlkRefState[idx] = COWS_NONE;
           }
           break;
         } else {
           MlkRefState[idx] = COWS_TWO;
         }
         // no break, behoort hier door te gaan
    case COWS_TWO :
         if ( MailPtr->message_id == ((LONG_COW_GEN_NUMBER) ? ExtMlkManualRefId : MlkManualRefId) ) {
           if ( (cow > 0) &&
               ((LONG_COW_GEN_NUMBER) ? ( cow != ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber) :
                         ((unsigned short)cow != ((   MlkRef **)p_MlkRef)[idx]->CowGenNumber) )) {
             FreeMlkRef(idx, FNC_COWS_TWO_TO_ONE);
             while ( p_MlkRef[idx] == NULL ) { p_MlkRef[idx] = calloc(1, (MailPtr->message_length+offset)); }
             memcpy( p_MlkRef[idx], MlkRefPtr, MailPtr->message_length );
             if (offset){
               if ( LONG_COW_GEN_NUMBER ){
                 ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               } else {
                 ((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               }
             }
             MlkRefState[idx] = COWS_ONE;

             // koe staat er al, tijd hoeft niet te worden geupdate.
             CowMlkYldManInput[idx]  = MAN_INPUT_MANUAL;

           } else {
             FreeMlkRef(idx, FNC_COWS_TWO_TO_NONE);
             MlkRefState[idx] = COWS_NONE;
           }
           FreeMlkRefB(idx);
         } else {
           // MlkRef or MlkUpdateRef
           FreeMlkRef(-1, FNC_COWS_TWO_TO_TWO);
           FreeMlkRefB(idx);
           if ( (cow > 0) &&
                 (cow != ((LONG_COW_GEN_NUMBER) ? ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber :
                                                  ((   MlkRef **)p_MlkRef)[idx]->CowGenNumber)) ) {
             while ( p_MlkRef_B[idx] == NULL ) { p_MlkRef_B[idx] = calloc(1, (MailPtr->message_length+offset)); }
             memcpy( p_MlkRef_B[idx], MlkRefPtr, MailPtr->message_length );
             if (offset){
               if ( LONG_COW_GEN_NUMBER ){
                 ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               } else {
                 ((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0xff;
               }
             }

             // bewaar momemt de koe binnen komt
             CowMlkYldStartTime_B[idx] = AbsTime;
             CowMlkYldManInput_B[idx]  = (MailPtr->message_id == ((LONG_COW_GEN_NUMBER) ? ExtMlkRefId : MlkRefId)) ? MAN_INPUT_REF : MAN_INPUT_UPDATE;

             // clear milk resultaat andere zijde
             if ( idx != SideState ) MlkResOtherSide = 0;

           } else {
             FreeMlkRef(-1, FNC_COWS_TWO_TO_ONE);
             MlkRefState[idx] = COWS_ONE;
           }
         }
      break;
    default :
      break;
  }

  // stop voeren op melkplaats zonder koe!!!!
  if ( MlkRefState[idx] == COWS_NONE ){
    if (p_CowFedParlRef[idx] != NULL) { free(p_CowFedParlRef[idx] ); p_CowFedParlRef[idx] = NULL; }
    if (p_CowFedParlRes[idx] != NULL) { free(p_CowFedParlRes[idx] ); p_CowFedParlRes[idx] = NULL; }
  }

  // vraag ook alle data andere zijde op ( swing-over )
  if (MlkRefState[idx] == COWS_ONE && cow) {
    if ( (MailPtr->message_id != ((LONG_COW_GEN_NUMBER) ? ExtMlkRefId : MlkRefId )) &&
       (p_CowFedParlRes[idx] != NULL) ){
      // er is geschoven en er was al gevoerd, dus tegoed afboeken bij geplaatste koe
      while (HndlCowFedParlRes(idx) == 0)TaskExit();
    }
    SetGetCowRefs(idx);
  }

  ContScheduler();

exit: ;

}


// =============================================================================
// Handle ActRefReq
// =============================================================================
unsigned char HndlCowActRefReq(unsigned char Side)
{
  unsigned short len;
  unsigned char  Buffer[MESS_LEN(ExtCowActRefReq)];
  void *p_CowActRefReq = (void *)(Buffer + 2);

  if (p_CowActRef[Side] != NULL) {free(p_CowActRef[Side]); p_CowActRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)  = ExtCowActRefReqId;
    ((ExtCowActRefReq *)p_CowActRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowActRefReq *)p_CowActRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(ExtCowActRefReq);
  } else {
    *((unsigned short *)Buffer)  = CowActRefReqId;
    ((CowActRefReq *)p_CowActRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowActRefReq *)p_CowActRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(CowActRefReq);
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, len, Buffer);
}


// =============================================================================
// Handle ActRef
// =============================================================================
void HndlCowActRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side   = GetNodeIndex(MailPtr->address_dest);

  void *CowActRefPtr = ( void *)( &( MailPtr->message_start));

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowActRef): sizeof(CowActRef))) {goto exit;}  // bericht is te klein !!!

  StopScheduler();

  if (p_CowActRef[Side] != NULL) {free(p_CowActRef[Side]); p_CowActRef[Side] = NULL;}
  while (p_CowActRef[Side] == NULL) {p_CowActRef[Side] = calloc(1, MailPtr->message_length);}
  memcpy(p_CowActRef[Side], CowActRefPtr, MailPtr->message_length);

  ContScheduler();

  // verstuur automatisch een separatie bericht indien er een attentie is en dit ook gewenst is
  if ( LONG_COW_GEN_NUMBER ? ((ExtCowActRef **)p_CowActRef)[Side]->CowActAttention : ((CowActRef **)p_CowActRef)[Side]->CowActAttention ) {
    if ( (SoftKey & SK_STIM_TRCK_ARM_ENABLE) && (EESepMsgCntrl & SEND_SEP_MSG_ACT) ){
      // deze functie handelt verder alles af
      HandleKeySave(EESepActFuncNr ,SM_COW_INFO, SCREEN_COW_INFO_1, 1, Side, 0xff);
    }
  }

exit: ;

}

// =============================================================================
// Handle HerdConRefReq
// =============================================================================
unsigned char HndlHerdConRefReq( void )
{
  unsigned char  Buffer[MESS_LEN(HerdConRefReq)];
  HerdConRefReq  *p_HerdConRefReq = (HerdConRefReq *)(Buffer + 2);

  if (p_HerdConRef != NULL) {free(p_HerdConRef); p_HerdConRef = NULL;}

  *((unsigned short *)Buffer)   = HerdConRefReqId;
  p_HerdConRefReq->PerGenNumber = PerGenNumber[0];

  return SendMessage( ConLow, GetNodeAddress(0), 0, MESS_LEN(HerdConRefReq), Buffer);
}


// =============================================================================
void HndlHerdConRef(NETIO_MAIL *MailPtr)
{
  HerdConRef *HerdConRefPtr = ( HerdConRef *)( &( MailPtr->message_start));

  if (MailPtr->message_length < sizeof(HerdConRef)) {goto exit;}  // bericht is te klein !!!

  StopScheduler();

  if (p_HerdConRef != NULL) {free(p_HerdConRef); p_HerdConRef = NULL;}
  while (p_HerdConRef == NULL) {p_HerdConRef = calloc(1, MailPtr->message_length);}
  memcpy(p_HerdConRef, HerdConRefPtr, MailPtr->message_length);

  if ( p_HerdConRef->HerdConNrOfValues > 40 )p_HerdConRef->HerdConNrOfValues = 40;

  ContScheduler();

exit: ;

}


// =============================================================================
// Handle CowConRefReq
// =============================================================================
unsigned char HndlCowConRefReq(unsigned char Side)
{
  unsigned short len;
  unsigned char  Buffer[MESS_LEN(ExtCowConRefReq)];
  void  *p_CowConRefReq = (void *)(Buffer + 2);

  if (p_CowConRef[Side] != NULL) {free(p_CowConRef[Side]); p_CowConRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)  = ExtCowConRefReqId;
    ((ExtCowConRefReq *)p_CowConRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowConRefReq *)p_CowConRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(ExtCowConRefReq);
  } else {
    *((unsigned short *)Buffer)  = CowConRefReqId;
    ((CowConRefReq *)p_CowConRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowConRefReq *)p_CowConRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(CowConRefReq);
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, len, Buffer);
}

// =============================================================================
// Handle CowGenRefReq
// =============================================================================
unsigned char HndlCowGenRefReq(unsigned char Side)
{
  unsigned short len;
  unsigned char  Buffer[MESS_LEN(ExtCowGenRefReq)];
  void  *p_CowGenRefReq = (void *)(Buffer + 2);

  if (p_CowGenRef[Side] != NULL) {free(p_CowGenRef[Side]); p_CowGenRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)  = CowGenRefReqId;
    ((ExtCowGenRefReq *)p_CowGenRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowGenRefReq *)p_CowGenRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(ExtCowGenRefReq);
  } else {
    *((unsigned short *)Buffer)  = CowGenRefReqId;
    ((CowGenRefReq *)p_CowGenRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowGenRefReq *)p_CowGenRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    len = MESS_LEN(CowGenRefReq);
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, len, Buffer);
}


// =============================================================================
// Handle CowGenRes
// =============================================================================
unsigned char HndlCowGenRes( unsigned char Side)
{
  unsigned short len;
  unsigned char  Buffer[MESS_LEN(ExtCowGenRes)];
  void *CowGenResPtr = (void *)(Buffer +2);

  // zend nieuw pergenres
  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                       = ExtCowGenResId;
    ((ExtCowGenRes *)CowGenResPtr)->CowGenNumber        = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    ((ExtCowGenRes *)CowGenResPtr)->CowGenNextGrpNumber = ((ExtCowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber;
    len = MESS_LEN(ExtCowGenRes);
  } else {
    *((unsigned short *)Buffer)       = CowGenResId;
    ((CowGenRes *)CowGenResPtr)->CowGenNumber        = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    ((CowGenRes *)CowGenResPtr)->CowGenNextGrpNumber = ((CowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber;
    len = MESS_LEN(CowGenRes);
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, len, Buffer);
}


// =============================================================================
// Handle CowConRef
// =============================================================================
void HndlCowConRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;
  void *CowConRefPtr = ( void *)( &( MailPtr->message_start));

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowConRef) : sizeof(CowConRef))) {goto exit;}  // bericht is te klein !!!

  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowConRef[Side] != NULL) {free(p_CowConRef[Side]); p_CowConRef[Side] = NULL;}
  while (p_CowConRef[Side] == NULL) {p_CowConRef[Side] = calloc(1, MailPtr->message_length);}
  memcpy(p_CowConRef[Side], CowConRefPtr, MailPtr->message_length);

  if ( (LONG_COW_GEN_NUMBER ? ((ExtCowConRef **)p_CowConRef)[Side]->HerdConNrOfValues : ((CowConRef **)p_CowConRef)[Side]->HerdConNrOfValues) > 40 ){
    if (LONG_COW_GEN_NUMBER ){
      ((ExtCowConRef **)p_CowConRef)[Side]->HerdConNrOfValues = 40;
    } else {
      ((   CowConRef **)p_CowConRef)[Side]->HerdConNrOfValues = 40;
    }
  }
  ContScheduler();

exit: ;
}

// =============================================================================
// Handle CowConRef
// =============================================================================
void HndlCowGenRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  void *CowGenRefPtr = ( void *)( &( MailPtr->message_start));
  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowGenRef) : sizeof(CowGenRef))) {goto exit;}  // bericht is te klein !!!
  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowGenRef[Side] != NULL) {free(p_CowGenRef[Side]); p_CowGenRef[Side] = NULL;}
  while (p_CowGenRef[Side] == NULL) {p_CowGenRef[Side] = calloc(1, MailPtr->message_length);}
  memcpy(p_CowGenRef[Side], CowGenRefPtr, MailPtr->message_length);

  if ( LONG_COW_GEN_NUMBER ){
    ((ExtCowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber = ((ExtCowGenRef **)p_CowGenRef)[Side]->CowGenGrpNumber;
  } else {
    ((CowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber = ((CowGenRef **)p_CowGenRef)[Side]->CowGenGrpNumber;
  }
  ContScheduler();

exit: ;
}

// =============================================================================
// Handle Cow Conductivity Result
// =============================================================================
unsigned char HndlCowConRes(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtCowConRes)];
  HVC_BUFFER_INFO *p = HVC_Buffer;
  unsigned char rval = 1;

  if ( CBWptr && p_CowConRef[Side] ){
    void *CowConResPtr = (void *)(Buffer +2);

    StopScheduler();

    // zend nieuw CowConRes
    if ( LONG_COW_GEN_NUMBER ){
      *((unsigned short *)Buffer)         = ExtCowConResId;
      ((ExtCowConRes *)CowConResPtr)->PerGenNumber          = PerGenNumber[Side];
      ((ExtCowConRes *)CowConResPtr)->CowGenNumber          = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
      ((ExtCowConRes *)CowConResPtr)->CowConDataValue1      = p[0].average;
      ((ExtCowConRes *)CowConResPtr)->CowConDataValue2      = p[1].average;
      ((ExtCowConRes *)CowConResPtr)->CowConDataValue3      = p[2].average;
      ((ExtCowConRes *)CowConResPtr)->CowConDataValue4      = p[3].average;
      ((ExtCowConRes *)CowConResPtr)->CowConRollingAverage1 = (p[0].r_average == 0 ? ((ExtCowConRef **)p_CowConRef)[Side]->CowConRollingAverage1 : p[0].r_average);
      ((ExtCowConRes *)CowConResPtr)->CowConRollingAverage2 = (p[1].r_average == 0 ? ((ExtCowConRef **)p_CowConRef)[Side]->CowConRollingAverage2 : p[1].r_average);
      ((ExtCowConRes *)CowConResPtr)->CowConRollingAverage3 = (p[2].r_average == 0 ? ((ExtCowConRef **)p_CowConRef)[Side]->CowConRollingAverage3 : p[2].r_average);
      ((ExtCowConRes *)CowConResPtr)->CowConRollingAverage4 = (p[3].r_average == 0 ? ((ExtCowConRef **)p_CowConRef)[Side]->CowConRollingAverage4 : p[3].r_average);
      ((ExtCowConRes *)CowConResPtr)->CowConDataAttentions  = ConDataAttentions;
    } else {
      *((unsigned short *)Buffer)                        = CowConResId;
      ((CowConRes *)CowConResPtr)->PerGenNumber          = PerGenNumber[Side];
      ((CowConRes *)CowConResPtr)->CowGenNumber          = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
      ((CowConRes *)CowConResPtr)->CowConDataValue1      = p[0].average;
      ((CowConRes *)CowConResPtr)->CowConDataValue2      = p[1].average;
      ((CowConRes *)CowConResPtr)->CowConDataValue3      = p[2].average;
      ((CowConRes *)CowConResPtr)->CowConDataValue4      = p[3].average;
      ((CowConRes *)CowConResPtr)->CowConRollingAverage1 = (p[0].r_average == 0 ? ((CowConRef **)p_CowConRef)[Side]->CowConRollingAverage1 : p[0].r_average);
      ((CowConRes *)CowConResPtr)->CowConRollingAverage2 = (p[1].r_average == 0 ? ((CowConRef **)p_CowConRef)[Side]->CowConRollingAverage2 : p[1].r_average);
      ((CowConRes *)CowConResPtr)->CowConRollingAverage3 = (p[2].r_average == 0 ? ((CowConRef **)p_CowConRef)[Side]->CowConRollingAverage3 : p[2].r_average);
      ((CowConRes *)CowConResPtr)->CowConRollingAverage4 = (p[3].r_average == 0 ? ((CowConRef **)p_CowConRef)[Side]->CowConRollingAverage4 : p[3].r_average);
      ((CowConRes *)CowConResPtr)->CowConDataAttentions  = ConDataAttentions;
    }

    ContScheduler();

    rval = SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowConRes) : MESS_LEN(CowConRes), Buffer);
  }
  return rval;
}

// =============================================================================
unsigned char HndlCowConSampleRes(unsigned char Side)
{
  unsigned char      Buffer[MESS_LEN(ExtCowConSampleRes)];
  unsigned char     *ps = (unsigned char *)ConductivityBuffer;
  void   *CowConSampleResPtr = (void *)(Buffer +2);
  unsigned char     *pd;
  unsigned char     i, j, nr_of_samples;
  unsigned char     rval = 1;

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)      = ExtCowConSampleResId;
    ((ExtCowConSampleRes *)CowConSampleResPtr)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowConSampleRes *)CowConSampleResPtr)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    pd = (unsigned char *)(&(((ExtCowConSampleRes *)CowConSampleResPtr)->CowConSample001));
  } else {
    *((unsigned short *)Buffer)      = CowConSampleResId;
    ((CowConSampleRes *)CowConSampleResPtr)->PerGenNumber = PerGenNumber[Side];
    ((CowConSampleRes *)CowConSampleResPtr)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    pd = (unsigned char *)(&(((CowConSampleRes *)CowConSampleResPtr)->CowConSample001));
  }

  for ( i = 0; i < 4; ){
    StopScheduler();

    if ( LONG_COW_GEN_NUMBER ){
      ((ExtCowConSampleRes *)CowConSampleResPtr)->CowConSampleTeatNumber = i+1;
      ((ExtCowConSampleRes *)CowConSampleResPtr)->Dummy = 0;
    } else {
      ((CowConSampleRes *)CowConSampleResPtr)->CowConSampleTeatNumber = i+1;
      ((CowConSampleRes *)CowConSampleResPtr)->Dummy = 0;
    }
    nr_of_samples = CBWptr > 144 ? 144 : CBWptr;

    for ( j = 0; j < nr_of_samples; j++ )  pd[j] = ps[j*4+i];

    ContScheduler();
    rval = SendMessage( ConLow, GetNodeAddress(Side), 0,
       ((LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowConSampleRes): MESS_LEN(CowConSampleRes)) - (144 - nr_of_samples)), Buffer);
    if (rval)i++;
    else TaskExit();
  }
  return rval;
}


// =============================================================================
unsigned char HndlCowFedInfoRefReq(unsigned char Side)
{
  unsigned char      Buffer[MESS_LEN(ExtCowFedInfoRefReq)];
  void  *p_CowFedInfoRefReq = (void *)(Buffer + 2);

  if (p_CowFedInfoMail[Side] != NULL) {free(p_CowFedInfoMail[Side]); p_CowFedInfoMail[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                               = ExtCowFedInfoRefReqId;
    ((ExtCowFedInfoRefReq *)p_CowFedInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowFedInfoRefReq *)p_CowFedInfoRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                            = CowFedInfoRefReqId;
    ((CowFedInfoRefReq *)p_CowFedInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowFedInfoRefReq *)p_CowFedInfoRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowFedInfoRefReq) : MESS_LEN(CowFedInfoRefReq), Buffer);
}


// =============================================================================
void HndlCowFedInfoRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowFedInfoRef) : sizeof(CowFedInfoRef))) {goto exit;}  // bericht is te klein !!!
  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowFedInfoMail[Side] != NULL) {free(p_CowFedInfoMail[Side]); p_CowFedInfoMail[Side] = NULL;}
  while (p_CowFedInfoMail[Side] == NULL) {p_CowFedInfoMail[Side] = calloc(1, (MailPtr->message_length + sizeof(NETIO_MAIL)));}
  memcpy(p_CowFedInfoMail[Side], MailPtr, (MailPtr->message_length + sizeof(NETIO_MAIL)));

  ContScheduler();

exit: ;
}

// =============================================================================
unsigned char HndlCowFedParlInfoRefReq(unsigned char Side)
{
  unsigned char      Buffer[MESS_LEN(ExtCowFedParlInfoRefReq)];
  void  *p_CowFedParlInfoRefReq = (void *)(Buffer + 2);

  if (p_CowFedParlInfoMail[Side] != NULL) {free(p_CowFedParlInfoMail[Side]); p_CowFedParlInfoMail[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)      =  ExtCowFedParlInfoRefReqId;
    ((ExtCowFedParlInfoRefReq *)p_CowFedParlInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowFedParlInfoRefReq *)p_CowFedParlInfoRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)      =  CowFedParlInfoRefReqId;
    ((CowFedParlInfoRefReq *)p_CowFedParlInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowFedParlInfoRefReq *)p_CowFedParlInfoRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowFedParlInfoRefReq): MESS_LEN(CowFedParlInfoRefReq), Buffer);
}


// =============================================================================
void HndlCowFedParlInfoRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowFedParlInfoRef): sizeof(CowFedParlInfoRef))) {goto exit;}  // bericht is te klein !!!
  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowFedParlInfoMail[Side] != NULL) {free(p_CowFedParlInfoMail[Side]); p_CowFedParlInfoMail[Side] = NULL;}
  while (p_CowFedParlInfoMail[Side] == NULL) {p_CowFedParlInfoMail[Side] = calloc(1, (MailPtr->message_length + sizeof(NETIO_MAIL)));}
  memcpy(p_CowFedParlInfoMail[Side], MailPtr, (MailPtr->message_length + sizeof(NETIO_MAIL)));

  ContScheduler();

exit: ;
}


// =============================================================================
unsigned char HndlCowFedParlRefReq(unsigned char side)
{
  unsigned char     Buffer[MESS_LEN(ExtCowFedParlRefReq)];
  void *p_CowFedParlRefReq = (void *)(Buffer + 2);

  if (p_CowFedParlRef[side] != NULL) {
    // als de flow al binnen is return
    if ( LONG_COW_GEN_NUMBER ){
      if ( ((ExtCowFedParlRef **)p_CowFedParlRef)[side]->CowGenNumber == ((ExtMlkRef **)p_MlkRef)[side]->CowGenNumber ){
        if ( p_CowFedParlRes[side] != NULL ){
          // corrigeer tegoed, omdat result nul gemaakt moet worden omdat het anders
          // tweemaal wordt opgestuurd.
          if ( (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed >=
               ( ExtraFeed[side] + (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)){
            (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = 0;
          } else {
            (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance =
                                        (((ExtCowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance +
                                            ExtraFeed[side] -
                                        (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
          }
          ExtraFeed[side] = 0;
          (((ExtCowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed = 0;
        }
        return 1;
      }
    } else {
      if ( ((CowFedParlRef **)p_CowFedParlRef)[side]->CowGenNumber == ((MlkRef **)p_MlkRef)[side]->CowGenNumber ){
        if ( p_CowFedParlRes[side] != NULL ){
          // corrigeer tegoed, omdat result nul gemaakt moet worden omdat het anders
          // tweemaal wordt opgestuurd.
          if ( (((CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed >=
               ( ExtraFeed[side] + (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance)){
            (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance = 0;
          } else {
            (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance =
                                        (((CowFedParlRef **)p_CowFedParlRef)[side]->Array)[0].CowFedParlBalance +
                                            ExtraFeed[side] -
                                        (((CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed;
          }
          ExtraFeed[side] = 0;
          (((CowFedParlRes **)p_CowFedParlRes)[side]->Array)[0].CowFedParlAmountFed = 0;
        }
        return 1;
      }

    }
    free(p_CowFedParlRef[side]);
    p_CowFedParlRef[side] = NULL;
  }

  if (p_CowFedParlRes[side] != NULL) {free(p_CowFedParlRes[side]); p_CowFedParlRes[side] = NULL;}

  ExtraFeed[side] = 0;

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)      = ExtCowFedParlRefReqId;
    ((ExtCowFedParlRefReq *)p_CowFedParlRefReq)->PerGenNumber = PerGenNumber[side];
    ((ExtCowFedParlRefReq *)p_CowFedParlRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)      = CowFedParlRefReqId;
    ((CowFedParlRefReq *)p_CowFedParlRefReq)->PerGenNumber = PerGenNumber[side];
    ((CowFedParlRefReq *)p_CowFedParlRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowFedParlRefReq): MESS_LEN(CowFedParlRefReq), Buffer);
}


// =============================================================================
unsigned char HndlCowSepInfoRefReq(unsigned char Side)
{
  unsigned char      Buffer[MESS_LEN(ExtCowSepInfoRefReq)];
  void  *p_CowSepInfoRefReq = (void *)(Buffer + 2);

  // verwijder ook de ResultFlow
  if (p_CowSepInfoResMail[Side] != NULL) {free(p_CowSepInfoResMail[Side]); p_CowSepInfoResMail[Side] = NULL;}
  if (p_CowSepInfoMail[Side]    != NULL) {free(p_CowSepInfoMail[Side]);    p_CowSepInfoMail[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                               = ExtCowSepInfoRefReqId;
    ((ExtCowSepInfoRefReq *)p_CowSepInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowSepInfoRefReq *)p_CowSepInfoRefReq)->CowGenNumber  = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                            = CowSepInfoRefReqId;
    ((CowSepInfoRefReq *)p_CowSepInfoRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowSepInfoRefReq *)p_CowSepInfoRefReq)->CowGenNumber  = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowSepInfoRefReq) : MESS_LEN(CowSepInfoRefReq), Buffer);
}


// =============================================================================
void HndlCowSepInfoRef(NETIO_MAIL *MailPtr)
{
  unsigned char   Side;
  CowSepInfoRes  *CowSepInfoResMailPtr;
  unsigned short  length;

  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  // verwijder ook de ResultFlow
  if (p_CowSepInfoResMail[Side] != NULL) {free(p_CowSepInfoResMail[Side]); p_CowSepInfoResMail[Side] = NULL;}
  if (p_CowSepInfoMail[Side]    != NULL) {free(p_CowSepInfoMail[Side]);    p_CowSepInfoMail[Side] = NULL;}

  // alle info wordt opgeslagen
  while (p_CowSepInfoMail[Side] == NULL) {p_CowSepInfoMail[Side] = calloc(1,(MailPtr->message_length+sizeof(NETIO_MAIL)));}
  memcpy(p_CowSepInfoMail[Side], MailPtr, MailPtr->message_length+sizeof(NETIO_MAIL) );

  // reserveer ruimte voor result; andere structure dan de reference! (MAX_NR_OF_SEP_RES == 5)
  length = (sizeof(ExtCowSepInfoRes)+((MAX_NR_OF_SEP_RES-1)*sizeof(CowSepInfoResMailPtr->Array[0]))+sizeof(NETIO_MAIL));
  while (p_CowSepInfoResMail[Side] == NULL) { p_CowSepInfoResMail[Side] = calloc(1, length); }

  // voorkom eventuele overflow bij het copieren, er is plaats voor 5 items.
  memcpy(p_CowSepInfoResMail[Side], MailPtr, sizeof(NETIO_MAIL) );
  if ( length >= (MailPtr->message_length+sizeof(NETIO_MAIL)) ){
    memcpy((&(p_CowSepInfoResMail[Side]->message_start))+2, &(MailPtr->message_start), MailPtr->message_length );
  } else {
    memcpy((&(p_CowSepInfoResMail[Side]->message_start))+2, &(MailPtr->message_start), length-sizeof(NETIO_MAIL));
    p_CowSepInfoResMail[Side]->message_length = length - sizeof(NETIO_MAIL);
  }

  ContScheduler();
}


// =============================================================================
// De gewijzigde data bevindt zich in de CowSepInfoResMail.
// =============================================================================
unsigned char HndlCowSepInfoRes(unsigned char Side)
{
  NETIO_MAIL      *MailPtr;
  void            *CowSepInfoRefPtr;
  void            *CowSepInfoResPtr;
  void            *CowSepInfoResMailPtr;    // Result is een kopie van de Ref, andere structure
  unsigned char   i, j, k, nr_of_ref_items, nr_of_res_items, send;
  unsigned char   rval = 1;  // OK

  // BufferLengte afhankelijk van aantal items
  unsigned char  Buffer[MESS_LEN(ExtCowSepInfoRes)+((MAX_NR_OF_SEP_RES-1)*sizeof(((ExtCowSepInfoRes *)CowSepInfoResPtr)->Array[0]))];

  MailPtr = (NETIO_MAIL *)p_CowSepInfoMail[Side];
  nr_of_ref_items  = ((MailPtr->message_length-(LONG_COW_GEN_NUMBER?4:2))/sizeof((((ExtCowSepInfoRes *)CowSepInfoRefPtr)->Array)[0]));
  CowSepInfoRefPtr = (CowSepInfoRef *)(&p_CowSepInfoMail[Side]->message_start);

  MailPtr = (NETIO_MAIL *)p_CowSepInfoResMail[Side];
  nr_of_res_items      = ((MailPtr->message_length-(LONG_COW_GEN_NUMBER?4:2))/sizeof((((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[0]));
  CowSepInfoResMailPtr = (CowSepInfoRes *)(&p_CowSepInfoResMail[Side]->message_start);

  // prepareer buffer
  if ( LONG_COW_GEN_NUMBER ){
    (ExtCowSepInfoRes *)CowSepInfoResPtr                 = (ExtCowSepInfoRes *)(Buffer +2);
    *((unsigned short *)Buffer)                          =  ExtCowSepInfoResId;
    ((ExtCowSepInfoRes *)CowSepInfoResPtr)->PerGenNumber =  PerGenNumber[Side];
    ((ExtCowSepInfoRes *)CowSepInfoResPtr)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    (CowSepInfoRes *)CowSepInfoResPtr                 = (CowSepInfoRes *)(Buffer +2);
    *((unsigned short *)Buffer)                       =  CowSepInfoResId;
    ((CowSepInfoRes *)CowSepInfoResPtr)->PerGenNumber =  PerGenNumber[Side];
    ((CowSepInfoRes *)CowSepInfoResPtr)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }

  // test of er separatie data is ingevuld
  // sparatie-command aanwezig, kijk of data veranderd is
  for( send = j = k = 0; (j < nr_of_res_items) && (j <= MAX_NR_OF_SEP_RES); j++ ){
    for( i = 0; i < nr_of_ref_items; i++ ){
      if ( LONG_COW_GEN_NUMBER ){
        if ( ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction == ((ExtCowSepInfoRef *)CowSepInfoRefPtr)->Array[i].CowSepFunction ) {
          if ( ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations != ((ExtCowSepInfoRes *)CowSepInfoRefPtr)->Array[i].CowSepNrOfSeparations ){
            // functie aanwezig, aantallen verschillend
            ((ExtCowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepFunction        = ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction;
            ((ExtCowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepNrOfSeparations = ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations;
            k++;
            send = TRUE;
          }
        }
        break;
      } else {
        if ( ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction == ((CowSepInfoRef *)CowSepInfoRefPtr)->Array[i].CowSepFunction ) {
          if ( ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations != ((CowSepInfoRes *)CowSepInfoRefPtr)->Array[i].CowSepNrOfSeparations ){
            // functie aanwezig, aantallen verschillend
            ((CowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepFunction        = ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction;
            ((CowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepNrOfSeparations = ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations;
            k++;
            send = TRUE;
          }
        }
        break;

      }
    }
    if ( i == nr_of_ref_items ){
      // functie niet aanwezig
      if ( LONG_COW_GEN_NUMBER ){
        ((ExtCowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepFunction        = ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction;
        ((ExtCowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepNrOfSeparations = ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations;
      } else {
        ((CowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepFunction        = ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepFunction;
        ((CowSepInfoRes *)CowSepInfoResPtr)->Array[k].CowSepNrOfSeparations = ((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[j].CowSepNrOfSeparations;
      }
      k++;
      send = TRUE;
    }
  }
  if ( send ){
    rval = SendMessage( ConLow, GetNodeAddress(Side), 0, (LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowSepInfoRes): MESS_LEN(CowSepInfoRes))+((k-1)*sizeof(((CowSepInfoRes *)CowSepInfoResPtr)->Array[0])), Buffer);
  }
  return rval;
}


// =============================================================================
void HndlCowFedParlRef(NETIO_MAIL *MailPtr)
{
  signed char     idx;
  CowFedParlRef *CowFedParlRefPtr = ( CowFedParlRef *)( &( MailPtr->message_start));

  idx = GetNodeIndex(MailPtr->address_dest);
  if (idx == -1) {goto exit;}

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowFedParlRef): sizeof(CowFedParlRef))) {goto exit;}  // bericht is te klein !!!

  StopScheduler();

  if (p_CowFedParlRef[idx] != NULL) {free(p_CowFedParlRef[idx]); p_CowFedParlRef[idx] = NULL;}
  while (p_CowFedParlRef[idx] == NULL) {p_CowFedParlRef[idx] = calloc(1, MailPtr->message_length);}
  // niet meer copieren dan de werkelijke bericht lengte
  // CowFedParlRef bevat een Array echter alleen het eerste element wordt gebruikt
  // dus dat gaat goed !!
  memcpy(p_CowFedParlRef[idx], CowFedParlRefPtr, MailPtr->message_length );

  ContScheduler();

exit: ;
}


// =============================================================================
unsigned char HndlCowDiseaseRefReq(unsigned char Side)
{
  unsigned char     Buffer[MESS_LEN(ExtCowDiseaseRefReq)];
  void *p_CowDiseaseRefReq = (void *)(Buffer + 2);

  if (p_CowDiseaseMail[Side] != NULL) {free(p_CowDiseaseMail[Side]); p_CowDiseaseMail[Side] = NULL;}

  // gooi vorige result ook weg
  memory_clear ( CowDisease[Side], 14 );
  nr_of_disease_trt_items[Side] = disease_trt_number = 0;

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                               = ExtCowDiseaseRefReqId;
    ((ExtCowDiseaseRefReq *)p_CowDiseaseRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowDiseaseRefReq *)p_CowDiseaseRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                            = CowDiseaseRefReqId;
    ((CowDiseaseRefReq *)p_CowDiseaseRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowDiseaseRefReq *)p_CowDiseaseRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowDiseaseRefReq) : MESS_LEN(CowDiseaseRefReq), Buffer);
}


// =============================================================================
void HndlCowDiseaseRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side, items, ill;
  void           *CowDiseaseRefPtr;

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowDiseaseRef) : sizeof(CowDiseaseRef))) {goto exit;}  /* bericht is te klein !!! */
  Side = GetNodeIndex(MailPtr->address_dest);


  if (LONG_COW_GEN_NUMBER ){
    items = ((MailPtr->message_length-4)/sizeof((((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
  } else {
    items = ((MailPtr->message_length-2)/sizeof((((CowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
  }

  if ( items == 1){
    CowDiseaseRefPtr = (void *)(&MailPtr->message_start);
    ill = LONG_COW_GEN_NUMBER ?
              ((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array[0].CowDiseaseCode:
              ((   CowDiseaseRef *)CowDiseaseRefPtr)->Array[0].CowDiseaseCode;
    if (ill==0) {
      MailPtr->message_length -= sizeof((((CowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]);
    }
  }
  StopScheduler();

  if (p_CowDiseaseMail[Side] != NULL) {free(p_CowDiseaseMail[Side]); p_CowDiseaseMail[Side] = NULL;}
  while (p_CowDiseaseMail[Side] == NULL) {p_CowDiseaseMail[Side] = calloc(1, (MailPtr->message_length+sizeof(NETIO_MAIL)));}
  memcpy(p_CowDiseaseMail[Side], MailPtr, (MailPtr->message_length+sizeof(NETIO_MAIL)));

  ContScheduler();

exit: ;
}

// =============================================================================
unsigned char HndlCowDiseaseRes( unsigned char Side)
{
  // BufferLengte afhankelijk van aantal items
  void *CowDiseaseResPtr;
  unsigned char  Buffer[MESS_LEN(ExtCowDiseaseRes)+4*sizeof(((((CowDiseaseRes *)CowDiseaseResPtr)->Array)[0]))];

  if (!nr_of_disease_trt_items[Side] ) return 1;

  CowDiseaseResPtr = (CowDiseaseRes *)(Buffer +2);
  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                          = ExtCowDiseaseResId;
    ((ExtCowDiseaseRes *)CowDiseaseResPtr)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowDiseaseRes *)CowDiseaseResPtr)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                       = CowDiseaseResId;
    ((CowDiseaseRes *)CowDiseaseResPtr)->PerGenNumber = PerGenNumber[Side];
    ((CowDiseaseRes *)CowDiseaseResPtr)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  memory_cpy( LONG_COW_GEN_NUMBER ?
              (unsigned char *)&(((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[0].CowDiseaseCode) :
              (unsigned char *)&(((   CowDiseaseRes *)CowDiseaseResPtr)->Array[0].CowDiseaseCode)  ,
              (unsigned char *)&(CowDisease[Side][4]), (nr_of_disease_trt_items[Side])*2);

  return SendMessage( ConLow, GetNodeAddress(Side), 0,
             ((LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowDiseaseRes): MESS_LEN(CowDiseaseRes))
               +(nr_of_disease_trt_items[Side]-1)*2), Buffer);
}


// =============================================================================
// Handle CowMlkPlsRef
// =============================================================================
void HndlCowMlkPlsRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  if ( SoftKey & SK_FLW_CNTRL_PLS_ENABLE ){
    CowMlkPlsRef *CowMlkPlsRefPtr = ( CowMlkPlsRef *)( &( MailPtr->message_start));

    if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowMlkPlsRef) : sizeof(CowMlkPlsRef))) {goto exit;}  // bericht is te klein !!!

    Side = GetNodeIndex(MailPtr->address_dest);
    StopScheduler();

#if _DEBUG_
//    if ( CowMlkPlsRefPtr->CowGenNumber != p_MlkRef[Side]->CowGenNumber ) goto exit;
#endif
    if (p_CowMlkPlsRef[Side] != NULL) {free(p_CowMlkPlsRef[Side]); p_CowMlkPlsRef[Side] = NULL;}
    while ( p_CowMlkPlsRef[Side] == NULL) { p_CowMlkPlsRef[Side] = calloc(1, MailPtr->message_length);}
    memcpy( p_CowMlkPlsRef[Side], CowMlkPlsRefPtr, MailPtr->message_length);

    ContScheduler();

exit: ;
  }
}

// =============================================================================
// Handle CowMlkPlsRefReq
// =============================================================================
unsigned char HndlCowMlkPlsRefReq(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtCowMlkPlsRefReq)];
  void  *p_CowMlkPlsRefReq = (void *)(Buffer + 2);

  if (p_CowMlkPlsRef[Side] != NULL) {free(p_CowMlkPlsRef[Side]); p_CowMlkPlsRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                             = ExtCowMlkPlsRefReqId;
    ((ExtCowMlkPlsRefReq *)p_CowMlkPlsRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                          = CowMlkPlsRefReqId;
    ((CowMlkPlsRefReq *)p_CowMlkPlsRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowMlkPlsRefReq) : MESS_LEN(CowMlkPlsRefReq), Buffer);
}


// =============================================================================
// Handle Cow Parlour Feed Result
// =============================================================================
unsigned char HndlCowFedParlRes(unsigned char Side)
{
  // BufferLengte afhankelijk van aantal items
  unsigned char  Buffer[MESS_LEN(ExtCowFedParlRes)];
  void *CowFedParlResPtr = (void *)(Buffer +2);
  unsigned char rval = 1;

  if ( p_CowFedParlRes[Side] != NULL ){

    if ( LONG_COW_GEN_NUMBER ){
      *((unsigned short *)Buffer)                          = ExtCowFedParlResId;
      ((ExtCowFedParlRes *)CowFedParlResPtr)->PerGenNumber = PerGenNumber[Side];
      ((ExtCowFedParlRes *)CowFedParlResPtr)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
      ((ExtCowFedParlRes *)CowFedParlResPtr)->Array[0].CowFedParlAmountFed = (((ExtCowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed;
    } else {
      *((unsigned short *)Buffer)                       = CowFedParlResId;
      ((   CowFedParlRes *)CowFedParlResPtr)->PerGenNumber = PerGenNumber[Side];
      ((   CowFedParlRes *)CowFedParlResPtr)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
      ((   CowFedParlRes *)CowFedParlResPtr)->Array[0].CowFedParlAmountFed = (((CowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed;
    }


    if ( LONG_COW_GEN_NUMBER ){
      if (((ExtCowFedParlRes *)CowFedParlResPtr)->Array[0].CowFedParlAmountFed ){
        rval = SendMessage( ConLow, GetNodeAddress(Side), 0, MESS_LEN(ExtCowFedParlRes), Buffer);
      }

      // reset amount fed, make copy
      if ( (((ExtCowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed >= ( ExtraFeed[Side] + ((( ExtCowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance)){
        (((ExtCowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance = 0;
      } else {
        (((ExtCowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance +=
                                                               ExtraFeed[Side] -
                                                              (((ExtCowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed;
      }
      ExtraFeed[Side] = 0;
      (((ExtCowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed = 0;
    } else {
      if (((CowFedParlRes *)CowFedParlResPtr)->Array[0].CowFedParlAmountFed ){
        rval = SendMessage( ConLow, GetNodeAddress(Side), 0, MESS_LEN(CowFedParlRes), Buffer);
      }

      // reset amount fed, make copy
      if ( (((CowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed >= ( ExtraFeed[Side] + ((( CowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance)){
        (((CowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance = 0;
      } else {
        (((CowFedParlRef **)p_CowFedParlRef)[Side]->Array)[0].CowFedParlBalance +=
                                                               ExtraFeed[Side] -
                                                              (((CowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed;
      }
      ExtraFeed[Side] = 0;
      (((CowFedParlRes **)p_CowFedParlRes)[Side]->Array)[0].CowFedParlAmountFed = 0;
    }
  }
  return rval;
}

// =============================================================================
// Handle Cow Parlour Feed Result
// =============================================================================
unsigned char HndlCowExtFedParlRef(unsigned char Side)
{
  // BufferLengte afhankelijk van aantal items
  unsigned char  Buffer[MESS_LEN(CowExtFedParlRef)];
  void *CowExtFedParlRefPtr = (void *)(Buffer +2);

  *((unsigned short *)Buffer)                                = CowExtFedParlRefId;
  ((CowExtFedParlRef *)CowExtFedParlRefPtr)->PerGenNumber    = PerGenNumber[Side];
  ((CowExtFedParlRef *)CowExtFedParlRefPtr)->CowGenNumber    = LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber : ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  ((CowExtFedParlRef *)CowExtFedParlRefPtr)->CowExtParlTotal = ExtraFeed[Side];

  return SendMessage( ConLow, GetNodeAddress(Side), 0, MESS_LEN(CowExtFedParlRef), Buffer);
}



// =============================================================================
// Handle Cow Temperature Result
// =============================================================================
unsigned char HndlCowTempRes(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtCowTmpRes)];

  void *CowTmpResPtr = (void *)(Buffer +2);

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                  = ExtCowTmpResId;
    ((ExtCowTmpRes *)CowTmpResPtr)->PerGenNumber  = PerGenNumber[Side];
    ((ExtCowTmpRes *)CowTmpResPtr)->CowGenNumber  = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;

    ((ExtCowTmpRes *)CowTmpResPtr)->CowTmpLastResultTemp = MaxCowTemperature;
    ((ExtCowTmpRes *)CowTmpResPtr)->CowTmpAttention      = CowTempAttention;
  } else {
    *((unsigned short *)Buffer)               = CowTmpResId;
    ((CowTmpRes *)CowTmpResPtr)->PerGenNumber  = PerGenNumber[Side];
    ((CowTmpRes *)CowTmpResPtr)->CowGenNumber  = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;

    ((CowTmpRes *)CowTmpResPtr)->CowTmpLastResultTemp = MaxCowTemperature;
    ((CowTmpRes *)CowTmpResPtr)->CowTmpAttention      = CowTempAttention;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowTmpRes) : MESS_LEN(CowTmpRes), Buffer);
}

// =============================================================================
// Handle TempRefReq
// =============================================================================
unsigned char HndlCowTmpRefReq(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtCowTmpRefReq)];
  void *p_CowTmpRefReq = (void *)(Buffer + 2);

  if (p_CowTmpRef[Side] != NULL) {free(p_CowTmpRef[Side]); p_CowTmpRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                       = ExtCowTmpRefReqId;
    ((ExtCowTmpRefReq *)p_CowTmpRefReq)->PerGenNumber = PerGenNumber[Side];
    ((ExtCowTmpRefReq *)p_CowTmpRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                    = CowTmpRefReqId;
    ((CowTmpRefReq *)p_CowTmpRefReq)->PerGenNumber = PerGenNumber[Side];
    ((CowTmpRefReq *)p_CowTmpRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowTmpRefReq) : MESS_LEN(CowTmpRefReq), Buffer);
}


// =============================================================================
// Handle TempRef
// =============================================================================
void HndlCowTmpRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  void *CowTmpRefPtr = ( void *)( &( MailPtr->message_start));

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowTmpRef) : sizeof(CowTmpRef))) {goto exit;}  /* bericht is te klein !!! */

  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowTmpRef[Side] != NULL) {free(p_CowTmpRef[Side]); p_CowTmpRef[Side] = NULL;}
  while (p_CowTmpRef[Side] == NULL) {p_CowTmpRef[Side] = calloc(1, MailPtr->message_length);}
  memcpy(p_CowTmpRef[Side], CowTmpRefPtr, MailPtr->message_length);

  ContScheduler();

exit: ;
}

// =============================================================================
// Handle HndlMlkAmntRef
// =============================================================================
void HndlMlkAmntRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  void *MlkAmntRefPtr = ( void *)( &( MailPtr->message_start));

  if (MailPtr->message_length < sizeof(MlkAmntRef)) {goto exit;}  /* bericht is te klein !!! */

  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();
  HandleMlkAmntRef(MailPtr);
  ContScheduler();

exit: ;
}

// =============================================================================
// Handle MlkRes
// =============================================================================
void HndlMlkRes(unsigned char Type, unsigned char update, BOOLEAN generateMlkRef)
{
  unsigned char  Buffer[MESS_LEN(ExtExtendedMlkRes)];
  unsigned short length, address;
  unsigned char  idx    = GetSideIndex(SideState);
  unsigned char  offset = 0;
  void          *p_ExtendedMlkRes = (void *)(Buffer + 2);

/*
  if ((p_MlkRef[idx] == NULL) && (generateMlkRef == TRUE)) {
    // Prepare a p_MlkRef[idx]

    while ( p_MlkRef[idx] == NULL ) { p_MlkRef[idx] = calloc(1, (MESS_LEN(ExtMlkRef))); }
    if ( LONG_COW_GEN_NUMBER ){
      //((ExtMlkRes *)p_ExtendedMlkRes)->PerGenNumber        = p_PerGenRef[idx]->PerGenNumber;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenReproState = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps = 0;
      //MlkDuration;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0;
      ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount = ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount;
    } else {
      ((MlkRef **)p_MlkRef)[idx]->CowGenNumber = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowGenHeatDate = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowGenInsemDate = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowGenReproState = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps = 0;
      //MlkDuration;
      ((MlkRef **)p_MlkRef)[idx]->CowGenManualRelease = 0;
      ((MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount = ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount;
    }
  }

*/
  if ((p_MlkRef[idx] != NULL) || (generateMlkRef == TRUE)) {
    if ( LONG_COW_GEN_NUMBER ){
      *((unsigned short *)Buffer) = ExtMlkResId;
      length = MESS_LEN(ExtMlkRes);

      if (p_MlkRef[idx] == NULL){
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenNumber        = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenReproState    = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkNrOfSeps      = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenManualRelease = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenHeatDate      = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenInsemDate     = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenDryOffDate    = 0;
      } else {
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenNumber        = ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenReproState    = CowGenReproState[idx] == 3 ? 3 : ((ExtMlkRef **)p_MlkRef)[idx]->CowGenReproState;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkNrOfSeps      = ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenManualRelease = ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenHeatDate      = CowGenHeatDate[idx]   == AbsDate ? CowGenHeatDate[idx]   : ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenInsemDate     = CowGenInsemDate[idx]  == AbsDate ? CowGenInsemDate[idx]  : ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenDryOffDate    = CowGenDryOffDate[idx] == AbsDate ? CowGenDryOffDate[idx] : ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate;
      }
      ((ExtMlkRes *)p_ExtendedMlkRes)->PerGenNumber        = p_PerGenRef[idx]->PerGenNumber;
      ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldDuration   = MlkDuration;

      if ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 0xff) offset = 2;

      if ( MilkMeterType >= MMT_NO_MILK_METER ){
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldManual     = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount     = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = 0;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = FALSE;
      } else {
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldManual     = MilkManualInput;
        ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount     = (update == SEND_UPDATE_CLR_MLK_AMOUNT ? 0 : (unsigned short)((MlkAmount+5)/10));
        if ((p_MlkRef[idx] == NULL) || (((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount >= ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount)) {
          ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = FALSE;
        } else {
          ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = TRUE;
        }
      }
    } else {
      *((unsigned short *)Buffer) = MlkResId;
      length = MESS_LEN(MlkRes);

      if (p_MlkRef[idx] == NULL){
        ((MlkRes *)p_ExtendedMlkRes)->CowGenNumber        = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenReproState    = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkNrOfSeps      = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenManualRelease = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = FALSE;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenHeatDate      = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenInsemDate     = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenDryOffDate    = 0;
      } else {
	      ((MlkRes *)p_ExtendedMlkRes)->CowGenNumber        = ((MlkRef **)p_MlkRef)[idx]->CowGenNumber;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenReproState    = CowGenReproState[idx] == 3 ? 3 : ((MlkRef **)p_MlkRef)[idx]->CowGenReproState;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkNrOfSeps      = ((MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenManualRelease = ((MlkRef **)p_MlkRef)[idx]->CowGenManualRelease;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenHeatDate      = CowGenHeatDate[idx]   == AbsDate ? CowGenHeatDate[idx]   : ((MlkRef **)p_MlkRef)[idx]->CowGenHeatDate;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenInsemDate     = CowGenInsemDate[idx]  == AbsDate ? CowGenInsemDate[idx]  : ((MlkRef **)p_MlkRef)[idx]->CowGenInsemDate;
        ((MlkRes *)p_ExtendedMlkRes)->CowGenDryOffDate    = CowGenDryOffDate[idx] == AbsDate ? CowGenDryOffDate[idx] : ((MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate;
      }

      ((MlkRes *)p_ExtendedMlkRes)->PerGenNumber        = p_PerGenRef[idx]->PerGenNumber;
      ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldDuration   = MlkDuration;

      if ( ((MlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 0xff) offset = 2;
      
      if ( MilkMeterType >= MMT_NO_MILK_METER ){
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldManual     = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount     = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = 0;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = FALSE;
      } else {
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldManual     = MilkManualInput;
        ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount     = (update == SEND_UPDATE_CLR_MLK_AMOUNT ? 0 : (unsigned short)((MlkAmount+5)/10));
        if ((p_MlkRef[idx] == NULL)||(((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount >= ((MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount)) {
          ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = FALSE;
        } else {
          ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAttention  = TRUE;
        }
      }
    }
    // proceed with MilkMeterType
    if ( MilkMeterType < MMT_NO_MILK_METER ){
      // Geiten melkglas
      if (EEGoatMilkGlass == TRUE) {
        if (MlkAmount < 300) {
          //Bij melkgiften < 300 gr. de geprogrammeerde hoeveelheid oversturen
          if ( LONG_COW_GEN_NUMBER ){
            ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount  = ((EEMmGoatVal2+5)/10);
          } else {
            ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldAmount  = ((EEMmGoatVal2+5)/10);
          }
        }
      }

      if (MilkManualInput == FALSE) {
        if ( LONG_COW_GEN_NUMBER ){
          ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = (unsigned short)((MlkMaxFlow+5)/10);
        } else {
          ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = (unsigned short)((MlkMaxFlow+5)/10);
        }
      } else {
        if ( LONG_COW_GEN_NUMBER ){
          ((ExtMlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = 0;
        } else {
          ((MlkRes *)p_ExtendedMlkRes)->CowMlkYldMaxSpeed   = 0;
        }
      }
    }


    if (Type == EXTEND_RES) {
      if ( LONG_COW_GEN_NUMBER ){
        *((unsigned short *)Buffer) = ExtExtendedMlkResId;
        length = MESS_LEN(ExtExtendedMlkRes);

        ((ExtExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime1 = 0;
        ((ExtExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime2 = 0;
        ((ExtExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime3 = 0;
        ((ExtExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime4 = 0;
      } else {
        *((unsigned short *)Buffer) = ExtendedMlkResId;
        length = MESS_LEN(ExtendedMlkRes);

        ((ExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime1 = 0;
        ((ExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime2 = 0;
        ((ExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime3 = 0;
        ((ExtendedMlkRes *)p_ExtendedMlkRes)->CowMlkYldDeadMlkTime4 = 0;
      }
    }

//    if ((LONG_COW_GEN_NUMBER ? ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenNumber : ((MlkRes *)p_ExtendedMlkRes)->CowGenNumber) > 0) {
      address = (LONG_COW_GEN_NUMBER ? ((ExtMlkRes *)p_ExtendedMlkRes)->PerGenNumber % 100 : ((MlkRes *)p_ExtendedMlkRes)->PerGenNumber % 100);
      
      // wait untill message accepted
      while (SendMessage( ConLow, address, 0, length-offset, Buffer) == 0)TaskExit();
      
      if ((LONG_COW_GEN_NUMBER ? ((ExtMlkRes *)p_ExtendedMlkRes)->CowGenNumber : ((MlkRes *)p_ExtendedMlkRes)->CowGenNumber) > 0) {
        if (!update )HndlCowResults(idx);
      }

    if (Type == NORMAL_RES) {
      //
      FreeMlkRef(idx, FNC_NORMAL_RES);
      p_MlkRef[idx]   = p_MlkRef_B[idx];
      p_MlkRef_B[idx] = NULL;

      if (p_CowActRef[idx] != NULL) { free(p_CowActRef[idx]); p_CowActRef[idx] = NULL; }

      if (p_MlkRef[idx]) {
          // volgende koe, zelfde zijde vraag gegevens op
        MlkRefState[idx] = COWS_ONE;
        SetGetCowRefs(idx);
      } else {
        MlkRefState[idx] = COWS_NONE;
      }
      MeasState[idx] = MEAS_IDLE;
    } else {
      if ( (Type == NORMAL_RES_NO_CLEAR) && !(p_MlkRef_B[idx]) && SwingOver() ){
        // anders wordt oude data niet overschreven als er een MlkRef binnen komt
//        MlkRefState[idx] = COWS_NONE;  // versie 101
      }
    }
  }
}


// =============================================================================
// Handle CowMlkSampleRes
// =============================================================================
unsigned char HndlCowMlkSampleRes(unsigned char Side)
{
  unsigned char   Buffer[MESS_LEN(ExtCowMlkSampleRes)];
  void           *CowMlkSampleResPtr  = (void *)(Buffer +2);

  StopScheduler();
  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer) = ExtCowMlkSampleResId;
    ((ExtCowMlkSampleRes *)CowMlkSampleResPtr)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer) = CowMlkSampleResId;
    ((CowMlkSampleRes *)CowMlkSampleResPtr)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }

  memory_cpy( LONG_COW_GEN_NUMBER ?
              (unsigned char *)&(((ExtCowMlkSampleRes *)CowMlkSampleResPtr)->CowMlkSample001) :
              (unsigned char *)&(((   CowMlkSampleRes *)CowMlkSampleResPtr)->CowMlkSample001),
              (unsigned char *)&(MlkAmountProfileTable), MlkAmountProfileTableIndex );

  ContScheduler();
  return SendMessage( ConLow, GetNodeAddress(Side), 0, (LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowMlkSampleRes) : MESS_LEN(CowMlkSampleRes))-(144 - MlkAmountProfileTableIndex), Buffer);
}


// =============================================================================
// Handle MlkYldStatusRes
// =============================================================================
unsigned char HndlMlkYldStatusRes(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtMlkYldStatusRes)];
  void *MlkYldStatusResPtr = (void *)(Buffer +2);

  StopScheduler();

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                                         = ExtMlkYldStatusResId;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowGenNumber            = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldStartTime      = CowMlkYldStartTime[Side];
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldNrOfAttach2min = CowMlkYldNrOfAttach2min;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldNrOfAttach     = CowMlkYldNrOfAttach;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldManDetach      = CowMlkYldManDetach;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldManInput       = CowMlkYldManInput[Side];
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount00_15    = CowMlkYldAmount00_15;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount15_30    = CowMlkYldAmount15_30;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount30_60    = CowMlkYldAmount30_60;
    ((ExtMlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount60_120   = CowMlkYldAmount60_120;
  } else {
    *((unsigned short *)Buffer)                                      = MlkYldStatusResId;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowGenNumber            = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldStartTime      = CowMlkYldStartTime[Side];
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldNrOfAttach2min = CowMlkYldNrOfAttach2min;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldNrOfAttach     = CowMlkYldNrOfAttach;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldManDetach      = CowMlkYldManDetach;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldManInput       = CowMlkYldManInput[Side];
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount00_15    = CowMlkYldAmount00_15;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount15_30    = CowMlkYldAmount15_30;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount30_60    = CowMlkYldAmount30_60;
    ((MlkYldStatusRes *)MlkYldStatusResPtr)->CowMlkYldAmount60_120   = CowMlkYldAmount60_120;
  }
  ContScheduler();
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtMlkYldStatusRes) : MESS_LEN(MlkYldStatusRes), Buffer);
}


// =============================================================================
unsigned char HndlPerPresenceResReq(NETIO_MAIL *MailPtr)
{
  unsigned char Buffer[MESS_LEN(PerPresenceRes)];
  PerPresenceRes *PerPresenceResPtr = (PerPresenceRes *)(Buffer +2);
  *((unsigned short *)Buffer)       = PerPresenceResId;

  StopScheduler();
  PerPresenceResPtr->PerGenNumber = ((PerGenNumber[0] / 100) * 100) + MailPtr->address_dest;
  ContScheduler();

  return SendMessage( ConLow, MailPtr->address_dest, 0, sizeof(Buffer), Buffer );
}



// =============================================================================
// SetGetCowRefs
// =============================================================================
void SetGetCowRefs(unsigned char Side)
{
  Get_Cow_Refs = 0xff;  // request all items again
  CowRefSide   = Side;
}

// =============================================================================
// GetCowRefs
// only handle one message per loop because of user interface
// =============================================================================
void GetCowRefs(void)
{
  unsigned char succes = 0;
  
  if (Get_Cow_Refs != 0 ) {
    if (Get_Cow_Refs == 0xFF ){
      if ( !SwingOver() ){
        // reset alle oude attenties, bij swing-over gebeurt dit bij wisselen zijde (milk.c)
        MlkAttention = CowGenInsemDate[CowRefSide] = CowGenHeatDate[CowRefSide] = CowGenDryOffDate[CowRefSide] = 0;
        ConDataAttentions = CowTempAttention = 0;
        CowGenReproState[CowRefSide] = 0xff;
      }
    }

    if (MlkCowAvailable(CowRefSide)) {
      switch(Get_Cow_Refs){
        case  0xff:
              if (HndlCowGenRefReq(CowRefSide))succes = 1;
          break;
        case  0xfe:
              if ( SoftKey & SK_FLW_CNTRL_PLS_ENABLE){
                if (HndlCowMlkPlsRefReq(CowRefSide))succes = 1;
              } else {
              	succes = 1;          // proceed with other requests
              }
          break;
        case  0xfd:
              if ((EEConMeasuring & CM_MEASURE_ENABLE ) || EERobot == TRUE){
                if (HndlCowConRefReq(CowRefSide))succes = 1;
              } else {
              	succes = 1;          // proceed with other requests
              }
          break;
        case  0xfc:
              if (EETempMeasuring == TRUE  || EERobot == TRUE){
                if(HndlCowTmpRefReq(CowRefSide))succes = 1;
              } else {
              	succes = 1;          // proceed with other requests
              }
          break;
        case  0xfb:
              if (EEFeeding && (SoftKey & SK_FEEDING_ENABLE)){
                if(HndlCowFedParlRefReq(CowRefSide))succes = 1;
              } else {
              	succes = 1;          // proceed with other requests
              }
          break;
        case  0xfa:
              if (HndlCowFedInfoRefReq(CowRefSide))succes = 1;
          break;
        case  0xf9:
              if (HndlCowFedParlInfoRefReq(CowRefSide))succes = 1;
          break;
        case  0xf8:
              if (HndlCowActMemAttRefReq(CowRefSide))succes = 1;
          break;
        case  0xf7:
              if (HndlCowDiseaseRefReq(CowRefSide))succes = 1;
          break;
        case  0xf6:
              if (HndlCowSepInfoRefReq(CowRefSide))succes = 1;
          break;
        default:
              if (HndlCowActRefReq(CowRefSide)){
                Get_Cow_Refs = 0;
              }
          break;
      }
      // select next state
      if (Get_Cow_Refs &&(succes == 1)) Get_Cow_Refs--;
    } else {
      // No cow available
      Get_Cow_Refs = 0;
    }
  }
}



// =============================================================================
// Handle CowResults
// =============================================================================
void HndlCowResults(unsigned char Side)
{
  if (MlkCowAvailable(Side)) {
    while (HndlCowDiseaseRes(Side) == 0)TaskExit();
    if ( (EEConMeasuring & CM_MEASURE_ENABLE) || EERobot == TRUE ) {
      TaskExit();
      while (HndlCowConRes(Side) == 0)TaskExit();
      if ((EEConMeasuring & CM_LONG_RECORD) &&  (MilkMeterType < MMT_NO_MILK_METER)){
        TaskExit();
        while (HndlCowConSampleRes(Side)== 0)TaskExit();
        TaskExit();
        while (HndlCowMlkSampleRes(Side) == 0)TaskExit();
      }
    }
    if (EETempMeasuring == TRUE){
      TaskExit();
      while (HndlCowTempRes(Side) == 0)TaskExit();
    }

    if ( p_CowGenRef[Side] != NULL ){
      if ( LONG_COW_GEN_NUMBER ){
        if ( ((ExtCowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber !=
             ((ExtCowGenRef **)p_CowGenRef)[Side]->CowGenGrpNumber ){
          // stuur nieuw groep-nr over
          TaskExit();
          while(HndlCowGenRes(Side)== 0)TaskExit();
        }
      } else {
        if ( ((CowGenRef **)p_CowGenRef)[Side]->CowGenNextGrpNumber !=
             ((CowGenRef **)p_CowGenRef)[Side]->CowGenGrpNumber ){
          // stuur nieuw groep-nr over
          TaskExit();
          while (HndlCowGenRes(Side))TaskExit();
        }
      }
    }
    if ( p_CowFedParlRes[Side] != NULL && p_CowFedParlRef[Side] != NULL){
      TaskExit();
      while (HndlCowFedParlRes(Side))TaskExit();

      //
    }
    TaskExit();
    while(HndlMlkYldStatusRes(Side) == 0)TaskExit();
  }
}



// =============================================================================
void FlowControl(NETIO_MAIL *MailPtr)
{

  switch (MailPtr->message_id) {
    case DldPerEraseRefId:              HndlDldPerEraseRef(MailPtr);               break;
    case SysGenAbsTimeBroadcastId:      HndlSysGenAbsTimeBroadcast(MailPtr);       break;
    case PerGenInstResReqId:            HndlPerGenInstResReq(MailPtr);             break;
    case PerGenNumberRefId:             HndlPerGenNumberRef(MailPtr);              break;
    case PerPresenceResReqId:           HndlPerPresenceResReq(MailPtr);            break;
    case PerGenRefId:                   HndlPerGenRef(MailPtr);                    break;
    default:                                                                       break;
  }
  if ( SoftKey & SK_VC4_ENABLE ){
    switch (MailPtr->message_id) {
      case MlkAmntRefId:                  HndlMlkAmntRef(MailPtr);                   break;
      case HerdConRefId:                  HndlHerdConRef(MailPtr);                   break;
      case ExtMlkRefId:
      case ExtMlkManualRefId:
      case ExtMlkUpdateRefId:
      case MlkRefId:
      case MlkManualRefId:
      case MlkUpdateRefId:                HndlMlkRef(MailPtr);                       break;
      case ExtCowActRefId:
      case CowActRefId:                   HndlCowActRef(MailPtr);                    break;
      case ExtCowConRefId:
      case CowConRefId:                   HndlCowConRef(MailPtr);                    break;
      case ExtCowTmpRefId:
      case CowTmpRefId:                   HndlCowTmpRef(MailPtr);                    break;
      case ExtCowGenRefId:
      case CowGenRefId:                   HndlCowGenRef(MailPtr);                    break;
      case ExtCowFedParlInfoRefId:
      case CowFedParlInfoRefId:           HndlCowFedParlInfoRef(MailPtr);            break;
      case ExtCowFedInfoRefId:
      case CowFedInfoRefId:               HndlCowFedInfoRef(MailPtr);                break;
      case ExtCowSepInfoRefId:
      case CowSepInfoRefId:               HndlCowSepInfoRef(MailPtr);                break;
      case ExtCowFedParlRefId:
      case CowFedParlRefId:               HndlCowFedParlRef(MailPtr);                break;
      case ExtCowDiseaseRefId:
      case CowDiseaseRefId:               HndlCowDiseaseRef(MailPtr);                break;
      case PerMatMlkRefId:                HndlPerMatMlkRef(MailPtr);                 break;
      case PerMatMlkResReqId:             HndlPerMatMlkResReq(MailPtr);              break;
      case PerFuncTestId:                 HndlPerFuncTest(MailPtr);                  break;
      case ExtCowMlkPlsRefId:
      case CowMlkPlsRefId:                HndlCowMlkPlsRef(MailPtr);                 break;
      case ExtCowActMemAttRefId:
      case CowActMemAttRefId:             HndlCowActMemAttRef(MailPtr);              break;
      case PerMPCInstRefId:               HndlPerMPCInstRef(MailPtr);                break;
      case PerMPCInstResReqId:            HndlPerMPCInstResReq(MailPtr);             break;
      case PerGenErrorResetRefId:         WriteEEpromShort( 0, &ErrorInfo );         break;
      default:                                                                       break;
    }
  }
}


