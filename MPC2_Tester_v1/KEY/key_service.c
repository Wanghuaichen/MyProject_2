#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/input.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../lib/mem.h"
#include "../lib/eeprom.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../lib/typedef.h"
#include "../lib/text.h"
#include "../include/cstartup.h"
#include "../include/romeehc1.h"
#include "../appl/feed.h"
#include "../appl/milk.h"
#include "../appl/takeoff.h"
#include "../appl/memolac.h"
#include "../appl/pulsation.h"
#include "../appl/tandem.h"
#include "../appl/temp.h"
#include "../comm/send_msg.h"
#include "../comm/flowctrl.h"



void HandleKeyService( void )
{
 long  value;

  KeyValue = KeyA + KeyState;

  switch ( KeyValue ) {
    case KEY_WASH:
         if (KeyHold == FALSE) {
           KeyMode      = KM_TEST;
           MlkState     = MLK_TEST;
           ScreenNumber = SCREEN_RESET;
         }
         KeyState = KS_NORMAL;
      break;
    case KEY_SHIFT :
         if ( (ScreenMode !=  SM_ACT_DISEASE_INFO) && (ScreenMode !=  SM_CAL_TEMP_INFO)){
           if (ScreenEEPtr == &AddressEE){
             ScreenNumber = FieldNrMax;
             FieldNr      = FieldNrMax-1;
           } else {
             // afhankelijk van de mogelijkheden, PIC en/of instellingen
             // worden sommige items NIET weergegeven
             // De volgorde van de item beschrijving (const ITEM_INFO) is hier van belang
             if      ((ScreenEEPtr ==  &KeySoft  ) && (!(SoftKey & SK_VC4_ENABLE))&&((EEMilkMeterType == MMT_VFLOW)||(EEMilkMeterType == MMT_VP4001)))      FieldNr -=12;
             else if ((ScreenEEPtr ==  &KeySoft  ) && (!(SoftKey & SK_VC4_ENABLE)))                                                                         FieldNr -=14;
             else if ((ScreenEEPtr ==  &KeySoft  ) && (!(SoftKey & SK_COND_TMP_ENABLE))&&((EEMilkMeterType == MMT_VFLOW)||(EEMilkMeterType == MMT_VP4001))) FieldNr -=3;
             else if ((ScreenEEPtr ==  &KeySoft  ) && (!(SoftKey & SK_COND_TMP_ENABLE)))                                                                    FieldNr -=5;
             else if ((ScreenEEPtr ==  &ServiceStandsEE)    && (!(SoftKey & (SK_TANDEM_ENABLE+SK_GATE_CONTROL_ENABLE))))                                    FieldNr -=6;
             else if ((ScreenEEPtr ==  &ServiceStandsEE)    && (!(SoftKey & SK_TANDEM_ENABLE)))                                                             FieldNr -=2;
             else if ((ScreenEEPtr ==  &OTRowMasterEE)      && (!(SoftKey & SK_GATE_CONTROL_ENABLE)))                                                       FieldNr -=5;
             else if ((ScreenEEPtr == (&FeedingEE)+8)       && (!(SoftKey & SK_FEEDING_ENABLE)) && (EEPulsation == 0) )                                     FieldNr -=17;
             else if ((ScreenEEPtr == (&FeedingEE)+8)       && (!(SoftKey & SK_FEEDING_ENABLE)) )        FieldNr -=9;
             else if ((ScreenEEPtr == (&FeedingEE)+8)       && (  EEFeeding  == 0) )                     FieldNr -=8;  // TestEE
             else if ((ScreenEEPtr == (&FeedingEE)+8)       && EEFeeding && !EEAddress2 )                FieldNr -=4;  // spring naar RLST
             else if ((ScreenEEPtr ==  &SB_RelaisTimeEE)    && EEAddressCopy )                           FieldNr -=3;  // swing-over, spring naar RLST
             else if ((ScreenEEPtr ==  &RelaisTimeEE)       && EEAddressCopy )                           FieldNr -=4;  //
             else if ((ScreenEEPtr ==  &NrOfPulsatorsEE)    && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))   FieldNr -=6;
             else if ((ScreenEEPtr == (&GoatMilkGlassEE)+3) && (EEGoatMilkGlass == 0) )                  FieldNr -=3;
             else if ((ScreenEEPtr ==  &CleanStartTimeEE)   && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))   FieldNr -=6;
             else if ((ScreenEEPtr == (&OutputK2EE)+6)      && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))   FieldNr -=6;
             else if ((ScreenEEPtr == (&OutputK2EE)+6)      && (EEOutputK2 != MILK_AUTOMAT_CONTROL))     FieldNr -=6;
             else if ((ScreenEEPtr == (&PulsationEE)+9)     && (EEPulsation == 0))                       FieldNr -=9;
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))   FieldNr -=7;  //
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (  EESepMsgCntrl == 0) )                  FieldNr -=5;  //
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (  EESepMsgCntrl & 0x08))                 FieldNr -=1;  // SepMinYield
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD)) FieldNr -=2;  // SepMinYield
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_COND))    FieldNr -=3;  // SepCond
             else if ((ScreenEEPtr ==  &ClusterDownEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_ACT))     FieldNr -=4;  // SepAct
             else if ((ScreenEEPtr ==  &SepTmpFuncNrEE)     && (!(EESepMsgCntrl & 0x07)))                FieldNr -=4;  //
             else if ((ScreenEEPtr ==  &SepTmpFuncNrEE)     && (  EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD)) FieldNr -=1;  //
             else if ((ScreenEEPtr ==  &SepTmpFuncNrEE)     && (  EESepMsgCntrl & SEND_SEP_MSG_COND))    FieldNr -=2;  //
             else if ((ScreenEEPtr ==  &SepTmpFuncNrEE)     && (  EESepMsgCntrl & SEND_SEP_MSG_ACT))     FieldNr -=3;  //
             else if ((ScreenEEPtr ==  &SepMinYldFuncNrEE)  && (!(EESepMsgCntrl & 0x03)))                FieldNr -=3;  //
             else if ((ScreenEEPtr ==  &SepMinYldFuncNrEE)  && (  EESepMsgCntrl & SEND_SEP_MSG_COND))    FieldNr -=1;  //
             else if ((ScreenEEPtr ==  &SepMinYldFuncNrEE)  && (  EESepMsgCntrl & SEND_SEP_MSG_ACT))     FieldNr -=2;  //
             else if ((ScreenEEPtr ==  &SepCondFuncNrEE)    && (!(EESepMsgCntrl & SEND_SEP_MSG_ACT)))    FieldNr -=2;  //
             else if ((ScreenEEPtr ==  &PrgVrsIC13 )        && (EEMilkMeterType == MMT_MEMOLAC))         FieldNr -=16;
             else if ((ScreenEEPtr ==  &PulsationEE )       && ((EEMilkMeterType == MMT_VFLOW) || (EEMilkMeterType == MMT_VFLOW)))  FieldNr -=3;
             else                                                                                        FieldNr--;
           }
           ScreenNumber--;
           ScreenEEPtr = DefTable[FieldNr];
           KeyState    = KS_NORMAL;
         } else {
           KeyState     = KS_CONTINUE;
           KeyShiftTime = Cnt10msec;
         }
      break;
    case KEY_ENTER:
         if ( (ScreenMode !=  SM_ACT_DISEASE_INFO) && (ScreenMode !=  SM_CAL_TEMP_INFO)){
             // afhankelijk van de mogelijkheden, PIC en/of instellingen
             // worden sommige items NIET weergegeven
             // De volgorde van de items is hier van belang
           if      ((ScreenEEPtr == &MlkFlwCalcTimeEE)    && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))                          FieldNr +=7;
//           else if ((ScreenEEPtr == &TakeOffTimeEE)&& (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE))&&(EEMilkMeterType == MMT_VFLOW)) FieldNr +=7;
           else if ((ScreenEEPtr == &SepMsgCntrlEE)       && (  EESepMsgCntrl == 0))                    FieldNr +=5;
           else if ((ScreenEEPtr == &SepMsgCntrlEE)       && (  EESepMsgCntrl & SEND_SEP_MSG_ACT))      FieldNr +=1;
           else if ((ScreenEEPtr == &SepMsgCntrlEE)       && (  EESepMsgCntrl & SEND_SEP_MSG_COND))     FieldNr +=2;
           else if ((ScreenEEPtr == &SepMsgCntrlEE)       && (  EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD))  FieldNr +=3;
           else if ((ScreenEEPtr == &SepMsgCntrlEE)       && (  EESepMsgCntrl & SEND_SEP_MSG_TMP))      FieldNr +=4;
           else if ((ScreenEEPtr == &SepActFuncNrEE)      && (!(EESepMsgCntrl & 0x0E)))                 FieldNr +=4;
           else if ((ScreenEEPtr == &SepActFuncNrEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_COND))     FieldNr +=1;
           else if ((ScreenEEPtr == &SepActFuncNrEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD))  FieldNr +=2;
           else if ((ScreenEEPtr == &SepActFuncNrEE)      && (  EESepMsgCntrl & SEND_SEP_MSG_TMP))      FieldNr +=3;
           else if ((ScreenEEPtr == &SepCondFuncNrEE)     && (!(EESepMsgCntrl & 0x0C)))                 FieldNr +=3;
           else if ((ScreenEEPtr == &SepCondFuncNrEE)     && (  EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD))  FieldNr +=1;
           else if ((ScreenEEPtr == &SepCondFuncNrEE)     && (  EESepMsgCntrl & SEND_SEP_MSG_TMP))      FieldNr +=2;
           else if ((ScreenEEPtr == &SepMinYldFuncNrEE)   && (!(EESepMsgCntrl & SEND_SEP_MSG_TMP)))     FieldNr +=2;
           else if ((ScreenEEPtr == &OutputK2EE)          && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))    FieldNr +=6;
           else if ((ScreenEEPtr == &OutputK2EE)          && (EEOutputK2 != MILK_AUTOMAT_CONTROL))      FieldNr +=6;
           else if ((ScreenEEPtr == &GoatMilkGlassEE)     && (EEGoatMilkGlass == 0) )                   FieldNr +=3;
           else if ((ScreenEEPtr == &PulsationEE)         && (!(SoftKey & SK_FEEDING_ENABLE))&& (EEPulsation  == 0))  FieldNr +=17;
           else if ((ScreenEEPtr == &PulsationEE)         && (EEPulsation  == 0) )                      FieldNr +=9;
           else if ((ScreenEEPtr == &PulsRatioEE)         && (!(SoftKey & SK_STIM_TRCK_ARM_ENABLE)))    FieldNr +=6;
           else if ((ScreenEEPtr == &NrOfPulsatorsEE)     && (!(SoftKey & SK_FEEDING_ENABLE)))          FieldNr +=9;
           else if ((ScreenEEPtr == &FeedingEE)           && (EEFeeding  == 0) )                        FieldNr +=8;
           else if ((ScreenEEPtr == &FeedingEE)           && EEFeeding && EEAddressCopy )               FieldNr +=4;  // stand-alone
           else if ((ScreenEEPtr == &RelaisTimeEE)        && !EEAddress2 )                              FieldNr +=4;  // stand-alone && swing-over
           else if ((ScreenEEPtr == &RelaisTimeEE)        && EEAddressCopy )                            FieldNr +=3;  // swing-over
           else if ((ScreenEEPtr == &ParlourTypeEE)&&((EEMilkMeterType == MMT_VFLOW)||(EEMilkMeterType == MMT_VP4001))&&(!(SoftKey & SK_VC4_ENABLE)))    FieldNr +=12;
           else if ((ScreenEEPtr == &ParlourTypeEE)       &&  (!(SoftKey & SK_VC4_ENABLE)))                                                              FieldNr +=14;
           else if ((ScreenEEPtr == &ParlourTypeEE)       && (!((SoftKey & (SK_TANDEM_ENABLE+SK_GATE_CONTROL_ENABLE)))))                                 FieldNr +=6;
           else if ((ScreenEEPtr == &ParlourTypeEE)       &&  (!(SoftKey & SK_GATE_CONTROL_ENABLE)))                                                     FieldNr +=5;
           else if ((ScreenEEPtr == &GateK5EE )           && (!(SoftKey & SK_TANDEM_ENABLE)))                                                            FieldNr +=2;
           else if ((ScreenEEPtr == &RowSizeEE)&& (!(SoftKey & SK_COND_TMP_ENABLE))&& ((EEMilkMeterType == MMT_VFLOW)||(EEMilkMeterType == MMT_VP4001))) FieldNr +=3;
           else if ((ScreenEEPtr == &RowSizeEE)&& (!(SoftKey & SK_COND_TMP_ENABLE)))                                                                     FieldNr +=5;
           else if ((ScreenEEPtr == &DispTimeEE)          && (EEMilkMeterType == MMT_MEMOLAC))                                                           FieldNr +=16;
           else if ((ScreenEEPtr == &ClnPhs1EE )          && ((EEMilkMeterType == MMT_VFLOW)||(EEMilkMeterType == MMT_VP4001)))                          FieldNr +=3;
           else                                                                                         FieldNr++;

           ScreenEEPtr = DefTable[FieldNr];
           ScreenNumber++;
           if (ScreenEEPtr == 0) {
             FieldNr      = 0;
             ScreenEEPtr  = DefTable[FieldNr];
             ScreenNumber = 0;
           }
         } else {
           if ( ScreenMode == SM_CAL_TEMP_INFO ){
             if ( ScreenNumber == SCREEN_CAL_TEMP_1 ){
               WriteEEpromByte( ActUadc, &EE_U1 );
               ScreenNumber++;
             } else {
               if ( ScreenNumber == SCREEN_CAL_TEMP_2 ){
                 WriteEEpromByte( ActUadc, &EE_U2 );
                 ScreenNumber = SCREEN_CAL_TEMP_1;
               }
             }
           }
         }
         KeyState = KS_NORMAL;
      break;
    case KEY_CLEAR:
         switch( ScreenMode){
           case SM_CAL_TEMP_INFO:
                if ( ScreenNumber == SCREEN_CAL_TEMP_3 ){
                  if ( GetNValue(3, 0, 7, &value)){
                    WriteEEpromByte( (unsigned char)value, &EE_U1 );
                    ScreenNumber++;
                  }
                } else {
                  if ( ScreenNumber == SCREEN_CAL_TEMP_4 ){
                    if ( GetNValue(3, 0, 7, &value)){
                      WriteEEpromByte( (unsigned char)value, &EE_U2 );
                      ScreenNumber = SCREEN_CAL_TEMP_1;
                    }
                  }
                }
             break;
           default:
                if (ScreenEEPtr->Attrib == WRITE) {
                  KeyModeOld = KeyMode;
                  KeyMode    = KM_INPUT;
                }
             break;
         }
         KeyState = KS_NORMAL;
      break;
    case KEY_DISEASE_INFO:
         if ( ScreenMode ==  SM_ACT_DISEASE_INFO ){
           ScreenNumber++;
           if (ScreenNumber == SCREEN_ACT_DISEASE_MAX) {
             ScreenNumber = SCREEN_CONDUCT_RF;
           }
         } else {
           ScreenMode   = SM_ACT_DISEASE_INFO;
           ScreenNumber = SCREEN_CONDUCT_RF;

           // zet pointer op laatste item
           ScreenEEPtr = DefTable[FieldNrMax-1];
           memory_set( DispTable, ' ', 10 );
         }
         KeyState = KS_NORMAL;
      break;
    case KEY_SHIFT_DISEASE_INFO:
         if ( ScreenMode == SM_ACT_DISEASE_INFO ){
           if (ScreenNumber == SCREEN_TEMP ) {
             // voer referentie punten in U35 en U40, bij resp R35 en R40
             ScreenMode   = SM_CAL_TEMP_INFO;
             ScreenNumber = SCREEN_CAL_TEMP_1;
           }
         } else {
           if ( ScreenMode == SM_CAL_TEMP_INFO ){
             ScreenNumber++;
             if (ScreenNumber == SCREEN_CAL_TEMP_MAX) {
               ScreenNumber = SCREEN_CAL_TEMP_1;
             }
           }
         }
      break;

    case KEY_K3_GATE :
         if(!KeyHold && !(SwingOver() && EEFeeding) ){
           if ( (EEParlourType == PT_HERRING_BONE) && EEGateK3Time ) {
              ExtPortK3State = EPK3S_ON;
             GateK3ActiveTime = Cnt10msec;
           } else {
              ExtPortK3State = EPK3S_TOGGLE;
           }
         }
      break;
    case KEY_K4_GATE : if(!KeyHold) ExtPortK4State = EPK4S_TOGGLE; break;
    case KEY_K5_GATE : if(!KeyHold) ExtPortK5State = EPK5S_TOGGLE; break;
    default :                                                      break;
  }
}