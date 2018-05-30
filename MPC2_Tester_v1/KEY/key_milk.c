#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/input.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../lib/mem.h"
#include "../lib/extport.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/text.h"
#include "../lib/watchdog.h"
#include "../motor/motor.h"
#include "../appl/mm_4001.h"
#include "../appl/tandem.h"
#include "../appl/temp.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/takeoff.h"
#include "../appl/conduct.h"
#include "../appl/pulsation.h"
#include "../comm/flowctrl.h"

extern unsigned char msgTimeoutTime;


// =============================================================================
// HandleKeyMilkingStopPulse
// Start/Stop pulsation
// =============================================================================
void HandleKeyMilkingStopPulse( void )
{
  if ( PLSCntrl.ManualPulseToggle ) PLSCntrl.ManualPulseToggle = 0;
  else                              PLSCntrl.ManualPulseToggle = 1;
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyMilkingStartPulse
// =============================================================================
void HandleKeyMilkingStartPulse( void )
{
  if ( ScreenMode  != SM_PULSE_INFO ){
    ScreenNumber    =  SCREEN_PULSE_INFO_0;
    ScreenMode      =  SM_PULSE_INFO;
    ScreenNumberOld = !ScreenNumber;
  } else {
    ScreenNumber++;
    if (ScreenNumber == SCREEN_PULSE_INFO_MAX) ScreenNumber = SCREEN_PULSE_INFO_0;
  }
  KeyState  = KS_NORMAL;
}

// =============================================================================
// HandleKeyMilkingCalendarInfo
// Show calendar info
// =============================================================================
void HandleKeyMilkingCalendarInfo( void )
{
  signed char  idx;
  idx = GetDisplaySide();

  if ( ScreenMode != SM_CALENDAR_INFO ){
    // controleer of er een attenties zijn (heat/insem/dry-off),
    // zoja zet deze eerst op het display, laat het display ook knipperen
    ScreenNumber = SCREEN_CALENDAR_INFO_0;
    if ( p_MlkRef[idx] != NULL ){
      if ( LONG_COW_GEN_NUMBER ){
        if      ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatAttent   ) ScreenNumber = SCREEN_CALENDAR_INFO_1;
        else if ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemAttent  ) ScreenNumber = SCREEN_CALENDAR_INFO_2;
        else if ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent ) ScreenNumber = SCREEN_CALENDAR_INFO_4;
        else if ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent ) ScreenNumber = SCREEN_CALENDAR_INFO_3;
      } else {
        if      ( ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatAttent   ) ScreenNumber = SCREEN_CALENDAR_INFO_1;
        else if ( ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemAttent  ) ScreenNumber = SCREEN_CALENDAR_INFO_2;
        else if ( ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent ) ScreenNumber = SCREEN_CALENDAR_INFO_4;
        else if ( ((   MlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent ) ScreenNumber = SCREEN_CALENDAR_INFO_3;
      }
    }
    ScreenMode  = SM_CALENDAR_INFO;
    ScreenNumberOld = !ScreenNumber;
  } else {
    ScreenNumber++;
    if (ScreenNumber == SCREEN_CALENDAR_INFO_MAX) ScreenNumber = SCREEN_CALENDAR_INFO_0;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyMilkingShiftFeedInfo
// =============================================================================
void HandleKeyMilkingShiftFeedInfo( void )
{
  signed char  idx;
  idx = GetDisplaySide();

  if ( p_CowFedParlRef[idx]){
    while(HndlCowFedParlRes(idx) == 0)TaskExit();
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyMilkingFeedInfo
// Show feed info
// =============================================================================
void HandleKeyMilkingFeedInfo( void)
{
  NETIO_MAIL *MailPtr;
  void       *CowFedInfoRefPtr;
  void       *CowFedParlInfoRefPtr;

  unsigned char i, nr_of_items;
  signed char   idx;

  idx = GetDisplaySide();

  if ( ScreenMode != SM_FEED_INFO ){
    /*
    ** Voer tegoeden voerstal
    */
    if (p_CowFedInfoMail[idx] != NULL ){
      CowFedBalance = CowFedTotal = 0;
      MailPtr = (NETIO_MAIL *)p_CowFedInfoMail[idx];
      if ( LONG_COW_GEN_NUMBER ){
        nr_of_items = ((MailPtr->message_length-4)/sizeof((((ExtCowFedInfoRef *)CowFedInfoRefPtr)->Array)[0]));
      } else {
        nr_of_items = ((MailPtr->message_length-2)/sizeof((((   CowFedInfoRef *)CowFedInfoRefPtr)->Array)[0]));
      }
      if( nr_of_items ){
        // tel voer hoeveelheden op, display in 0.1 Kg
        CowFedInfoRefPtr = (void *)(&p_CowFedInfoMail[idx]->message_start);
        for( i = 0; i < nr_of_items; i++ ){
          if ( LONG_COW_GEN_NUMBER ){
            CowFedBalance += ((ExtCowFedInfoRef *)CowFedInfoRefPtr)->Array[i].CowFedBalance;
            CowFedTotal   += ((ExtCowFedInfoRef *)CowFedInfoRefPtr)->Array[i].CowFedTotal;
          } else {
            CowFedBalance += ((   CowFedInfoRef *)CowFedInfoRefPtr)->Array[i].CowFedBalance;
            CowFedTotal   += ((   CowFedInfoRef *)CowFedInfoRefPtr)->Array[i].CowFedTotal;
          }
        }
        CowFedBalance /= 100;
        CowFedTotal   /= 100;
      }
    } else {
      CowFedBalance = CowFedTotal = 0;
    }
    /*
    ** Voer tegoeden melkstal
    ** SCREENS: 0: ACTFDM                     actual feed
    **          1: FDMILK or F_SPD            feed in milk parlour/feed_speed
    **          2: FD TOT or FBPRT or F_PRT   feed total, feedportion sideB, feedportion. (normal, swing_over&sideB,else ..)
    **          3: FD BAL or FBBAL            feed balance, balance sideB. (normal, swing_over&sideB)
    **          4: FD_CAL
    */
    if (p_CowFedParlInfoMail[idx] != NULL ){
      CowFedParlTotalFed = 0;
      MailPtr = (NETIO_MAIL *)p_CowFedParlInfoMail[idx];
      if ( LONG_COW_GEN_NUMBER ){
        nr_of_items = ((MailPtr->message_length-4)/sizeof((((ExtCowFedParlInfoRef *)CowFedParlInfoRefPtr)->Array)[0]));
      } else {
        nr_of_items = ((MailPtr->message_length-2)/sizeof((((   CowFedParlInfoRef *)CowFedParlInfoRefPtr)->Array)[0]));
      }
      if( nr_of_items ){
        // tel voer hoeveelheden op, display in 0.1 Kg
        CowFedParlInfoRefPtr = (void *)(&p_CowFedParlInfoMail[idx]->message_start);
        for( i = 0; i < nr_of_items; i++ ){
          if ( LONG_COW_GEN_NUMBER ){
            CowFedParlTotalFed   += ((ExtCowFedParlInfoRef *)CowFedParlInfoRefPtr)->Array[i].CowFedParlPrevTotalFed;
          } else {
            CowFedParlTotalFed   += ((   CowFedParlInfoRef *)CowFedParlInfoRefPtr)->Array[i].CowFedParlPrevTotalFed;
          }
        }
        CowFedParlTotalFed   /= 100;
      }
    } else {
      CowFedParlTotalFed = 0;
    }

    ScreenMode      = SM_FEED_INFO;
    // if feeding disabled do not show first screen (ACTFDM)
    if ( EEFeeding ) ScreenNumber = SCREEN_FEED_INFO_0;
    else {
      ScreenNumber = SCREEN_FEED_INFO_1;
      if ( ((ParlourType == PT_CARROUSEL ) && ( p_CowFedParlInfoMail[idx] == NULL)) ||
           ((ParlourType != PT_CARROUSEL ) && ( p_CowFedParlRef[idx] == NULL ))) ScreenNumber++;
    }
    ScreenNumberOld = !ScreenNumber;
  } else {
    ScreenNumber++;
    if (ScreenNumber == SCREEN_FEED_INFO_1){
      if ( ((ParlourType == PT_CARROUSEL ) && ( p_CowFedParlInfoMail[idx] == NULL)) ||
           ((ParlourType != PT_CARROUSEL ) && ( p_CowFedParlRef[idx] == NULL ))) ScreenNumber++;
    }

    if (ScreenNumber == SCREEN_FEED_INFO_4){
      // next screen: FD_CAL; do not show when feeding disabled.
      if ( !EEFeeding ) ScreenNumber++;
    }

    if (ScreenNumber >= SCREEN_FEED_INFO_MAX){
      if ( EEFeeding ) ScreenNumber = SCREEN_FEED_INFO_0;
      else {
        ScreenNumber = SCREEN_FEED_INFO_1;
        if ( ((ParlourType == PT_CARROUSEL ) && ( p_CowFedParlInfoMail[idx] == NULL)) ||
             ((ParlourType != PT_CARROUSEL ) && ( p_CowFedParlRef[idx] == NULL ))) ScreenNumber++;
      }
    }
  }
}


// =============================================================================
// HandleKeyMilkingShiftMilkInfo
// =============================================================================
void HandleKeyMilkingShiftMilkInfo( void )
{
  if (EEOutputK2 == MILK_AUTOMAT_CONTROL) {
    if( StartMilkAutomat != MA_MANUAL_STARTED ){
      if ( MilkAutomatState != MILK_AUTOMAT_ON_OFF &&
           MilkAutomatState != MILK_AUTOMAT_OFF_ON ){
        StartMilkAutomat = MA_MANUAL_START;
        MilkAutomatState = MILK_AUTOMAT_START;
      }
    }
  }
}

// =============================================================================
// HandleKeyMilkingMilkInfo
// SCREENS: 0: SEPMLK   the number of milkings the milk should be separated
//          1: MILKT    milk-test-value, the minimum expected milk-yield
//          2: COLOST   colostrum-milk Yes or No
//          3: TIME     total milk-time
//          4: MAXSP    maximum measured milk flow
//          5: AVRSP    the averige milk flow
//          6: MLKSP    the actual milk flow
//          7: MAN REL  start with manual release
// =============================================================================
void HandleKeyMilkingMilkInfo( void )
{
  signed char  idx;
  idx = GetDisplaySide();

  if ( ScreenMode != SM_MILK_INFO ){
    ScreenNumber = SCREEN_MILK_INFO_0;

    // controleer of er een attenties zijn (Separatie/Biest), zoja zet deze
    // eerst op het display, laat het display ook knipperen
    if ( p_MlkRef[idx] != NULL ){
      if (  (LONG_COW_GEN_NUMBER ?
             !((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps && ((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost :
             !((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps && ((   MlkRef **)p_MlkRef)[idx]->CowGenColost )){
        ScreenNumber = SCREEN_MILK_INFO_2;
      }
    }
    ScreenMode      = SM_MILK_INFO;
    ScreenNumberOld = !ScreenNumber;
  } else {
    ScreenNumber++;
    if ( MilkMeterType >= MMT_NO_MILK_METER ){
       // gemiddelde melktijd ed. hebben geen zin
       if ( ScreenNumber == SCREEN_MILK_INFO_4 )ScreenNumber = SCREEN_MILK_INFO_0;
    }
    if (ScreenNumber == SCREEN_MILK_INFO_7){
      if (  (LONG_COW_GEN_NUMBER ?
             (((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 0xff) :
             (((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 0xff) )){
        ScreenNumber++;
      }
    }
    if (ScreenNumber == (MilkMeterType == MMT_NIVEAU ? (SCREEN_MILK_INFO_MAX+1) : SCREEN_MILK_INFO_MAX) ) {
      ScreenNumber = SCREEN_MILK_INFO_0;
    }
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyMilkingCowInfo
// =============================================================================
void HandleKeyMilkingCowInfo( void )
{
  NETIO_MAIL *MailPtr;
  void       *CowSepInfoResMailPtr;

  signed char  idx;
  idx = GetDisplaySide();

  if ( ScreenMode != SM_COW_INFO ){
    MailPtr = (NETIO_MAIL *)p_CowSepInfoResMail[idx];
    if (p_CowSepInfoResMail[idx] != NULL ) {
      CowSepNrOfItems = ((MailPtr->message_length-(LONG_COW_GEN_NUMBER ? 4:2))/
                          sizeof((((CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[0]));
    } else {
      CowSepNrOfItems = 0;
    }
    sub_screen_pntr  = 0;

    ScreenNumber    =  SCREEN_COW_INFO_0;
    ScreenMode      =  SM_COW_INFO;
    ScreenNumberOld = !ScreenNumber;
  } else {
    if ( (ScreenNumber == 0 && p_CowActMemAttRef[idx] != NULL ) ||
         (ScreenNumber == 1 && CowSepNrOfItems )                ){
      // Skip door toets ingedrukt te houden
      if ( KeyHold ){ ScreenNumber++; sub_screen_pntr = 0; }
      else {
        if ( ScreenNumber == 0 && p_CowActMemAttRef[idx] != NULL ) {
          sub_screen_pntr++;
          if (sub_screen_pntr >=
               ((LONG_COW_GEN_NUMBER ?
                 ((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt :
                 ((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt) != 0 ? 2 : 1) ){ ScreenNumber++; sub_screen_pntr = 0; }
        } else {
          if ( ScreenNumber == 1 && CowSepNrOfItems ){
            // separatie info, kunnen dus meerdere items zijn
            sub_screen_pntr++;
            if ( CowSepNrOfItems ){
              if (sub_screen_pntr >= CowSepNrOfItems ) ScreenNumber++;
            }
          }
        }
      }
      // anders wordt display te vlug terug geschakeld
      ScreenTime = Cnt1sec;
    } else {
      ScreenNumber++;
      sub_screen_pntr = 0;
      if (ScreenNumber == (SCREEN_COW_INFO_MAX-1))ScreenNumber = SCREEN_COW_INFO_0;
    }
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyMilkingDiseaseInfo
// =============================================================================
void HandleKeyMilkingDiseaseInfo( void )
{
  NETIO_MAIL        *MailPtr;
  CowDiseaseRef     *CowDiseaseRefPtr;
  unsigned char     i;

  signed char  idx;
  idx = GetDisplaySide();

  if (ScreenMode != SM_DISEASE_MILK_INFO ){

    // controleer of er een ziekte attenties zijn, zoja zet deze
    // eerst op het display, laat het display ook knipperen
    ScreenNumber   = SCREEN_DISEASE_1;
    disease_trt_number = 0;

    if ( p_CowDiseaseMail[idx] != NULL ){
      MailPtr = (NETIO_MAIL *)p_CowDiseaseMail[idx];
      if (LONG_COW_GEN_NUMBER ){
        nr_of_disease_items = ((MailPtr->message_length-4)/sizeof((((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
      } else {
        nr_of_disease_items = ((MailPtr->message_length-2)/sizeof((((CowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
      }
      if( nr_of_disease_items ){
        // disease_info ontvangen
        ScreenNumber   = SCREEN_DISEASE_0;
        disease_number = 0;
      }
    } else {
      // indien geen mengmelk maar wel take-off sensor ga naar screen 8, actual RR
      if ( !(EEExtConMeasuring & CM_ONE_CHANNEL) ){
        if ( MilkMeterType == MMT_TAKE_OFF_SENSOR ) ScreenNumber = SCREEN_DISEASE_8;
      }
    }

    if( ScreenNumber != SCREEN_DISEASE_0 ){
      // geen ziekte-codes voor deze koe
      if ( MeasState[idx] == MEAS_IDLE ) {
        if ( p_CowConRef[idx] &&
          (LONG_COW_GEN_NUMBER ?
          (((ExtCowConRef **)p_CowConRef)[idx]->CowConDataAttentions & 0x0f ) :
          (((   CowConRef **)p_CowConRef)[idx]->CowConDataAttentions & 0x0f ))){
          i = 0;
          while( !((LONG_COW_GEN_NUMBER ?
                  ((ExtCowConRef **)p_CowConRef)[idx]->CowConDataAttentions :
                  ((   CowConRef **)p_CowConRef)[idx]->CowConDataAttentions) & (0x01 << i)) )i++;
          ScreenNumber = SCREEN_DISEASE_1 + i;
        }
      } else {
        if ( idx == GetDisplaySide() ){
          // melken gestart
          if ( ConDataAttentions & 0x0f ){
            i = 0;
            while( !(ConDataAttentions & (0x01 << i)) )i++;
            ScreenNumber = SCREEN_DISEASE_1 + i;
          } else {
            if (  CowTempAttention ){
              ScreenNumber = SCREEN_DISEASE_5;
            }
          }
        }
      }
    }
    ScreenMode  = SM_DISEASE_MILK_INFO;
    ScreenNumberOld = !ScreenNumber;
  } else {
    // display eerst alle ziekte-codes
    // Skip door toets ingedrukt te houden
    if ( ScreenNumber == SCREEN_DISEASE_0 && nr_of_disease_items ){
      if ( disease_number < (nr_of_disease_items-1) && !KeyHold ){
        disease_number++;
        // anders wordt display te vlug terug geschakeld
        ScreenTime = Cnt1sec;
      } else {
        ScreenNumber++;
      }
    } else {
      if ( ScreenNumber == SCREEN_DISEASE_1 ){
        if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
          if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
            ScreenNumber = SCREEN_DISEASE_4;
          } else {
            ScreenNumber = SCREEN_DISEASE_6;
          }
        }
      } else {
        if ( ScreenNumber == SCREEN_DISEASE_7 ){
          if ( EEExtConMeasuring & CM_ONE_CHANNEL )ScreenNumber = SCREEN_DISEASE_10;
        } else {
          if ( ScreenNumber == SCREEN_DISEASE_8 ){
            if ( MilkMeterType == MMT_TAKE_OFF_SENSOR )ScreenNumber = SCREEN_DISEASE_10;
          } else {
            if ( ScreenNumber == SCREEN_DISEASE_11 ){
              if ( (MilkMeterType == MMT_TAKE_OFF_SENSOR) || ( EEExtConMeasuring & CM_ONE_CHANNEL ))ScreenNumber = SCREEN_DISEASE_14;
            }
          }
        }
      }

      if (ScreenNumber == (SCREEN_DISEASE_MAX-1)) {
        // display de ingegeven ziekte-codes plus behandeling
        if ( nr_of_disease_trt_items[idx] ){
          if ( disease_trt_number < ( nr_of_disease_trt_items[idx] == 5 ? 4 : nr_of_disease_trt_items[idx] ) ){
            disease_trt_number++;
            // anders wordt display te vlug terug geschakeld
            ScreenTime = Cnt1sec;
          }
        } else {
          disease_trt_number = 0;
          // bepaal met welk scherm er moet worden vervolgd
          if ( nr_of_disease_items ){
            ScreenNumber   = SCREEN_DISEASE_0;
            disease_number = 0;
          } else {
            ScreenNumber = SCREEN_DISEASE_1;
            if ( !(EEExtConMeasuring & CM_ONE_CHANNEL) ){
              if ( MilkMeterType == MMT_TAKE_OFF_SENSOR ) ScreenNumber = SCREEN_DISEASE_8;
            }
          }
        }
      } else {
        ScreenNumber++;
      }
    }
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyMilkingShiftClear
// =============================================================================
void HandleKeyMilkingShiftClear( void )
{
  // edit tweede item, ga naar input mode
  EditItem   = 2;
  KeyState   = KS_NORMAL;
  KeyModeOld = KeyMode;
  KeyMode    = KM_INPUT;
}


// =============================================================================
// HandleKeyMilkingClear
// =============================================================================
void HandleKeyMilkingClear( void )
{
  long  value;

  // edit eerste item, ga naar input mode
  if ( (ScreenMode == SM_FEED_INFO) && (ScreenNumber == SCREEN_FEED_INFO_4)){
    if ( GetNValue(2, 0, 8, &value)){
      value > 20 ? (nr_of_cal_feed_portions = 20) : (nr_of_cal_feed_portions = value);
      while ( nr_of_cal_feed_portions ){
        ScreenTime = Cnt1sec;             // hold display
        DispValue((unsigned char *)&(nr_of_cal_feed_portions), _U1, 2, 0, READ_ONLY, POS_RIGHT);
        memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_CAL]), 6 );
        DispScreen();
        Display();
        if ( EERelaisTime ){
          RelaisControl();
          nr_of_cal_feed_portions--;
        } else {
          if ( MotorControl(0, TURN) ) nr_of_cal_feed_portions = 0;         // error
          else                         nr_of_cal_feed_portions--;
        }
        TaskDelay(100);
      }
    }
  } else {
    EditItem   = 1;
    KeyModeOld = KeyMode;
    KeyMode    = KM_INPUT;
  }
}

// =============================================================================
// HandleKeyMilkingWash
// =============================================================================
void HandleKeyMilkingWash( void )
{
  if (KeyHold == TRUE) {
    MlkState          = MLK_START_CLEANING;
    KeyMode           = KM_CLEANING;
    TakeOffState      = TO_START_CLEAN;
    ScreenNumber      = SCREEN_RESET;
    ul_CleanStartTime = Cnt1sec;
    PlsState          = PLS_STOP_PULSATION;
    if ( MilkMeterType < MMT_NO_MILK_METER) UnicoState = UNICO_DETACH;
  } else {
    if ( MlkState == MLK_WAIT_FOR_SEND) {
      MlkSepStandAlone = 0;
      // Melkglas leeg laten lopen
      if (EEOutputP1Inv == TRUE) {
        if (PORTA & P_3) ExtOutputP1State = P1_ON;
        else             ExtOutputP1State = P1_START_MAN;
      } else {
        if (PORTA & P_3) ExtOutputP1State = P1_START_MAN;
        else             ExtOutputP1State = P1_ON;
      }
      MlkState = MLK_SEND_MAN;
    }
  }
}


// =============================================================================
// HandleKeyMilkingShiftWash
// =============================================================================
void HandleKeyMilkingShiftWash( void)
{
  // start reinigen, verstuur eventueel ook een bericht naar ander MPC's
  if ( EERowSize && !(ExternMessages & SEND_CLEAN_STOP_MESSAGES) ){
    ExternMessages |= SEND_CLEAN_START_MESSAGES;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyMilkingK5Gate
// =============================================================================
void HandleKeyMilkingK5Gate( void )
{
  if ( ParlourTandemTypeAndMilkingMode() ){
    HandleKeyOpenTandem(KeyValue, KeyHold);
  } else {
    if(!KeyHold) ExtPortK5State = EPK5S_TOGGLE;
  }
}


// =============================================================================
// HandleKeyMilkingK4Gate
// =============================================================================
void HandleKeyMilkingK4Gate( void)
{
  if ( ParlourTandemTypeAndMilkingMode() ){
    HandleKeyOpenTandem(KeyValue, KeyHold);
  } else {
    if(!KeyHold) ExtPortK4State = EPK4S_TOGGLE;
  }
}

// =============================================================================
// HandleKeyMilkingK3Gate
// =============================================================================
void HandleKeyMilkingK3Gate( void )
{
  if ( ParlourTandemTypeAndMilkingMode() ){
    HandleKeyOpenTandem(KeyValue, KeyHold);
  } else {
    // bij SwingOver en voeren wordt K3 gebruikt voor motor sturing
    // dus afblijven
    if ( !(SwingOver() && EEFeeding)){
      if(!KeyHold){
        if ( (EEParlourType == PT_HERRING_BONE) && EEGateK3Time ) {
          ExtPortK3State = EPK3S_ON;
          GateK3ActiveTime = Cnt10msec;
        } else {
          ExtPortK3State = EPK3S_TOGGLE;
        }
      } else {
        ExtPortK3State = EPK3S_WAIT_I3_FREE;  // sluit hek nadat I3 vrij is
      }
    }
  }
}

// =============================================================================
// HandleKeyMilkingNine
// =============================================================================
void HandleKeyMilkingNine( void )
{
  if (EEOutputK2 == MILK_AUTOMAT_CONTROL) {
    // Toggle output P3
    if ( PORTA & P_5 )  PORTA &= ~P_5;
    else                PORTA |=  P_5;
  }
}

// =============================================================================
// HandleKeyMilkingTwo
// =============================================================================
void HandleKeyMilkingTwo( void )
{
  if ( SwingOver() ){
    // swing-over
    if ( SideState == SIDE_A ){
      DisplayOtherSide = TRUE;
      OtherDisplayTime = Cnt10msec;
    } else {
      DisplayOtherSide = FALSE;
    }
  }
}

// =============================================================================
// HandleKeyMilkingOne
// =============================================================================
void HandleKeyMilkingOne( void )
{
  if ( SwingOver() ){
    // swing-over
    if ( SideState == SIDE_B ){
      DisplayOtherSide = TRUE;
      OtherDisplayTime = Cnt10msec;
    } else {
      DisplayOtherSide = FALSE;
    }
  }
}


// =============================================================================
// HandleKeyMilkingShift2ManualReleaseCluster
// =============================================================================
void HandleKeyMilkingShift2ManualReleaseCluster( void )
{
  // laat cluster handmatig zakken, vacuum blijft dicht. Wordt gebruikt
  // om melkstel te reinigen of om een beter zicht op de koeien te krijgen
  ClusterState = CLUSTER_LOW;
  KeyState     = KS_NORMAL;
}

// =============================================================================
// HandleKeyMilkingShiftManualReleaseCluster
// =============================================================================
unsigned char HandleKeyMilkingShiftManualReleaseCluster( void )
{
  unsigned char rval = 0;
    signed char side = GetDisplaySide();

  if (TakeOffState != TO_MAN) {
  	rval = 1;
    if ( ((p_MlkRef[side] != NULL) &&
            (LONG_COW_GEN_NUMBER ?
            (CowMilked[side] == ((ExtMlkRef **)p_MlkRef)[side]->CowGenNumber) :
            (CowMilked[side] == ((   MlkRef **)p_MlkRef)[side]->CowGenNumber))) ||
         (p_MlkRef[side] == NULL) ){
      // opnieuw onderhangen van het melkstel
      TakeOffState = TO_RELEASE_MAN;
      if ( (p_MlkRef[side] != NULL) || (EEAddressCopy == 0) ) {MlkState = MLK_CONTINUE; rval = 2;}
      else                                                    {MlkState = MLK_INIT;     rval = 3;}
    }
  }
  KeyState = KS_NORMAL;
  return rval;
}


// =============================================================================
// HandleKeyMilkingManualReleaseCluster
// returns uneven value if action performed
// =============================================================================
unsigned char HandleKeyMilkingManualReleaseCluster( void )
{
  signed char  idx;
  unsigned char rval = 0;
#if _HARTMAN_
  unsigned short time;
#endif

  idx = GetDisplaySide();

  if (MlkState != MLK_INIT){
    rval = 1;
    if (MlkState == MLK_SEND_WAIT) {
      rval = 2;
      // opnieuw onderhangen van het melkstel
      TakeOffState = TO_RELEASE_MAN;
      MlkState     = MLK_CONTINUE;
    } else {
      rval = 3;
      if (TakeOffState == TO_AUTO) {
        // bij gebruik van een UNICO mag er niet gewisseld worden van automatisch
        // naar manueel
        rval = 4;
        CowMlkYldManDetach++;                      // manual release
        TakeOffState = TO_MAN;
        if (MlkState != MLK_IDLE) MlkState = MLK_NEUTRAL;
        UnicoState   = UNICO_ATTACH_MAN;
      } else {
        rval = 5;
        if (TakeOffState != TO_MAN) {
          // test op separatie-melk en op droge koeien          
          if ((GetCowAtt(idx) & (CA_SEP+CA_COLOST+CA_DRY_BLCK+CA_NOCOW)) ) {
            rval = 9;
            if (KeyHold == TRUE) {
              rval = 6;
              TakeOffState = TO_RELEASE_MAN;
              MlkState     = MLK_INIT;
            }
          } else {
            rval = 7;
            if ( DifferentCowAvailable(idx) || StandAlone() || (msgTimeoutTime==0)) {
              rval = 8;
              TakeOffState = TO_RELEASE_MAN;
              MlkState     = MLK_INIT;
            }
          }
        }
      }
    }
#if _HARTMAN_
    if ( EEHartman ){
      // tbv hartman, een puls op de motor uitgang
      if ( KeyHold != TRUE ){
        PORTA &= ~0x40;
        time   = Cnt10msec;
        while (!TimeControl(Cnt10msec, time + 50))WatchDog();
        PORTA |= 0x40;
      }
    }
#endif
  }
  KeyState = KS_NORMAL;
  return rval;
}


// =============================================================================
// HandleKeyMilkingShift2AutoRelease
// =============================================================================
unsigned char HandleKeyMilkingShift2AutoRelease( void )
{
	unsigned char rval = 0;
	
  if ( EERowSize ){
    // set MPC's in pre-wash mode, centraal
    rval = 1;
    ExternMessages |= SEND_PRE_WASH_START_MESSAGES;
  }
  KeyState = KS_NORMAL;
  return rval;
}


// =============================================================================
// HandleKeyMilkingShiftAutoRelease
// =============================================================================
unsigned char HandleKeyMilkingShiftAutoRelease(void)
{
  unsigned char rval = 0;
    signed char side = GetDisplaySide();

  WaitForI2High  = FALSE;
  if (TakeOffState != TO_AUTO) {
    rval = 1;
    if ( ((p_MlkRef[side] != NULL) &&
             (LONG_COW_GEN_NUMBER ?
             (CowMilked[side] == ((ExtMlkRef **)p_MlkRef)[side]->CowGenNumber) :
             (CowMilked[side] == ((   MlkRef **)p_MlkRef)[side]->CowGenNumber))) ||
         (p_MlkRef[side] == NULL) ){
      // opnieuw onderhangen van het melkstel
      TakeOffState = TO_RELEASE_AUTO;
      if ( (p_MlkRef[side] != NULL) || (EEAddressCopy == 0) ){MlkState = MLK_CONTINUE; rval = 2;}
      else                                                   {MlkState = MLK_INIT    ; rval = 3;}
    }
  }
  KeyState = KS_NORMAL;
  return rval;
}


// =============================================================================
// HandleKeyMilkingAutoRelease
// =============================================================================
unsigned char HandleKeyMilkingAutoRelease( void )
{
  signed char  idx;
  unsigned char rval = 0;  // assume ok
#if _HARTMAN_
  unsigned short time;
#endif
  idx = GetDisplaySide();

  WaitForI2High  = FALSE;
  if (MlkState != MLK_INIT){
    rval = 1;
    if (MlkState == MLK_SEND_WAIT) {
      TakeOffState = TO_RELEASE_AUTO;
      MlkState     = MLK_CONTINUE;
      rval = 2;         
    } else {
      // It is possible that MLK_INIT
      rval = 3;
      if (TakeOffState == TO_MAN) {
        rval = 4;
        TakeOffState = TO_AUTO;
        UnicoState   = UNICO_ATTACH_AUTO;
        if (MlkState != MLK_IDLE) MlkState = MLK_NEUTRAL;
      } else {
        if (TakeOffState != TO_AUTO) {
          rval = 5;
          // test op separatie-melk en op droge koeien 
          // NO_COW attention is also generated when communication fails if this happnes
          // milking should still be possible
          if ((GetCowAtt(idx) & (CA_SEP+CA_COLOST+CA_DRY_BLCK+CA_NOCOW))) {
            rval = 6;
            if ((DifferentCowAvailable(idx) && (KeyHold == TRUE)) || (msgTimeoutTime==0)) {
       	      rval = 7;
              TakeOffState = TO_RELEASE_AUTO;
              MlkState     = MLK_INIT;
            }
          } else {
            rval = 8;
            if (DifferentCowAvailable(idx) || StandAlone()) {
              rval = 9;
              TakeOffState = TO_RELEASE_AUTO;
              MlkState     = MLK_INIT;
            }
          }
        }
#endif
      }
    }
#if _HARTMAN_
    if ( EEHartman ){
      // tbv hartman, een puls op de motor uitgang
      if ( KeyHold != TRUE ){
        PORTA &= ~0x40;
        time   = Cnt10msec;
        while (!TimeControl(Cnt10msec, time + 50))WatchDog();
        PORTA |= 0x40;
      }
    }
#endif
  }
  KeyState = KS_NORMAL;
  return rval;
}


// =============================================================================
// HandleKeyMilkingActMilkValve
// =============================================================================
void HandleKeyMilkingActMilkValve( void )
{
  if (KeyHold == TRUE){
    if ((ExtPortCopy & K1) ||
  	((EEOutputK2 == UNICO_CONTROL) && (MilkMeterType== MMT_NIVEAU)) ) {

      MlkSepStandAlone = 0;
      switch (MilkMeterType) {
        case MMT_NIVEAU:
             if (EEOutputP1Inv == TRUE) {
               if (PORTA & P_3) ExtOutputP1State = P1_ON;
               else             ExtOutputP1State = P1_START_MAN;
             } else {
               if (PORTA & P_3) ExtOutputP1State = P1_START_MAN;
               else             ExtOutputP1State = P1_ON;
             }
          break;
        case MMT_MEMOLAC:
             ValveState = VALVE_START;
          break;
      } // end switch ( MilkMeterType)
    }
    KeyState = KS_NORMAL;
  } else {
    // dit is tevens een extra 'SHIFT_KEY' !!!!!!!!!!!!!!
    KeyState = KS_CONTINUE_2;
    KeyShiftTime = Cnt10msec;
  }
}


// =============================================================================
// HandleKeyMilkingClusterRemoval
// =============================================================================
unsigned char HandleKeyMilkingClusterRemoval( void )
{
  unsigned char rval = 0;
	
  // ophalen melkstel vanuit melken
  if ((TakeOffState == TO_MAN)  || (TakeOffState == TO_AUTO)) {
    rval = 1;
    TakeOffState = TO_START_UNICO;
    MlkState     = MLK_TAKE_OFF;
  } else {
    // ophalen melkstel vanuit reinigen
    if ((TakeOffState == TO_INIT) || (TakeOffState == TO_CLEANING)) {
      rval = 2;
      TakeOffState = TO_FIRST_START;
    } else {
      // ophalen melkstel vanuit cluster-down situatie
      rval = 3;
      ClusterState = CLUSTER_DOWN_HIGH;
    }
  }
  KeyState = KS_NORMAL;
  return rval;
}

void ReportKeyMilk(unsigned char Key, unsigned char path)
{
  unsigned char  buffer[MESS_LEN(DbgMessKeyRes)+2];
  PerFuncTest    *PerFuncTestPtr;

  void          *DbgMessKeyResPtr = (void *)(buffer + 4);
  
  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = DbgMessKeyResId;
   
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgKey      = Key;
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgMlkState = MlkState;
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgToState  = TakeOffState;
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgCowAtt   = (GetCowAtt(SideState) & (CA_SEP+CA_COLOST+CA_DRY_BLCK+CA_NOCOW)) ? 1 : 0;
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgPlsState = PlsState;
  ((DbgMessKeyRes *)DbgMessKeyResPtr)->DbgKeyPath  = path;

  while(SendMessage( ConLow, GetNodeAddress(SideState), 0, MESS_LEN(DbgMessKeyRes)+2, buffer)== 0)TaskExit();
}

// =============================================================================
// HandleKeyMilking
// =============================================================================
void HandleKeyMilking(void)
{
	unsigned char path;
  KeyValue = KeyA + KeyState;

  // zorg voor genoeg display-tijd 'andere zijde' ANY_KEY
  if ( EEAddress2 && DisplayOtherSide ) OtherDisplayTime = Cnt10msec;

  switch (KeyValue) {
    case KEY_CLUSTER_REMOVAL :
         path = HandleKeyMilkingClusterRemoval();
         ReportKeyMilk(KeyValue, path);
#if DEBUG_VP4001         
         InitMsgCntr();
#endif
      break;

    case KEY_ACT_MILK_VALVE :
    case KEY_ACT_MILK_VALVE+0x40:     // vanaf versie19 - 25 heeft extra dump niet gewerkt
         HandleKeyMilkingActMilkValve();
      break;

    case KEY_AUTO_RELEASE:            // start milking: automatic take-off
    case KEY_AUTO_RELEASE_EXT:	
         if ( (GetManualorAutoRelease() == KEY_MANUAL_REL_CLUSTER) && !FirstAttach ){
           path = HandleKeyMilkingManualReleaseCluster();
           if (!(path & 0x01)){                     // even path: action performed
             FirstAttach = 1;                       // only if milking has been started; else manual release does not work.
           }
           ReportKeyMilk(KeyHold == TRUE ? KEY_AUTO_RELEASE_FIRST_ATTACH_HOLD : KEY_AUTO_RELEASE_FIRST_ATTACH, path);
         } else {
           path = HandleKeyMilkingAutoRelease();
           ReportKeyMilk(KeyHold == TRUE ? KEY_AUTO_RELEASE_HOLD : KeyValue, path);
         }
#if DEBUG_VP4001         
         InitMsgCntr();
#endif
      break;

    case KEY_SHIFT_AUTO_RELEASE:
         path = HandleKeyMilkingShiftAutoRelease();
         ReportKeyMilk(KeyValue, path);
      break;

    case KEY_SHIFT_2_AUTO_RELEASE:
         path = HandleKeyMilkingShift2AutoRelease();
         ReportKeyMilk(KeyValue, path);
      break;

    case KEY_MANUAL_REL_CLUSTER:
         FirstAttach = 1;
         path = HandleKeyMilkingManualReleaseCluster();
         ReportKeyMilk(KeyHold == TRUE ? KEY_MANUAL_REL_CLUSTER_HOLD : KEY_MANUAL_REL_CLUSTER, path);
      break;

    case KEY_SHIFT_MANUAL_REL_CLUSTER:
         path = HandleKeyMilkingShiftManualReleaseCluster();
         ReportKeyMilk(KeyValue, path);
      break;

    case KEY_SHIFT_2_MANUAL_REL_CLUSTER:
         HandleKeyMilkingShift2ManualReleaseCluster();
         ReportKeyMilk(KeyValue, 0);
      break;

    case '1':
         HandleKeyMilkingOne();
      break;

    case '2':
         HandleKeyMilkingTwo();
      break;

    case '9':
         HandleKeyMilkingNine();
      break;

    case '6':
    case '5':
    case '4':
    case '3':
    case KEY_SHIFT_0:
    case KEY_SHIFT_1:
    case KEY_SHIFT_2:
    case KEY_SHIFT_3:
    case KEY_SHIFT_4:
    case KEY_SHIFT_5:
    case KEY_SHIFT_6:
    case KEY_SHIFT_7:
    case KEY_SHIFT_8:
    case KEY_SHIFT_9:
    case KEY_SHIFT_K3_GATE:
    case KEY_SHIFT_K4_GATE:
    case KEY_SHIFT_K5_GATE:
         if ( ParlourTandemTypeAndMilkingMode() ){
           HandleKeyOpenTandem(KeyValue, KeyHold);
         }
         KeyState = KS_NORMAL;
      break;

    case KEY_K3_GATE:                // CENTRAL GATE *******************
         HandleKeyMilkingK3Gate();
      break;
    case KEY_K4_GATE :
         HandleKeyMilkingK4Gate();
      break;
    case KEY_K5_GATE :
         HandleKeyMilkingK5Gate();
      break;

    case KEY_SHIFT_WASH:
         HandleKeyMilkingShiftWash();
      break;

    case KEY_WASH:
         HandleKeyMilkingWash();
      break;

    case KEY_CLEAR:
         HandleKeyMilkingClear();
      break;

    case KEY_SHIFT_CLEAR:
         HandleKeyMilkingShiftClear();
      break;

    case KEY_DISEASE_INFO:
         HandleKeyMilkingDiseaseInfo();
      break;

    case KEY_COW_INFO:
         HandleKeyMilkingCowInfo();
      break;

    case KEY_MILK_INFO:
         HandleKeyMilkingMilkInfo();
      break;

    case KEY_SHIFT_MILK_INFO:
         HandleKeyMilkingShiftMilkInfo();
      break;

    case KEY_FEED_INFO:
         HandleKeyMilkingFeedInfo();
      break;

    case KEY_SHIFT_FEED_INFO:
         HandleKeyMilkingShiftFeedInfo();
      break;

    case KEY_CALENDAR_INFO:
         HandleKeyMilkingCalendarInfo();
      break;

    case KEY_START_PULSE:
         HandleKeyMilkingStartPulse();
      break;
    case KEY_STOP_PULSE:
         HandleKeyMilkingStopPulse();
      break;

    case KEY_SHIFT:
         KeyState     = KS_CONTINUE;
         KeyShiftTime = Cnt10msec;
      break;
    default :
      break;
  }
}
