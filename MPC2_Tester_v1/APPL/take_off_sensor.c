/*****************************************************************/
/* Memolac Milkmeter                                            */
/****************************************************************/

#include "memolac.h"
#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/time.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h"
#include "../lib/intr6811.h"
#include "../lib/stdlib.h"
#include "../key/key.h"
#include "../key/keyint.h"
#include "../appl/takeoff.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/pulsation.h"

#pragma codeseg(EEPAGE)

unsigned char take_off_sensor_ratio;
unsigned char take_off_sensor_level                                                         ;
// =============================================================================
// MEMOLAC MEASURING
// =============================================================================
void TakeOffSensorMeasuring(void)
{
  static unsigned char Init     = FALSE;

  switch (MlkState) {
    case MLK_START_CLEANING:                            // STATE 20
    case MLK_CLEANING:                                  // STATE 21
    case MLK_CLEANING_END:                              // STATE 23
      if (Init == FALSE) {
        MlkMode = MM_INIT;
        if ( EEI2Control & 0x04 ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
        else                      CleanPhase = CLEAN_PHASE_START;
      } else {
        MlkMode = MM_CLEANING;
      }
    break;
    case MLK_DHIA:                                      // STATE 103
      // Reset melkgift
      if ((KeyA == KEY_CLEAR && ScreenNumber == SCREEN_RESET) || MlkMode ==MM_CLEANING) {
        MlkMode = MM_INIT;
      } else {
        MlkMode = MM_DHIA;
      }
    break;
    case MLK_WAIT_FOR_START:                            // STATE 11
      if (MlkMode == MM_CLEANING) {
        MlkMode = MM_INIT;
        if ( EEI2Control & 0x04 ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
        else                      CleanPhase = CLEAN_PHASE_START;
      }
    break;

    case MLK_SERVICE:                                   // STATE 101
      MlkMode = MM_SERVICE;
    break;

    default:
      if ( EEI2Control & 0x04 ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      else                      CleanPhase = CLEAN_PHASE_START;
    break;
  }

  switch (MlkMode) {
    case MM_INIT    :                                   // STATE 0
      // Init memolac milk measuring
      StoredMlkAmount = 45;
      MlkAmount = 45;
      MlkAmountKg = 0;
      MlkDuration = 0;
      MlkActFlow = 0;
      MlkMaxFlow = 0;
      MlkMode = MM_MEAS;
      Init = TRUE;
      FlowState = 1;
    break;

    case MM_CLEANING:                                   // STATE 4
      switch (CleanPhase) {
        case CLEAN_PHASE_START:                         // STATE 0
          CleanPhase       = CLEAN_PHASE_0;
          StartCleanPhase1 = Cnt1sec;
        break;

        case CLEAN_PHASE_0:                             // STATE 3
          // Tien seconden wachten na opstarten (anders start reiniging direct omdat
          // klep tijdens initialiseren wordt bekrachtigd
          if (Cnt1sec > 10  && Cnt1sec > (StartCleanPhase1 + 1)) {
            StartCleanPhase1 = Cnt1sec;
            ClnPhsStartTime  = Cnt1sec;

            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          }
        break;

        case CLEAN_PHASE_1:                             // STATE 4
          // Reinigingsfase 1 verstreken
          if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase1 + (60 * EEClnPhs1))) {
            CleanPhase       = CLEAN_PHASE_2;
            StartCleanPhase2 = Cnt1sec;
            ClnPhsStartTime  = Cnt1sec;
          } else {
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          }
        break;
        case CLEAN_PHASE_2:                             // STATE 5
          // Reinigingsfase 2 verstreken, zoja stop de pulsatie
          if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase2 + (60 * EEClnPhs2))) {
            PlsState    = PLS_STOP_PULSATION;
            VacuumState = VACUUM_ON;
            CleanPhase  = CLEAN_PHASE_END;
          } else{
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          }
        break;

        case CLEAN_PHASE_END:                           // STATE 7
          // finish extra dump
          if ( ((EEI2Control & 0x03) == 0) || (((EEI2Control & 0x03) == 0x02) && EERowSize)){
            SetStartSwitchState();
            if ( StartSwitchState == SWITCH_CLOSED ){
              Cnt1sec          = 0;
              StartCleanPhase4 = Cnt1sec;
              VacuumState      = VACUUM_ON;
              CleanPhase       = CLEAN_PHASE_PRE_CLEAN;

              // set MPC's in pre-wash mode, centraal
              if ( EERowSize ){
                 ExternMessages |= SEND_WASH_MODE_4_START_MESSAGES;
              }
            }
          }

        break;

        case CLEAN_PHASE_PRE_CLEAN:                     // STATE 8
             // Reinigingsfase 2 verstreken
             if (TimeControl( (unsigned short)Cnt1sec, StartCleanPhase4 + (60 * EEClnPhs4))) {
               PlsState    = PLS_STOP_PULSATION;
               VacuumState = VACUUM_ON;
               CleanPhase  = CLEAN_PHASE_END;
             } else{
               // controleer of de pulsatie AAN of UIT moet
               CheckPulsation();
             }
          break;
        case CLEAN_PHASE_I2_CONTROLLED_START:
             // set MPC's in I2C wash mode, centraal
             if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
               ExternMessages |= SEND_WASH_MODE_I2C_START_MESSAGES;
             }
             CleanPhase = CLEAN_PHASE_I2_CONTROLLED_0;
          break;

        case CLEAN_PHASE_I2_CONTROLLED_0:
             // Tien seconden wachten na opstarten (anders start reiniging direct omdat
             // klep tijdens initialiseren wordt bekrachtigd
             if ( ((EEI2Control & 0x03) == 0) || (((EEI2Control & 0x03) == 0x02) )){
               // wassen afhankelijk van I2 of afhankelijk van Master
               SetStartSwitchState();

               if (Cnt1sec > 10 ) {
                 if ( ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)) ||
                      (((EEI2Control & 0x03) == 0x02) && (ValveState == VALVE_ON)) ){
                     ClnPhsStartTime  = Cnt1sec;
                     StartCleanPhase1 = Cnt1sec;
                     CleanPhase = CLEAN_PHASE_I2_CONTROLLED;
                 }
               }
             }
             // controleer of de pulsatie AAN of UIT moet
             CheckPulsation();
          break;
        case CLEAN_PHASE_I2_CONTROLLED:
             // controleer of de pulsatie AAN of UIT moet
             CheckPulsation();
             if ( ((EEI2Control & 0x03) == 0) || (((EEI2Control & 0x03) == 0x02) && EERowSize)){
               SetStartSwitchState();
               if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
                 CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;

                 // set MPC's in I2C wash mode, centraal
                 if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
                   ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
                 }
               }
             }
          break;
        case CLEAN_PHASE_I2_CONTROLLED_END:
             if ( ((EEI2Control & 0x03) == 0) || (((EEI2Control & 0x03) == 0x02) && EERowSize)){
               SetStartSwitchState();

               if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
                 CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
               }
             }
          break;
      }
    break;

    case MM_MEAS:                                       // STATE 1
    case MM_DHIA:                                       // STATE 3
    case MM_SERVICE:                                    // STATE 5
    default :
      break;
  } // switch MlkMode
}


