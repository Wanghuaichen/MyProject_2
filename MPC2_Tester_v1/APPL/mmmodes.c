/*****************************************************************/
/* General wash Milkmeter                                            */
/****************************************************************/

#include "memolac.h"
#include "vflow.h"
#include "mm_4001.h"
#include "../lib/time.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h"
#include "../key/key.h"
#include "../appl/takeoff.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/pulsation.h"

#pragma codeseg(EEPAGE)


// =============================================================================
// HandleCleanPhase
// =============================================================================
void HandleCleanPhase(void)
{
  switch  (MilkMeterType) {
    case  MMT_TAKE_OFF_SENSOR:
//                  TakeOffSensorMMCleaning();
      break;
    case  MMT_NIVEAU :
//                  NiveauMMCleaning();
      break;
    case  MMT_VFLOW:
          VFlowMMDefault();
      break;
    case  MMT_VP4001:
          VP4001MMDefault();
      break;
    default :
      break;
  }

  switch (CleanPhase) {
    case  CLEAN_PHASE_START:                         // STATE 0
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCPStart();
          }
          CleanPhase       = CLEAN_PHASE_0;
          StartCleanPhase1 = Cnt1sec;

          if ( (EEI2Control & 0x04) || (EEI2Control == 0x00) ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      break;

    case  CLEAN_PHASE_0:                             // STATE 3
          // Tien seconden wachten na opstarten (anders start reiniging direct omdat
          // klep tijdens initialiseren wordt bekrachtigd
          if (Cnt1sec > 10  && Cnt1sec > (StartCleanPhase1 + 1)) {
            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_0();
            } else {
              CleanPhase       = CLEAN_PHASE_1;
              StartCleanPhase1 = Cnt1sec;
              ClnPhsStartTime  = Cnt1sec;
            }
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          } else {
            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_Controlled_Valve();
            } else {
              // controleer of de pulsatie AAN of UIT moet
              CheckPulsation();
            }
          }
      break;

    case  CLEAN_PHASE_1:                             // STATE 4
          // Reinigingsfase 1 verstreken
          if (TimeControl( (unsigned short)Cnt1sec, StartCleanPhase1 + (60 * EEClnPhs1))) {
            CleanPhase       = CLEAN_PHASE_2;
            StartCleanPhase2 = Cnt1sec;
            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_1_STOP();
            }
          } else {
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_CleanState();
            }
          }
      break;
   case  CLEAN_PHASE_2:                             // STATE 5
         // Reinigingsfase 2 verstreken, zoja stop de pulsatie
         if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase2 + (60 * EEClnPhs2))) {
           PlsState    = PLS_STOP_PULSATION;
           if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
           else                                      VacuumState = VACUUM_ON;
           CleanPhase  = CLEAN_PHASE_END;
         } else{
           // controleer of de pulsatie AAN of UIT moet
           CheckPulsation();
           if (MilkMeterType == MMT_MEMOLAC) {
             HndlMemoCP_2();
           }
         }
     break;

    case  CLEAN_PHASE_END:                           // STATE 7
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCP_End();
          }

          // finish extra dump
          if ( ((EEI2Control & 0x03) != 1) && EERowSize ){
            SetStartSwitchState();
            if ( StartSwitchState == SWITCH_CLOSED ){
              Cnt1sec          = 0;
              StartCleanPhase4 = Cnt1sec;
              if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
              else                                      VacuumState = VACUUM_ON;
              CleanPhase       = CLEAN_PHASE_PRE_CLEAN;

              // set MPC's in pre-wash mode, centraal
              if ( EERowSize ){
                 ExternMessages |= SEND_WASH_MODE_4_START_MESSAGES;
              }
            }
          }
      break;

    case  CLEAN_PHASE_PRE_CLEAN:                     // STATE 8
          // Reinigingsfase 2 verstreken
          if (TimeControl((unsigned short)Cnt1sec, StartCleanPhase4 + (60 * EEClnPhs4))) {
            PlsState    = PLS_STOP_PULSATION;
            CleanPhase  = CLEAN_PHASE_END;
          } else {
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();

            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_PreClean();
            }
          }
      break;
    case  CLEAN_PHASE_I2_CONTROLLED_START:                // state 9
          SetStartSwitchState();

          //   2: afhankelijk van de communicatie; de 'Master' moet I2 in de gaten houden
          //   1: onafhankelijk van I2
          //   0: afhankelijk van I2;
          if (Cnt1sec > 10 ) {
            if ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)) {
              // set MPC's in I2C wash mode, centraal
              if ((((EEI2Control & 0x03) == 0x02) && EERowSize)){
                ExternMessages |= SEND_WASH_MODE_I2C_START_MESSAGES;
              }
              CleanPhase  = CLEAN_PHASE_I2_CONTROLLED_0;
              if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_ON;

              if (MilkMeterType == MMT_MEMOLAC) {
                HndlMemoCP_ControlledStart();
              }
            }
          }
      break;

    case  CLEAN_PHASE_I2_CONTROLLED_0:                    // state 10
          // Tien seconden wachten na opstarten (anders start reiniging direct omdat
          // klep tijdens initialiseren wordt bekrachtigd
          if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
            // wassen afhankelijk van I2 of afhankelijk van Master
            SetStartSwitchState();

            if (Cnt1sec > 10 ) {
              if (((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD))){
                if (MilkMeterType == MMT_MEMOLAC) {
                  HndlMemoCP_Controlled_0_1();
                } else {
                  CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
                }
              } else {
                if ((StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)) {
                  if ((((EEI2Control & 0x03) == 0x02) && EERowSize)){
                    ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
                    CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                    if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
                  } else {
                    if (((EEI2Control & 0x03) == 0x00)){
                      CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                      if( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
                    }
                  }
                }
              }
            }
          } else {
            // slave; start
            if (MilkMeterType == MMT_MEMOLAC) {
              HndlMemoCP_Controlled_0_1();
            } else {
              CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
            }
          }
          // controleer of de pulsatie AAN of UIT moet
          CheckPulsation();
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCP_Controlled_Valve();
          }
      break;

    case  CLEAN_PHASE_I2_CONTROLLED_1:                  // state 11
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCP_Controlled_1();
          } else {
            ClnPhsStartTime  = Cnt1sec;
            StartCleanPhase1 = Cnt1sec;
            CleanPhase = CLEAN_PHASE_I2_CONTROLLED;
          }
          CheckPulsation();
      break;

    case  CLEAN_PHASE_I2_CONTROLLED:                  // state 11
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCP_CleanState();
          }

          // controleer of de pulsatie AAN of UIT moet
          CheckPulsation();
          SetStartSwitchState();

          if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
            // MASTER
            if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
              CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
              PlsState   = PLS_STOP_PULSATION;

              if (MilkMeterType == MMT_MEMOLAC) {
                HndlMemoCP_Controlled();
              }

              // set MPC's in I2C wash mode, centraal
              ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
              if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
            }
          } else {
            if ((EEI2Control & 0x03) == 0x00){
               // afhankelijk van I2
              if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                PlsState   = PLS_STOP_PULSATION;
                if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;

                if (MilkMeterType == MMT_MEMOLAC) {
                  HndlMemoCP_Controlled();
                }
              }
            }
          }
      break;
    case  CLEAN_PHASE_I2_CONTROLLED_END:                   // state 12
          // MASTER en afhankelijk van I2; vanuit CONTROLLED_START worden de slaves opgestart
          SetStartSwitchState();
          if ((((EEI2Control & 0x03) == 0x02) && EERowSize)){

            if ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
              CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
            }
          } else {
            if ((EEI2Control & 0x03) == 0x00) {
               // afhankelijk van I2
              if ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
              }
            }
          }
          if (MilkMeterType == MMT_MEMOLAC) {
            HndlMemoCP_Controlled_Valve();
          }
      break;
  } // switch CleanPhase
}

// =============================================================================
// Milk Meter Modes
// =============================================================================
void HandleMMModes(unsigned char MilkMeterType)
{
  static unsigned char Init     = FALSE;
  static unsigned char MilkDump = FALSE;

  switch (MlkState) {
    case  MLK_START_CLEANING:                            // STATE 15
    case  MLK_CLEANING:                                  // STATE 16
    case  MLK_CLEANING_END:                              // STATE 18
          if (Init == FALSE) {
            MlkMode = MM_INIT;
            if ( (EEI2Control & 0x07) == 0x01) CleanPhase = CLEAN_PHASE_START;
            else                               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
          } else {
            MlkMode = MM_CLEANING;
          }
    case  MLK_CLEANING_END_I2:                           // STATE 19
      break;
    case  MLK_DHIA:                                      // STATE 103
          // Reset melkgift
          if ((KeyA == KEY_CLEAR && ScreenNumber == SCREEN_RESET) || MlkMode ==MM_CLEANING) {
            MlkMode = MM_INIT;
          } else {
            MlkMode = MM_DHIA;
          }
    break;
    case  MLK_WAIT_FOR_START:                            // STATE 11
          if (MlkMode == MM_CLEANING) {
            MlkMode = MM_INIT;
            if ( (EEI2Control & 0x07) == 0x01) CleanPhase = CLEAN_PHASE_START;
            else                               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
          }
    break;

    case  MLK_SERVICE:                                   // STATE 101
          MlkMode = MM_SERVICE;
    break;
    case  MLK_RESTART_CLEANING:
          break;
    default:
          if ( (EEI2Control & 0x07) == 0x01 ) CleanPhase = CLEAN_PHASE_START;
          else                                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
    break;
  }

  switch (MlkMode) {
    case  MM_INIT    :                                   // STATE 0
          // Init milk meter measuring; global variables and milk-meter dependent
          MlkAmount = 45;
          MlkAmountKg = 0;
          MlkDuration = 0;
          MlkActFlow = 0;
          MlkMaxFlow = 0;
          StoredMlkAmount = 45;
          SendMlkAmount = 45;

          switch  (MilkMeterType) {
            case  MMT_VFLOW:
                  VFlowMMInit();
              break;
            case  MMT_VP4001:
                  VP4001MMInit();
              break;
            default :
              break;
          }
          if (Init != FALSE ) {
            MlkMode = MM_MEAS;
          }
          Init    = TRUE;
          MilkDump = TRUE;                // extra dump bij versturen van melk result
    break;


    case  MM_MEAS:                                       // STATE 1
          switch(MlkState) {
            //Milking??
            case  MLK_DONE:                                  // STATE 8
//                  if (MilkMeterType == MMT_MEMOLAC) {
//                  }
            case  MLK_IDLE:                                  // STATE 0
            case  MLK_SEND_WAIT:                             // STATE 6
            case  MLK_SEND:                                  // STATE 7
                  switch  (MilkMeterType) {
                    case  MMT_VFLOW:
                          VFlowMMDone();
                      break;
                    case  MMT_VP4001:
                          VP4001MMDone();
                      break;
                    default :
                      break;
                  }
              break;
            case  MLK_NEUTRAL:                               // STATE 2
            case  MLK_MEASURING:                             // STATE 3
            case  MLK_TAKE_OFF_WAIT:                         // STATE 4
            case  MLK_TAKE_OFF:                              // STATE 5
            case  MLK_TAKE_OFF_MAN:                          // STATE 10
                  switch  (MilkMeterType) {
                    case  MMT_VFLOW:
                          VFlowMMMeas();
                      break;
                    case  MMT_VP4001:
                          VP4001MMMeas();
                      break;
                    default :
                      break;
                  }
              break;
            case  MLK_CONTINUE:                              // STATE 10
                  switch  (MilkMeterType) {
                    case  MMT_VFLOW:
                          VFlowMMContinue();
                      break;
                    case  MMT_VP4001:
                          VP4001MMContinue();
                      break;
                    default :
                      break;
                  }
              break;
            default:
                  switch  (MilkMeterType) {
                    case  MMT_VFLOW:
                          VFlowMMDefault();
                      break;
                    case  MMT_VP4001:
                          VP4001MMDefault();
                      break;
                    default :
                      break;
                  }
            break;
          }

    break;

    case  MM_DHIA:                                       // STATE 3
    case  MM_SERVICE:                                    // STATE 5
      break;

    case  MM_CLEANING:                                   // STATE 4
          HandleCleanPhase();
      break;

    default :
      break;
  } // switch MlkMode
}

// =============================================================================
// isMilkingActive
// =============================================================================
unsigned char isMilkingActive(void)
{
  unsigned char rval = 0;

  if (MlkMode == MM_MEAS) {
    switch(MlkState) {
      case  MLK_NEUTRAL:                               // STATE 2
      case  MLK_MEASURING:                             // STATE 3
      case  MLK_TAKE_OFF_WAIT:                         // STATE 4
      case  MLK_TAKE_OFF:                              // STATE 5
      case  MLK_TAKE_OFF_MAN:                          // STATE 10
            rval = 1;
        break;
    }
  }
  return rval;
}
