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
#include "../comm/flowctrl.h"

#pragma codeseg(EEPAGE)


unsigned char  SensorState, MilkFlowCor, NrOfReedPulses;
unsigned short TimeLastFlowCalc;
unsigned short StartCleanPhase1, StartCleanPhase2, StartCleanPhase4, ClnPhsStartTime;
unsigned short LastValveStartTime, ValveTime;
unsigned long  MilkAmountLastFlowCalc;

extern void HandleCleanPhase(void);

// =============================================================================
// MEMOLAC MEASURING INTERRUPT ROUTINES
// =============================================================================
void HndlMemolacM1Int(void)
{
  //Disable input capture 3 interrupt
  TMSK1 &= ~IC3I;
  TFLG1  =  IC3F;

  SensorState |= BOTTOM_REED_SENSOR;
}

// =============================================================================
void HndlMemolacM2Int(void)
{
  // Disable input capture 2 interrupt
  TMSK1 &= ~IC2I;
  TFLG1  =  IC2F;

  SensorState |= MIDDLE_REED_SENSOR;

}

// =============================================================================
void HndlMemolacM3Int(void)
{
	// Disable input capture 1 interrupt
  TMSK1 &= ~IC1I;
  TFLG1  =  IC1F;

  SensorState |= TOP_REED_SENSOR;

  //         if ( PORTD & P_5) PORTD &= ~P_5;
  //         else              PORTD |=  P_5;

  //Activate milkmeter valve
  if ( !((MlkMode == MM_CLEANING)  &&
         (CleanState == FILL_MILK_METER) &&
         ((CleanPhase == CLEAN_PHASE_I2_CONTROLLED_START)||(CleanPhase == CLEAN_PHASE_I2_CONTROLLED_0) || (CleanPhase == CLEAN_PHASE_1)) )){
    PortP1On();
    ValveStartTime = Cnt10msec;
    ValveState     = VALVE_ON;
  }
}

// =============================================================================
// MEMOLAC VALVE HANDLING
// =============================================================================
void HndlValve(void)
{
  switch (ValveState) {
    case VALVE_ON_EXTRA_DUMP:
    case VALVE_ON:
         if (TimeControl( Cnt10msec, ValveStartTime + (ValveTime/10))) {
           // Test reed contact
           if (EEMmSensorMem == TRUE){
             // Reedcontact normally closed ??
             if ((PORTA & P_2) && (ValveState == VALVE_ON)) {
               // Reedcontact open, vlotter niet gezakt. Er bestaat een mogelijkheid
               // dat de vlotter vastzit, hiervoor geven we de klep weer even vrij
               ValveState = VALVE_SHORT_REL;
             } else {
               // Reedcontact closed, vlotter gezakt
               PortP1Off();
               ValveState = VALVE_OFF;
               TFLG1  = IC1F + IC2F + IC3F;
               TMSK1 |= IC1I + IC2I + IC3I;

               // Reset tijd zodat er geen extra puls wordt gegeven
               LastValveStartTime = Cnt1sec;
             }
           } else {
             // Reedcontact normally open
             if ((PORTA & P_2) || (ValveState == VALVE_ON_EXTRA_DUMP)) {
               // Reedcontact closed, vlotter gezakt
               PortP1Off();
               ValveState = VALVE_OFF;
               TFLG1  = IC1F + IC2F + IC3F;
               TMSK1 |= IC1I + IC2I + IC3I;

               // reset tijd zodat er geen extra puls wordt gegeven
               LastValveStartTime = Cnt1sec;
             } else {
               // Reedcontact open, vlotter niet gezakt. Er bestaat een mogelijkheid
               // dat de vlotter vastzit, hiervoor geven we de klep weer even vrij
               PortP1Off();
               ValveState = VALVE_SHORT_REL;
             }
           }
         }
      break;

    case VALVE_START:
    case VALVE_START_EXTRA_DUMP:
         // Activate milkmeter valve
         PortP1On();
         ValveStartTime = Cnt10msec;
         if ( ValveState == VALVE_START_EXTRA_DUMP )ValveState = VALVE_ON_EXTRA_DUMP;
         else                                       ValveState = VALVE_ON;
      break;
    case VALVE_RELEASE:
         // de-Activate milkmeter valve
         PortP1Off();
         ValveState = VALVE_OFF;
      break;
    case VALVE_SHORT_REL:
    case VALVE_SHORT_REL_EXTRA_DUMP:
         // Release milkmeter valve for short time
         PortP1Off();
         if ( (MlkMode == MM_CLEANING) && ((CleanPhase == CLEAN_PHASE_END) || (CleanPhase == CLEAN_PHASE_I2_CONTROLLED_END))){
           // indien reinigings fase veranderd dan stoppen
           ValveState = VALVE_OFF;
         } else {
           ValveStartTime = Cnt10msec;
           if ( ValveState == VALVE_SHORT_REL_EXTRA_DUMP) ValveState = VALVE_DELAY_EXTRA_DUMP;
           else                                           ValveState = VALVE_DELAY;
         }
      break;
    case VALVE_DELAY:
    case VALVE_DELAY_EXTRA_DUMP:
         // Wait for activating Valve Again
         if (TimeControl( Cnt10msec, ValveStartTime + 20)) {
           if (ValveState == VALVE_DELAY_EXTRA_DUMP) ValveState = VALVE_START_EXTRA_DUMP;
           else                                      ValveState = VALVE_START;
         }
      break;

    default:
    break;
  }
}


// =============================================================================
// HndlMemoCP_Controlled
// =============================================================================
void HndlMemoCP_Controlled(void) 
{
  ValveState = VALVE_START_EXTRA_DUMP;
}

// =============================================================================
// HndlMemoCP_Controlled_1
// =============================================================================
void HndlMemoCP_Controlled_1(void) 
{
  if (ValveState == VALVE_ON) {
    ClnPhsStartTime  = Cnt1sec;
    CleanState       = EMPTY_MILK_METER;
    StartCleanPhase1 = Cnt1sec;
    CleanPhase = CLEAN_PHASE_I2_CONTROLLED;
  }
  HndlValve();
}

// =============================================================================
// HndlMemoCP_Controlled_Valve
// =============================================================================
void HndlMemoCP_Controlled_Valve(void) 
{
  HndlValve();
}

// =============================================================================
// HndlMemoCP_Controlled_0_1
// =============================================================================
void HndlMemoCP_Controlled_0_1(void) 
{
  if (ValveState == VALVE_OFF ){
    CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
  }
}

// =============================================================================
// HndlMemoCP_ControlledStart
// =============================================================================
void HndlMemoCP_ControlledStart(void) 
{
  TMSK1 |= IC1I +IC2I + IC3I;
  ValveState       = VALVE_START;
//  HndlValve();
}

// =============================================================================
// HndlMemoCP_PreClean
// =============================================================================
void HndlMemoCP_PreClean(void) 
{
  if (TimeControl( (unsigned short)Cnt1sec, LastValveStartTime + 10)) {
    ValveState         = VALVE_START;
    LastValveStartTime = Cnt1sec;
  }
  HndlValve();
}

// =============================================================================
// HndlMemoCP_End
// =============================================================================
void HndlMemoCP_End(void) 
{
  if ( ValveState <= VALVE_DELAY )ValveState = VALVE_RELEASE;
  HndlValve();
}

// =============================================================================
// HndlMemoCP_2
// =============================================================================
void HndlMemoCP_2(void) 
{
  if (TimeControl((unsigned short)Cnt1sec, LastValveStartTime + 30)) {
    ValveState         = VALVE_START;
    LastValveStartTime = Cnt1sec;
  }
  HndlValve();
}

// =============================================================================
// HndlMemoCP_CleanState
// =============================================================================
void HndlMemoCP_CleanState(void)
{
  switch (CleanState) {
    case  EMPTY_MILK_METER:
          if (TimeControl((unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEEmptyTime))) {
            CleanState = FILL_MILK_METER;
            ValveState = VALVE_RELEASE;
            ClnPhsStartTime = Cnt1sec;
          }
     break;
    case  FILL_MILK_METER:
          if ( TimeControl( (unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEFillTime) )) {
            CleanState      =  EMPTY_MILK_METER;
            ClnPhsStartTime = Cnt1sec;
            ValveState      = VALVE_START;
          } else {
            ValveState = VALVE_RELEASE;
          }
    break;
  }
  HndlValve();
}

// =============================================================================
// HndlMemoCP_1_STOP
// =============================================================================
void HndlMemoCP_1_STOP(void) 
{
  ClnPhsStartTime  = Cnt1sec;
  ValveState       = VALVE_START;
  HndlValve();
}

// =============================================================================
// HndlMemoCP_0
// =============================================================================
void HndlMemoCP_0(void) 
{
  if (ValveState > VALVE_DELAY ) {
    HndlValve();
  } else {
    if (ValveState == VALVE_ON) {
      HndlValve();
      CleanPhase       = CLEAN_PHASE_1;
      StartCleanPhase1 = Cnt1sec;
      ClnPhsStartTime  = Cnt1sec;
      CleanState       = EMPTY_MILK_METER;
    }
  }
}

// =============================================================================
// HndlMemoCPStart
// =============================================================================
void HndlMemoCPStart(void) 
{
  ValveState = VALVE_START;
  HndlValve();
}

/*
// =============================================================================
// HndlMemoMlkStateDone
// =============================================================================
void HndlMemoMlkStateDone(void)
{
  if (MilkDump == TRUE) { 
    ValveState = VALVE_START;
    MilkDump = FALSE; 
  }
}

// =============================================================================
// MemolacMMMeas
// =============================================================================
void MemolacMMMeas(void)
{
  // Milk yield calculation:
  if (MilkManualInput == FALSE) {
    MlkAmount += CalcMilkYieldMem();
    MlkAmountKg = (MlkAmount+50)/100;
  }

  // save milk yield every second, to determine milk flow
  UpdateAndSendMlkState(MlkAmount, MlkState);
}
*/

// =============================================================================
// Bereken Flow Correctie factor
// =============================================================================
void CalcFlowCor(void)
{
  unsigned long ActFlowGr;
  unsigned char ActFlowKg;

  ActFlowGr = ((MlkAmount - MilkAmountLastFlowCalc)*60)/ (Cnt1sec -TimeLastFlowCalc);
  ActFlowKg = ActFlowGr/1000;

  MilkAmountLastFlowCalc = MlkAmount;
  TimeLastFlowCalc = Cnt1sec;

  switch (ActFlowKg) {
    case  0: { /* ValveTime = EEValveTime;   */ MilkFlowCor = EECorFlow0;  break; }
    case  1: { /* ValveTime = EEValveTime1;  */ MilkFlowCor = EECorFlow1;  break; }
    case  2: { /* ValveTime = EEValveTime2;  */ MilkFlowCor = EECorFlow2;  break; }
    case  3: { /* ValveTime = EEValveTime3;  */ MilkFlowCor = EECorFlow3;  break; }
    case  4: { /* ValveTime = EEValveTime4;  */ MilkFlowCor = EECorFlow4;  break; }
    case  5: { /* ValveTime = EEValveTime5;  */ MilkFlowCor = EECorFlow5;  break; }
    case  6: { /* ValveTime = EEValveTime6;  */ MilkFlowCor = EECorFlow6;  break; }
    case  7: { /* ValveTime = EEValveTime7;  */ MilkFlowCor = EECorFlow7;  break; }
    case  8: { /* ValveTime = EEValveTime8;  */ MilkFlowCor = EECorFlow8;  break; }
    case  9: { /* ValveTime = EEValveTime9;  */ MilkFlowCor = EECorFlow9;  break; }
    case 10: { /* ValveTime = EEValveTime10; */ MilkFlowCor = EECorFlow10; break; }
    case 11: { /* ValveTime = EEValveTime11; */ MilkFlowCor = EECorFlow11; break; }
    case 12: { /* ValveTime = EEValveTime12; */ MilkFlowCor = EECorFlow12; break; }
    case 13: { /* ValveTime = EEValveTime13; */ MilkFlowCor = EECorFlow13; break; }
  }
}

// =============================================================================
// Bereken Melk gift
// =============================================================================
unsigned short CalcMilkYieldMem(void)
{
  unsigned short MilkAdd;
  unsigned char CorConMem;

  CorConMem = EEMmCorMem;
  MilkAdd = 0;

  if (SensorState & BOTTOM_REED_SENSOR) {
    MilkAdd +=  CorConMem + MilkFlowCor;
    disable_interrupt();
    SensorState &= ~BOTTOM_REED_SENSOR;
    SensorState |= CALC_BOTTOM_REED_SENSOR;
    enable_interrupt();
    NrOfReedPulses++;
  }
  if (SensorState & MIDDLE_REED_SENSOR) {
    MilkAdd +=  CorConMem + MilkFlowCor;
    disable_interrupt();
    SensorState &= ~MIDDLE_REED_SENSOR;
    SensorState |= CALC_MIDDLE_REED_SENSOR;
    enable_interrupt();
    NrOfReedPulses++;
  }
  if (SensorState & TOP_REED_SENSOR) {
    MilkAdd +=  CorConMem + MilkFlowCor;
    disable_interrupt();
    // Test of van alle sensoren de melk is berekend
    if (!(SensorState & CALC_BOTTOM_REED_SENSOR)) {
      SensorState &= ~CALC_BOTTOM_REED_SENSOR;
      NrOfReedPulses++;
      MilkAdd +=  CorConMem + MilkFlowCor;
    }
    if (!(SensorState & CALC_MIDDLE_REED_SENSOR)) {
      SensorState &= ~CALC_MIDDLE_REED_SENSOR;
      NrOfReedPulses++;
      MilkAdd +=  CorConMem + MilkFlowCor;
    }
    SensorState &= ~TOP_REED_SENSOR;
    enable_interrupt();
    NrOfReedPulses++;
  }

  // na elke vijf pulsen melkstroom bepalen
  if (NrOfReedPulses >= 5) {
    CalcFlowCor();
    NrOfReedPulses = 0;
  }

  return(MilkAdd);
}


// =============================================================================
// HandleCleanPhase
// =============================================================================
/*
MemolacMMInit(void)
{
  // Init memolac milk measuring
  SensorState =0;
  // Set Interrupt registers
  TCTL2 = 0;
  //Capture on any edge
  TCTL2 = EDG1A + EDG1B + EDG2A + EDG2B + EDG3A + EDG3B;
  TFLG1 = IC1F + IC2F + IC3F;
  TMSK1 |= IC1I +IC2I + IC3I;

  // ResetFloatPosition();
  ValveState = VALVE_START;
  ValveTime = EEValveTime;                           // initate valve_time
  HndlValve();
  MilkFlowCor = EECorFlow0;
  NrOfReedPulses = 0;
  MilkAmountLastFlowCalc = 0;
  TimeLastFlowCalc = Cnt1sec;
}
*/

// =============================================================================
// HandleCleanPhase
// =============================================================================
/*
void HandleCleanPhase(void)
{
  switch (CleanPhase) {
    case CLEAN_PHASE_START:                         // CleanPhase 0
         ValveState       = VALVE_START;
         HndlValve();
         CleanPhase       = CLEAN_PHASE_0;
         StartCleanPhase1 = Cnt1sec;

         if ( (EEI2Control & 0x04) || (EEI2Control == 0x00) ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      break;

    case CLEAN_PHASE_0:                             // CleanPhase 3
         // Tien seconden wachten na opstarten (anders start reiniging direct omdat
         // klep tijdens initialiseren wordt bekrachtigd
         if (Cnt1sec > 10  && Cnt1sec > (StartCleanPhase1 + 1)) {
           if (ValveState > VALVE_DELAY ) {
             HndlValve();
           } else {
             if (ValveState == VALVE_ON) {
               HndlValve();
               CleanPhase       = CLEAN_PHASE_1;
               StartCleanPhase1 = Cnt1sec;
               ClnPhsStartTime  = Cnt1sec;
               CleanState       = EMPTY_MILK_METER;
             }
           }

           // controleer of de pulsatie AAN of UIT moet
           CheckPulsation();
         } else {
           HndlValve();
         }
      break;

    case CLEAN_PHASE_1:                             // CleanPhase 4
         // Reinigingsfase 1 verstreken
         if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase1 + (60 * EEClnPhs1))) {
           CleanPhase       = CLEAN_PHASE_2;
           StartCleanPhase2 = Cnt1sec;
           ClnPhsStartTime  = Cnt1sec;
           ValveState       = VALVE_START;
           HndlValve();
         } else {
           // controleer of de pulsatie AAN of UIT moet
           CheckPulsation();

           switch (CleanState) {
             case EMPTY_MILK_METER:
               if ( TimeControl( (unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEEmptyTime))) {
                 CleanState = FILL_MILK_METER;
                 ValveState = VALVE_RELEASE;
//                 HndlValve();
                 ClnPhsStartTime = Cnt1sec;
               }
               HndlValve();
             break;
             case FILL_MILK_METER:
               if ( TimeControl( (unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEFillTime) )) {
                 CleanState      =  EMPTY_MILK_METER;
                 ClnPhsStartTime = Cnt1sec;
                 ValveState      = VALVE_START;
//                 HndlValve();
               } else {
                 ValveState = VALVE_RELEASE;
               }
               HndlValve();
             break;
           }
         }
      break;
    case CLEAN_PHASE_2:                             // CleanPhase 5
         // Reinigingsfase 2 verstreken, zoja stop de pulsatie
         if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase2 + (60 * EEClnPhs2))) {
           PlsState    = PLS_STOP_PULSATION;
           if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
           else                                      VacuumState = VACUUM_ON;
           CleanPhase  = CLEAN_PHASE_END;
         } else{
           // controleer of de pulsatie AAN of UIT moet
           CheckPulsation();

           if ( TimeControl( (unsigned short)Cnt1sec, LastValveStartTime + 30)) {
             ValveState         = VALVE_START;
             LastValveStartTime = Cnt1sec;
           }
           HndlValve();
         }
      break;
    case CLEAN_PHASE_END:                           // CleanPhase 7
         // finish extra dump
         if ( ValveState <= VALVE_DELAY )ValveState = VALVE_RELEASE;
         HndlValve();

         if ( ((EEI2Control & 0x03) != 1) && EERowSize ){
           SetStartSwitchState();
           if ( StartSwitchState == SWITCH_CLOSED ){
             Cnt1sec          = 0;
             StartCleanPhase4 = Cnt1sec;
             ValveState       = VALVE_START;
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
    case CLEAN_PHASE_PRE_CLEAN:                     // CleanPhase 8
         // Reinigingsfase 2 verstreken
         if (TimeControl( (unsigned short)Cnt1sec, StartCleanPhase4 + (60 * EEClnPhs4))) {
           PlsState    = PLS_STOP_PULSATION;
           CleanPhase  = CLEAN_PHASE_END;
         } else{
           // controleer of de pulsatie AAN of UIT moet
           CheckPulsation();

           if ( TimeControl( (unsigned short)Cnt1sec, LastValveStartTime + 10)) {
             ValveState         = VALVE_START;
             LastValveStartTime = Cnt1sec;
           }
           HndlValve();
         }
      break;
    case CLEAN_PHASE_I2_CONTROLLED_START:                // CleanPhase 9
         SetStartSwitchState();
         //
         //   2: afhankelijk van de communicatie; de 'Master' moet I2 in de gaten houden
         //   1: onafhankelijk van I2
         //   0: afhankelijk van I2;
         //

        if (Cnt1sec > 10 ) {
          if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)) {
            // set MPC's in I2C wash mode, centraal
            if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
              ExternMessages |= SEND_WASH_MODE_I2C_START_MESSAGES;
            }
            CleanPhase  = CLEAN_PHASE_I2_CONTROLLED_0;
            TMSK1 |= IC1I +IC2I + IC3I;
            ValveState       = VALVE_START;
//            HndlValve();  
            if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_ON;
          }
        }
        HndlValve();
      break;

    case CLEAN_PHASE_I2_CONTROLLED_0:                    // CleanPhase 10
         // Tien seconden wachten na opstarten (anders start reiniging direct omdat
         // klep tijdens initialiseren wordt bekrachtigd
         if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
           // wassen afhankelijk van I2 of afhankelijk van Master
           SetStartSwitchState();

           if (Cnt1sec > 10 ) {
             if ( ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD))){
//                      (((EEI2Control & 0x03) == 0x02) && (ValveState == VALVE_ON)) ){
               if ( ValveState == VALVE_OFF ){
                 CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
               }
             } else {
               if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)) {
                 if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
                   ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
                   CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                   if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
                 } else {
                   if ( ((EEI2Control & 0x03) == 0x00)){
                     CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                     if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
                   }
                 }
               }
             }
           }
         } else {
           // slave; start
           if ( ValveState == VALVE_OFF ){
             CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
           }
         }
         // controleer of de pulsatie AAN of UIT moet
         CheckPulsation();
         HndlValve();
      break;

    case CLEAN_PHASE_I2_CONTROLLED_1:                  // CleanPhase 11
         if ( ValveState == VALVE_ON ){
           ClnPhsStartTime  = Cnt1sec;
           CleanState       = EMPTY_MILK_METER;
           StartCleanPhase1 = Cnt1sec;
           CleanPhase = CLEAN_PHASE_I2_CONTROLLED;
         }

         CheckPulsation();
         HndlValve();
      break;

    case CLEAN_PHASE_I2_CONTROLLED:                  // CleanPhase 11

         switch (CleanState) {
           case EMPTY_MILK_METER:
                if ( TimeControl( (unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEEmptyTime))) {
                  CleanState = FILL_MILK_METER;
                  ValveState = VALVE_START_EXTRA_DUMP;
                  ClnPhsStartTime = Cnt1sec;
                }
            break;
           case FILL_MILK_METER:
                if ( TimeControl( (unsigned short)Cnt1sec, ClnPhsStartTime + abs(EEFillTime) )) {
                  CleanState      =  EMPTY_MILK_METER;
                  ClnPhsStartTime = Cnt1sec;
                  ValveState      = VALVE_START;
                } else {
                  if ( ValveState < VALVE_ON_EXTRA_DUMP ){
                    ValveState = VALVE_RELEASE;
                  }
                }
            break;
         }
         HndlValve();


         // controleer of de pulsatie AAN of UIT moet
         CheckPulsation();
         SetStartSwitchState();

         if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
           // MASTER
           if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
             CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
             ValveState = VALVE_START_EXTRA_DUMP;
             PlsState   = PLS_STOP_PULSATION;

             // set MPC's in I2C wash mode, centraal
             ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
             if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
           }
         } else {
           if ((EEI2Control & 0x03) == 0x00){
           	 // afhankelijk van I2
             if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
               ValveState = VALVE_START_EXTRA_DUMP;
               PlsState   = PLS_STOP_PULSATION;
               if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
             }
           }
         }
      break;
    case CLEAN_PHASE_I2_CONTROLLED_END:                   // CleanPhase 12
         // MASTER en afhankelijk van I2; vanuit CONTROLLED_START worden de slaves opgestart
         SetStartSwitchState();
         if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){

           if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
             CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
           }
         } else {
           if ((EEI2Control & 0x03) == 0x00){
           	 // afhankelijk van I2
             if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
             }
           }
         }
         HndlValve();
      break;
  } // switch CleanPhase
}
*/

// =============================================================================
// MEMOLAC MEASURING
//   called every second.
//   EEI2Control determines how the MPC is set in cleaning mode
// =============================================================================
void MemolacMeasuring(void)
{
  static unsigned char Init     = FALSE;
  static unsigned char MilkDump = FALSE;

  switch (MlkState) {
    case MLK_START_CLEANING:                            // MlkState 20
    case MLK_CLEANING:                                  // MlkState 21
    case MLK_CLEANING_END:                              // MlkState 23
      if (Init == FALSE) {
        MlkMode = MM_INIT;
        if ( (EEI2Control & 0x07) == 0x01) CleanPhase = CLEAN_PHASE_START;
        else                               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      } else {
        MlkMode = MM_CLEANING;
      }
    case MLK_CLEANING_END_I2:                           // MlkState 24
      break;
    case MLK_DHIA:                                      // MlkState 103
      // Reset melkgift
      if ((KeyA == KEY_CLEAR && ScreenNumber == SCREEN_RESET) || MlkMode ==MM_CLEANING) {
        MlkMode = MM_INIT;
      } else {
        MlkMode = MM_DHIA;
      }
    break;
    case MLK_WAIT_FOR_START:                            // MlkState 11
      if (MlkMode == MM_CLEANING) {
        MlkMode = MM_INIT;
        if ( (EEI2Control & 0x07) ==  0x01) CleanPhase = CLEAN_PHASE_START;
        else                                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      }
    break;

    case MLK_SERVICE:                                   // MlkState 101
      MlkMode = MM_SERVICE;
    break;
    case MLK_RESTART_CLEANING:
    	break;
    default:
      if ( (EEI2Control & 0x07) == 0x01 ) CleanPhase = CLEAN_PHASE_START;
      else                                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
    break;
  }

  switch (MlkMode) {
    case MM_INIT:                                        // MlkMode 0
      // Init memolac milk measuring
      SensorState =0;
      // Set Interrupt registers
      TCTL2 = 0;
      //Capture on any edge
      TCTL2 = EDG1A + EDG1B + EDG2A + EDG2B + EDG3A + EDG3B;
      TFLG1 = IC1F + IC2F + IC3F;
      TMSK1 |= IC1I +IC2I + IC3I;

      // ResetFloatPosition();
      ValveState = VALVE_START;
      ValveTime = EEValveTime;                           // initate valve_time
      HndlValve();
      MilkFlowCor = EECorFlow0;
      MilkAmountLastFlowCalc = 0;
      TimeLastFlowCalc = Cnt1sec;

      NrOfReedPulses = 0;
      StoredMlkAmount = 45;
      SendMlkAmount = 45;
      MlkAmount = 45;
      MlkAmountKg = 0;
      MlkDuration = 0;
      MlkActFlow = 0;
      MlkMaxFlow = 0;
      if (Init != FALSE )   MlkMode = MM_MEAS;
      Init    = TRUE;
      MilkDump = TRUE;                // extra dump bij versturen van melk result
    break;


    case MM_MEAS:                                       // MlkMode 1
      HndlValve();
      switch(MlkState) {

        //Milking??
        case MLK_DONE:                                  // MlkState 8
             if (MilkDump == TRUE){ ValveState = VALVE_START; MilkDump = FALSE; }
        case MLK_IDLE:                                  // MlkState 0
        case MLK_NEUTRAL:                               // MlkState 2
        case MLK_MEASURING:                             // MlkState 3
        case MLK_TAKE_OFF_WAIT:                         // MlkState 4
        case MLK_TAKE_OFF_MAN:                          // MlkState 10
        case MLK_SEND_WAIT:                             // MlkState 6
          // Milk yield calculation:
          if (MilkManualInput == FALSE) {
            MlkAmount += CalcMilkYieldMem();
            MlkAmountKg = (MlkAmount+50)/100;
          }

          // save milk yield every second, to determine milk flow
          UpdateAndSendMlkState(MlkAmount, MlkState);
        break;
        default:
        break;
      }
    break;

    case MM_DHIA:                                       // MlkMode 3
      HndlValve();

      // Melkberekenen
      MlkAmount += CalcMilkYieldMem();
      MlkAmountKg = (MlkAmount+50)/100;
    break;

    case MM_SERVICE:                                    // MlkMode 5
      HndlValve();
    break;

    case MM_CLEANING:                                   // MlkMode 4
         HandleCleanPhase();
      break;

    default :
      break;
  } // switch MlkMode
}


