#include "../key/disp.h"
#include "../key/key.h"
#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/rti_int.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/time.h"
#include "../lib/extport.h"
#include "../include/romeehc1.h"
#include "../comm/flowctrl.h"
#include "../key/keyint.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/pulsation.h"
#include "../appl/take_off_sensor.h"
#include "../appl/takeoff.h"
#include "../appl/tandem.h"
#include "../appl/conduct.h"


unsigned short pls_on_time;
unsigned char  on;


// =============================================================================
// HandleScreenMilk0DisplayMilkProcess
// Handles data during milking process
// =============================================================================
void HandleScreenMilk0DisplayMilkProcess(void)
{
  unsigned char   idx;

  // kan eventueel overschreven worden indien EETest = 2
  if (MilkMeterType == MMT_NIVEAU && !EERobot ) {
    // Wordt melkglas geledigd
    if ( ExtOutputP1State == P1_OFF_ON || ExtOutputP1State == P1_OFF ) {
      AsciiToDisp('n', 7, 1);
    } else {
      AsciiToDisp(' ', 7, 1);
    }
  }

  if (EETest == FALSE) {
    if ( TakeOffState == TO_MAN ) {
      AsciiToDisp('h', 6, 1);
    } else {
      if ( ParlourTandemTypeAndMilkingMode() && (IC13PrgVrs >= 3)){
        // display status dmv 3 streepjes + tandem status, wordt ondersteund
        // vanaf versie 3.
        switch( MlkState ){
          case MLK_NEUTRAL:
               if ( MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED )  AsciiToDisp(0x7F, 6, 1);
               else {
                 if ((MpcOtsInfo.station_state & (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) == (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) {
                   AsciiToDisp(0x90, 6, 1);   //
                 } else {
                   if ( MpcOtsInfo.station_state & OT_EXIT_GATE_BLOCKED ){
                     if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x98, 6, 1);
                     else                             AsciiToDisp(0x94, 6, 1);
                   } else {
                     if ( MpcOtsInfo.station_state & OT_ENTR_GATE_BLOCKED ) {
                       if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x94, 6, 1);
                       else                             AsciiToDisp(0x98, 6, 1);
                     } else {
                       AsciiToDisp(0x8b, 6, 1);   // 3 streepjes
                     }
                   }
                 }
               }
            break;
          case MLK_MEASURING:
               if ( MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED )     AsciiToDisp(0x80, 6, 1);
               else {
                 if ((MpcOtsInfo.station_state & (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) == (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) {
                   AsciiToDisp(0x91, 6, 1);   //
                 } else {
                   if ( MpcOtsInfo.station_state & OT_EXIT_GATE_BLOCKED ) {
                     if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x99, 6, 1);
                     else                             AsciiToDisp(0x95, 6, 1);
                   } else {
                     if ( MpcOtsInfo.station_state & OT_ENTR_GATE_BLOCKED ){
                       if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x95, 6, 1);
                       else                             AsciiToDisp(0x99, 6, 1);
                     } else {
                       AsciiToDisp( '=', 6, 1);   // 2 streepjes
                     }
                   }
                 }
               }
            break;
          case MLK_TAKE_OFF_WAIT:
               if ( MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED )     AsciiToDisp(0x81, 6, 1);
               else {
                    if ((MpcOtsInfo.station_state & (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) == (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) {
                   AsciiToDisp(0x92, 6, 1);
                 } else {
                   if ( MpcOtsInfo.station_state & OT_EXIT_GATE_BLOCKED ) {
                     if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x9A, 6, 1);
                     else                             AsciiToDisp(0x96, 6, 1);
                   } else {
                     if ( MpcOtsInfo.station_state & OT_ENTR_GATE_BLOCKED ) {
                       if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x96, 6, 1);
                       else                             AsciiToDisp(0x9A, 6, 1);
                     } else {
                       AsciiToDisp( '_', 6, 1);   // 1 streepjes
                     }
                   }
                 }
               }
            break;
          default:
               if ( MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED )     AsciiToDisp(0x82, 6, 1);
               else {
                 if ((MpcOtsInfo.station_state & (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) == (OT_EXIT_GATE_BLOCKED+OT_ENTR_GATE_BLOCKED)) {
                   AsciiToDisp(0x93, 6, 1);   //
                 } else {
                   if ( MpcOtsInfo.station_state & OT_EXIT_GATE_BLOCKED ) {
                     if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x9B, 6, 1);
                     else                             AsciiToDisp(0x97, 6, 1);
                   } else {
                     if ( MpcOtsInfo.station_state & OT_ENTR_GATE_BLOCKED ) {
                       if ( OTRowSide == OT_LEFT_SIDE ) AsciiToDisp(0x97, 6, 1);
                       else                             AsciiToDisp(0x9B, 6, 1);
                     } else {
                       AsciiToDisp( ' ', 6, 1);
                     }
                   }
                 }
               }
        } // end switch(MlkState)
      } else {
        // display melk status dmv 3 streepjes
        idx = GetDisplaySide();
        if ( idx == SideState ){
          switch( MlkState ){
            case MLK_NEUTRAL:
                 AsciiToDisp(0x8b, 6, 1);
                 pls_on_time  = Cnt10msec;
              break;  // 3 streepjes
            case MLK_MEASURING:
                  // indicator for traction arm
                  if ( PlsState == PLS_TA_STIM_PULSATION ){
                    if (TimeControl( Cnt10msec, pls_on_time +10)){
                      pls_on_time  = Cnt10msec;
                      if (on) on = 0;
                      else    on = 1;
                    }
                  } else {
                    pls_on_time  = Cnt10msec;
                    on = 1;
                  }
                  if ( on){
                    AsciiToDisp( '=', 6, 1);   // 1 streepjes
                  } else {
                    AsciiToDisp( ' ', 6, 1);   // 1 streepjes
                  }
              break;  // 2

            case MLK_TAKE_OFF_WAIT:
                  // indicator for traction arm
                  if ( PlsState == PLS_TA_STIM_PULSATION ){
                    if (TimeControl( Cnt10msec, pls_on_time +10)){
                      pls_on_time  = Cnt10msec;
                      if (on) on = 0;
                      else    on = 1;
                    }
                  } else {
                    pls_on_time  = Cnt10msec;
                    on = 1;
                  }
                  if ( on){
                    AsciiToDisp( '_', 6, 1);   // 1 streepjes
                  } else {
                    AsciiToDisp( ' ', 6, 1);   // 1 streepjes
                  }
              break;  // 1 streepje
            default:
                 pls_on_time  = Cnt10msec;
                 AsciiToDisp( ' ', 6, 1);
              break;
          }
        }
      }
    }
  } else {
    // Voor testdoeleinden
    if (  (EETest == 2) &&
         ((ParlourType >= PT_SEMI_TANDEM) && (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR)) &&
          (OTRowMaster == EEAddress) ){
      // display OTMasterState en 'ToWaitFor'
      AsciiToDisp((unsigned char)(ATMasterState + 0x30), 6, 1);
      AsciiToDisp((unsigned char)(MpcOtmParlourInfo.gate_to_wait_for + 0x30), 7, 1);
    } else {
      AsciiToDisp((unsigned char)(MlkState + 0x30), 6, 1);
    }
  }
}

// =============================================================================
// HandleScreenMilk0NoMeasMode
// Handles data to be displayed while not actually milking
// =============================================================================
void HandleScreenMilk0NoMeasMode(void )
{
  unsigned char   idx;
  unsigned short  DummyValue = 0;

  if ( ( MilkMeterType >= MMT_NO_MILK_METER ) ){
    memory_set((unsigned char *)&(DispTable[7]), ' ', 3);
  } else {
    if ( MilkMeterType == MMT_TAKE_OFF_SENSOR ){
      DispValue((unsigned char *)&take_off_sensor_ratio, _U1, 3, 0, WRITE, POS_8);
      if ( take_off_sensor_level) disp_point = 0x0020;
      else                        disp_point = 0x0000;
    } else {
      idx = GetDisplaySide();
      if ( (idx != SideState && SwingOver()) ){
        // zet op write zodat in KM_INPUT de invoer mogelijk wordt, hier wordt adhv
        // screen het juiste item geselecteerd
        DispValue((unsigned char *)&MlkResOtherSide, _U2, 3, 1, WRITE, POS_8);
      } else {
        DispValue((unsigned char *)&DummyValue, _U2, 3, 1, WRITE, POS_8);
      }
    }
  }
}

// =============================================================================
// HandleScreenMilk0MeasModeOrStandAlone
// Handles data to be displayed during milking
// =============================================================================
unsigned char HandleScreenMilk0MeasModeOrStandAlone(void)
{
  unsigned char   idx;
  unsigned short  DummyValue = 0;

  // Melkgift weergeven in stand_alone en meetmode
  idx = GetDisplaySide();
  if ((MeasState[idx] == MEAS_MEAS && MlkAmountKg > 0) ||
       MeasState[idx] == MEAS_DONE                     ||
      (EEAddressCopy == 0)                             ||
     ((p_MlkRef[idx] != NULL) && MilkManualInput) ) {

    if (  MilkMeterType >= MMT_NO_MILK_METER ){
      // MMT_NO_MILK_METER:
      memory_set((unsigned char *)&(DispTable[7]), ' ', 3);
    } else {
      switch ( MilkMeterType) {
        case MMT_NIVEAU:
             // Geiten melkglas ??
             if (EEGoatMilkGlass !=TRUE ) {
               //Normale niveau melkmeter
               if (((idx != SideState && SwingOver()) ? MlkResOtherSide : MlkAmountKg ) >= 25){
                 DispValue((idx != SideState && SwingOver()) ? ((unsigned char *)&MlkResOtherSide) :
                                      ( (unsigned char *)&MlkAmountKg), _U2, 3, 1, WRITE, POS_8);
               } else {
                 DispValue((unsigned char *)&DummyValue, _U2,  3, 1, WRITE, POS_8);
               }
             } else {
               // Geiten melkglas, weergave melkgift > 0.3 kg
               if (((idx != SideState && SwingOver()) ? MlkResOtherSide : MlkAmountKg ) >= 295 ) {
                 DispValue((idx != SideState && SwingOver()) ? ((unsigned char *)&MlkResOtherSide) :
                                      ( (unsigned char *)&MlkAmountKg), _U2, 3, 1, WRITE, POS_8);
               } else {
                 DispValue((unsigned char *)&DummyValue, _U2,  3, 1, WRITE, POS_8);
               }
             }
          break;
        case MMT_VP4001:
        case MMT_VFLOW:
             DispValue((idx != SideState && SwingOver()) ? ((unsigned char *)&MlkResOtherSide) :
                                      ((unsigned char *)&MlkAmountKg), _U2, 3, 1, WRITE, POS_8);
          break;
        case MMT_MEMOLAC:
             if ((((idx != SideState && SwingOver()) ? (MlkResOtherSide) : ( MlkAmountKg )) > 2)) {
               DispValue((idx != SideState && SwingOver()) ? ((unsigned char *)&MlkResOtherSide) :
                                      ((unsigned char *)&MlkAmountKg), _U2, 3, 1, WRITE, POS_8);
             } else {
               DispValue((unsigned char *)&DummyValue,  _U2, 3, 1, WRITE, POS_8);
             }
          break;
        case MMT_TAKE_OFF_SENSOR:
             DispValue((unsigned char *)&take_off_sensor_ratio, _U1, 3, 0, WRITE, POS_8);
             if ( take_off_sensor_level) disp_point = 0x0020;
             else                        disp_point = 0x0000;
          break;
        default:
             DispValue((idx != SideState && SwingOver()) ? ((unsigned char *)&MlkResOtherSide) :
                                      ( (unsigned char *)&MlkAmountKg), _U2, 3, 1, WRITE, POS_8);
          break;
      } //  end switch( MilkMeterType )
    }
    return 1;
  }
  return 0;
}

static unsigned char GetNrOfItems(unsigned short att)
{
  unsigned char i, x;

  for ( x = 0, i = 0; i < 16; i++){
    if( att & (0x0001<<i))x++;
  }
  return x;
}

static unsigned short GetAttentionToDisplay(unsigned short att, unsigned char item)
{
  unsigned short v;
  unsigned char i,x;

  for ( x = 0, i = 0; i < 16; i++){
    v = 0x0001<<i;
    if(att & v)x++;
    if (x == item) return v;
  }
  return 0;
}

// =============================================================================
// HandleScreenMilk0DisplayAttention
// Handles dispaly attentions
// =============================================================================
unsigned char HandleScreenMilk0DisplayAttention( void )
{
  unsigned char   mask, item, idx;
  unsigned short  t, att, DummyValue = 0;
  void           *CowDiseaseRefPtr;

  // Indien LampAttentie deze weergeven tot melkstart
  // GetDisplayAtt() returns all attentions; display them all!
  idx = GetDisplaySide();
  att = GetDisplayAtt(idx);

  mask = GetNrOfItems(att)+1;

  /*
  // has to be called all the time  because the function HandleScreenMilk0DisplayRespNr puts in the responder-
  // number every time. So data has to be set again.!!!
  */
  item = Cnt1sec % mask;

  if (item && att){
    // zorg er voor dat er niets ingevuld kan worden
    DispValue((unsigned char *)&DummyValue, _U2, 5, 0, READ_ONLY, POS_LEFT);
    t = GetAttentionToDisplay(att, item);

    switch (t)  {
      case  DPL_SEP_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MILK_SEP]), 10);
        break;
      case  DPL_COLOST_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_COLOST]), 10);
        break;
      case  DPL_DRY_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_DRY]), 10);
        break;
      case  DPL_ILL_ATT1:
      case  DPL_ILL_ATT2:
      case  DPL_ILL_ATT3:
      case  DPL_ILL_ATT4:
      case  DPL_ILL_ATT5:
            CowDiseaseRefPtr = (void *)(&p_CowDiseaseMail[idx]->message_start);

            // offset wordt bepaald door t een x aantal plaatsen naar rechts te schuiven
            DispValue(LONG_COW_GEN_NUMBER ?
                (unsigned char *)&(((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array[(t>>DPL_ILL_ATT1_OFFSET)-1].CowDiseaseCode) :
                (unsigned char *)&(((   CowDiseaseRef *)CowDiseaseRefPtr)->Array[(t>>DPL_ILL_ATT1_OFFSET)-1].CowDiseaseCode), _U1, 2, 0, READ_ONLY, POS_RIGHT);

            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ILL_ATT]), 7);
            Display();
        break;
      case  DPL_COND_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CONN_PATT]), 10);
        break;
      case  DPL_ACT_COND_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CONN_ATT]), 10);
        break;
      case  DPL_CM_ERR:
            DispValue((unsigned char *)&(CMErrorInfo.CMErrCode), _U2, 1, 0, READ_ONLY, POS_RIGHT);
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CM_ERROR]), 9);
        break;
      case  DPL_TM_ERR:
//            DispValue((unsigned char *)&xatt, _U2, 1, 0, READ_ONLY, POS_RIGHT);
//            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_TM_ERROR]), 9);
        break;
      case  DPL_TMP_ATT:
            memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_TEMP_ATT]), 10);
        break;
      case DPL_COMM_ATT:
      	    memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_ERR_VC4]), 10);
      	break;  
    }
    return 1;
  }
  return 0;
}

// =============================================================================
// HandleScreenMilk0DisplayRespNr
// Displays responder number
// =============================================================================
void HandleScreenMilk0DisplayRespNr( void )
{
  unsigned char   idx;
  unsigned short  DummyValue = 0;

  DispTable[6] = DispTable[5] = ' ';

  // display respondernummer op eerste vijf posities
  // moet hier blijven staan ivm punt in melk-gift
  idx = GetDisplaySide();
  if (p_MlkRef[idx] != NULL) {
    DispValue(LONG_COW_GEN_NUMBER ?
               (unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber :
               (unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowGenNumber, _U2, 5, 0, WRITE, POS_LEFT);
  } else {
    if ( EEAddressCopy != 0 ){
      DispValue((unsigned char *)&DummyValue, _U2, 5, 0, WRITE, POS_LEFT);
    }
  }

  // tbv 'rare characters die blijven staan
  if ( SwingOver() ){
    DispTable[6] = idx == SIDE_A ? 'A' : 'B';
    if ( idx != SideState ) DispFlash |= 0x0008;      // ------x---
  }
}

// =============================================================================
// HandleScreenMilk0_StandAlone
// Handles data to be displayed when MPC is stand-alone
// =============================================================================
void HandleScreenMilk0_StandAlone(void )
{
  unsigned char   idx;

  idx = GetDisplaySide();

  // bepaal welke gegevens moeten worden gedisplayed
  DispTable[6] = DispTable[5] = DispTable[4] = DispTable[0]= ' ';

  if ( p_CowFedParlRef[idx] != NULL ){
    if ( p_CowFedParlRes[idx] != NULL ){
      ExtraFeedDpl = (signed int)((( ExtraFeed[idx]  + (LONG_COW_GEN_NUMBER ?
                    ((((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance) :
                     (((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance))) -
                     (LONG_COW_GEN_NUMBER ?
                     (((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed :
                     (((   CowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed))/100;
    } else {
      ExtraFeedDpl = (signed int)( ExtraFeed[idx] + (LONG_COW_GEN_NUMBER ?
                     ((((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance) :
                     ((((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance)))/100;
    }
    DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_2);
  } else{
    if ( p_CowFedParlRes[idx] != NULL ){
      ExtraFeedDpl = (signed int)( ExtraFeed[idx] - (LONG_COW_GEN_NUMBER ?
                     ((((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed) :
                     ((((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed)))/100;
    } else {
      ExtraFeedDpl = (signed int)(ExtraFeed[idx]/100);
    }
    DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_2);
  }
  // tbv rare characters en 'side' aanduiding
  if ( SwingOver() ){
    DispTable[4] = idx == SIDE_A ? 'A' : 'B';
    if ( idx != SideState ) DispFlash |= 0x0020;     // ----x-----
  }
}


// =============================================================================
// HandleScreenMilking
// Handles data to be displayed in milking mode
// =============================================================================
void HandleScreenMilking(void)
{
  unsigned short  DummyValue = 0;
  unsigned char   idx;

  DummyValue = 0;
  idx = GetDisplaySide();


  switch (ScreenNumber) {
    case SCREEN_MILK_0:

         // standalone?
         if (EEAddressCopy ==0){
            memory_set((unsigned char *)&(DispTable[0]), ' ', 5);
         }

         if ( StandAloneFeeding() ){
           DispTable[6] = DispTable[5] = DispTable[4] = DispTable[0]= ' ';
           HandleScreenMilk0MeasModeOrStandAlone();
         } else {
            DispTable[6] = DispTable[5] = ' ';
           HandleScreenMilk0DisplayRespNr();
         }

         if (  !HandleScreenMilk0DisplayAttention() ){
           if ( !HandleScreenMilk0MeasModeOrStandAlone() ){
             HandleScreenMilk0NoMeasMode();
           }
           HandleScreenMilk0DisplayMilkProcess();

           // Bepaal of display of delen er van moeten knipperen
           if (MlkState == MLK_TAKE_OFF_MAN || ((MlkAttention == 1) && (MilkManualInput == FALSE))) {
             DispFlash = 0xffff;
           } else {
             // geef extra aandacht aan activiteits attentie
             if ( ((p_CowActRef[idx] != NULL &&
                    (LONG_COW_GEN_NUMBER ?
                    ((ExtCowActRef **)p_CowActRef)[idx]->CowActAttention :
                    ((   CowActRef **)p_CowActRef)[idx]->CowActAttention))  ||
                   (p_CowActMemAttRef[idx] != NULL &&
                    (LONG_COW_GEN_NUMBER ?
                    ((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt :
                   ((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt )))  &&
                ( (MlkState == MLK_WAIT_FOR_START) || (MlkState == MLK_DONE))){
               // laat koe-nr blinken
               DispFlash |= 0xffe0;
             }
           }
         }
      break;
    default:
      break;
  } // end switch( ScreenNumber )
}
