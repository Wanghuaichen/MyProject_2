#include "../lib/io6811.h"
#include "../lib/intr6811.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/extport.h"
#include "../lib/boolean.h"
#include "../lib/time.h"
#include "../lib/rti_int.h"
#include "../lib/reg_flags.h"
#include "../lib/stdlib.h"
#include "../include/romeehc1.h"
#include "../key/keyint.h"
#include "../key/key.h"
#include "../comm/flowctrl.h"
#include "takeoff.h"
#include "milkmisc.h"
#include "milk.h"
#include "pulsation.h"




#define PLS_MIN_RATIO    5
#define PLS_MAX_RATIO   70
#define PLS_MIN_FREQ     5
#define PLS_MAX_FREQ   360


PLS_PULSE_CNTRL PLSCntrl;

  signed short LP4_Counter;
  signed short LP5_Counter;
  signed short HP4_Counter;
  signed short HP5_Counter;

unsigned char  PlsState;
unsigned short at, xt;
unsigned char  KeyExtPulseControl;


#pragma codeseg(EEPAGE)


// =============================================================================
void InitPulsation( void )
{
  PlsState = PLS_IDLE;

//  if ( (EEPulseRatio     < PLS_MIN_RATIO) || (EEPulseRatio     > PLS_MAX_RATIO)) WriteEEpromByte( 60, &EEPulseRatio );
//  if ( (EEPulseFrequency < PLS_MIN_FREQ ) || (EEPulseFrequency > PLS_MAX_FREQ) ) WriteEEpromByte( 60, &EEPulseFrequency );
  PLSCntrl.FirstTimeStimulation = FALSE;
  PLSCntrl.PlsMsgControlled     = FALSE;
  PLSCntrl.PlsFreqCorr          = 0;
  PLSCntrl.PlsRatCorr           = 0;

  // tbv pulsatie, indien automatisch dan togglepuls op 1 zodat bij bediening
  // toets de stimulatie uit gezet wordt
  if ((EEPulsation & (PLS_ENABLE+PLS_MANUAL_ENABLE)) == PLS_ENABLE) PLSCntrl.ManualPulseToggle = 1;
  else                                                              PLSCntrl.ManualPulseToggle = 0;
  HPRIO = (HPRIO & 0xF0) | 0x0c; // OC2 high priority
}

// =============================================================================
// pulsatie met maximaal 1 sec vertragen.
// =============================================================================
void SetPulsation( void )
{
//	AbsTime = 0xffff;  // stond erin sinds versie 80, waarom ??
  at = (unsigned short)AbsTime;
  xt = ((EEAddress*50)/EENrOfPulsators);
  if (xt > 100) xt = xt % 100;
  xt = Cnt10msec + xt;
}

// =============================================================================
unsigned char CheckStartPulsation( void )
{
  unsigned char rval = 0;

  // test op stand-alone
  if ( EEAddress == 0 )return 1;

//  if ( TimeControl( (unsigned short)AbsTime, at)){
  if ((short)AbsTime != at){	
    if (TimeControl(Cnt10msec, xt))rval = 1;
  }
  return rval;
}

// =============================================================================
void StartPulsation( void )
{

  PORTA &= ~P_4;                                               // P4 laag  (rest)
  if ( EEOutputK2 != MILK_AUTOMAT_CONTROL )  PORTA &= ~P_5;    // P5 laag  (rest)

  LP4_Counter = 0;                                             // set P4 low-counter direct start
  LP5_Counter = (PLSCntrl.PTLowBack+PLSCntrl.PTHighBack)/2;    // set P5 low-counter on half puls time

}

// =============================================================================
void PulsationToRest( void )
{
  PORTA |= P_4;                                                // P4 hoog
  if ( EEOutputK2 != MILK_AUTOMAT_CONTROL ) PORTA |= P_5;      // P5 hoog
}

// =============================================================================
void StopPulsation(void)
{
  PORTA &= ~P_4;                                               // P4 laag
  if ( EEOutputK2 != MILK_AUTOMAT_CONTROL )PORTA &= ~P_5;      // P5 laag
}


// =============================================================================
unsigned char SetPulsationTimes( unsigned char mode )
{
  unsigned char  r_val = 0;
  unsigned short b_ratio, f_ratio;
  unsigned short b_freq,  f_freq;

  PLSCntrl.PlsMsgControlled = FALSE;
  switch (mode){
    case PLS_TA_STIM_PULSATION:
         if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
         	(((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIP_STIM) :
         	(((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIP_STIM)) ){
           PLSCntrl.PlsMsgControlled = TRUE;
           if ( LONG_COW_GEN_NUMBER ) {
             b_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripRatBack;
             f_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripRatFront;
             b_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripFrqBack;
             f_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripFrqFront;
           } else {
             b_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripRatBack;
             f_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripRatFront;
             b_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripFrqBack;
             f_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripFrqFront;
           }
         } else {
           f_ratio = b_ratio = EETAPulseRatio;
           f_freq  = b_freq  = EETAPulseFreq;
         }
      break;

    case STIMULATION_PULSE_MODE:
         f_ratio = b_ratio = EEStimPulseRatio;
         f_freq  = b_freq  = EEStimPulseFreq;

         if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
         	    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STIMULATION) :
         	    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STIMULATION) )){
           PLSCntrl.PlsMsgControlled = TRUE;
           if ( LONG_COW_GEN_NUMBER ) {
             if ( ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatBack ){
               b_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatBack;
               f_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatFront;
               b_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmFrqBack;
               f_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmFrqFront;
             }
           } else {
           	 if ( ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatBack ){
               b_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatBack;
               f_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmRatFront;
               b_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmFrqBack;
               f_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmFrqFront;
             }
           }
         }
        break;

    default:
         f_ratio = b_ratio = EEPulseRatio;
         f_freq  = b_freq  = EEPulseFrequency;

         if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
         	    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_PULSATION) :
         	    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_PULSATION) )){
           PLSCntrl.PlsMsgControlled = TRUE;
           if ( LONG_COW_GEN_NUMBER ) {
             if ( ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatBack){
             	 b_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatBack;
               f_ratio = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatFront;
               b_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqBack;
               f_freq  = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqFront;
             }
           } else {
           	 if ((( CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatBack ){
               b_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatBack;
               f_ratio = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatFront;
               b_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqBack;
               f_freq  = ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqFront;
             }
           }
         }
       break;
  } // end switch(mode)

  if ( (mode != PLS_TA_STIM_PULSATION) &&
  	    (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
  	    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_CORR) :
  	    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_CORR) )){
    // pas eventueel een correctie toe, deze is afhankelijk van de flow
    if ( !((PLSCntrl.PlsRatCorr <= 0) &&
    	      (MlkDurationCur < (LONG_COW_GEN_NUMBER ?
    	      (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime ):
    	      (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime) )))){
      r_val = 1;
      (unsigned short)b_ratio = (signed short)b_ratio + (signed short)((PLSCntrl.PlsRatCorr*(signed short)b_ratio)/100);
      (unsigned short)f_ratio = (signed short)f_ratio + (signed short)((PLSCntrl.PlsRatCorr*(signed short)f_ratio)/100);
    }
    // pas eventueel een correctie toe, deze is afhankelijk van de flow
    if ( !((PLSCntrl.PlsFreqCorr <= 0) &&
    	      (MlkDurationCur < (LONG_COW_GEN_NUMBER ?
    	      (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime ):
    	      (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime) )))){
      r_val = 1;
      (unsigned short)b_freq  = (signed short)b_freq  + (signed short)((PLSCntrl.PlsFreqCorr*(signed short)b_freq)/100);
      (unsigned short)f_freq  = (signed short)f_freq  + (signed short)((PLSCntrl.PlsFreqCorr*(signed short)f_freq)/100);
    }
  }

  // controleer op minimum en maximum
  if      ( f_ratio < PLS_MIN_RATIO ) f_ratio = PLS_MIN_RATIO;
  else if ( f_ratio > PLS_MAX_RATIO ) f_ratio = PLS_MAX_RATIO;

  if      ( b_ratio < PLS_MIN_RATIO ) b_ratio = PLS_MIN_RATIO;
  else if ( b_ratio > PLS_MAX_RATIO ) b_ratio = PLS_MAX_RATIO;

  if      ( f_freq < PLS_MIN_FREQ ) f_freq = PLS_MIN_FREQ;
  else if ( f_freq > PLS_MAX_FREQ ) f_freq = PLS_MAX_FREQ;

  if      ( b_freq < PLS_MIN_FREQ ) b_freq = PLS_MIN_FREQ;
  else if ( b_freq > PLS_MAX_FREQ ) b_freq = PLS_MAX_FREQ;

  // bepaal hoog/laag tijden in 10 msec
  // Th = ((60 (sec/min) * 100 (NrOf10MS/S) * ratio / Freq)+50(afronding))/100
  // Tl = (((6000  * (100 - ratio) / Freq )/100)
  PLSCntrl.PTHighBack  = ((((6000 * (unsigned long)b_ratio )/(unsigned long)b_freq) + 50)  / 100);     // suction
  PLSCntrl.PTHighFront = ((((6000 * (unsigned long)f_ratio )/(unsigned long)f_freq) + 50)  / 100);
  PLSCntrl.PTLowBack   = ((((6000 * (unsigned long)(100 - b_ratio))/(unsigned long)b_freq))/ 100);
  PLSCntrl.PTLowFront  = ((((6000 * (unsigned long)(100 - f_ratio))/(unsigned long)f_freq))/ 100);

  return r_val;
}


// =============================================================================
void TogglePulseOutputs( void)
{
  // FRONT
  if ( PORTA & P_4 ){
    // output_high, suction phase
    HP4_Counter--;
    if ( HP4_Counter <= 0 ){
      // toggle output, set low-counter, set rest phase
      PORTA &= ~P_4;
      LP4_Counter = PLSCntrl.PTLowFront;
    }
  } else {
    // output_low, rest phase
    LP4_Counter--;
    if ( LP4_Counter <= 0 ){
      // toggle output, set high-counter, set suction phase
      PORTA |= P_4;
      HP4_Counter = PLSCntrl.PTHighFront;
    }
  }

  // BACK
  // When tracktion-arm is selected P3 is used for tracktion-arm control
  if ( EEOutputK2 != MILK_AUTOMAT_CONTROL ){
    if ( PORTA & P_5 ){
      // output_high
      HP5_Counter--;
      if ( HP5_Counter <= 0 ){
        // toggle output, set low-counter
        PORTA &= ~P_5;
        LP5_Counter = PLSCntrl.PTLowBack;
      }
    } else {
      LP5_Counter--;
      if ( LP5_Counter <= 0 ){
        // toggle output, set high-counter
        PORTA |= P_5;
        HP5_Counter = PLSCntrl.PTHighBack;
      }
    }
  }
}

// =============================================================================
// Called in Cleaning Phase
// =============================================================================
void CheckPulsation( void)
{

  switch ( EEI2Control & 0x03 ){
    case 2: // afhankelijk van de communicatie
            if ( EERowSize == 0 ){
              // SLAVE
              switch ( KeyExtPulseControl ) {
                case SET_PULSATION_MESSAGE:
                     if ( PlsState == PLS_IDLE || PlsState == PLS_PRE_IDLE ){
                     	 PlsState = PLS_SET_PULSATION;
                       if ( MilkMeterType == MMT_NIVEAU ){
                         PortP1On();
                       }
                     }
                  break;
                case STOP_PULSATION_MESSAGE:
                     if ( (PlsState < PLS_STOP_PULSATION) && (PlsState != PLS_IDLE)) PlsState = PLS_STOP_PULSATION;
                  break;
              }
              break;
            }
      // no break, indien RowSize != 0
    case 0: // afhankelijk van I2
            // indien master dan bericht naar 'slaves' ipv bekabeling
            // de 'Master' moet dus ook I2 in de gaten houden

            if ((EEPulsation & PLS_HAPPEL_SYSTEM) ||
            	   (!(((MilkMeterType == MMT_MEMOLAC) ||
            	       (MilkMeterType == MMT_VFLOW)   ||
            	       (MilkMeterType == MMT_VP4001)  ||
            	       (MilkMeterType == MMT_TAKE_OFF_SENSOR ))&&( CleanPhase == CLEAN_PHASE_0)))){
              // Happel maakt gebruik van een pulsatie systeem waarbij er geen water komt voordat
              // de pulsatie is begonnen. Het was zo dat de pulsatie niet begon voordat er water kwam.

              SetStartSwitchState();
              switch (StartSwitchState) {
                case SWITCH_CLOSED:
                case SWITCH_CLOSED_HOLD:
                     if ( PlsState == PLS_IDLE || PlsState == PLS_PRE_IDLE ){
                       PlsState = PLS_SET_PULSATION;
                       //  in export HandleCentralMessages worden deze berichten gestuurd
                       if ( EERowSize ) ExternMessages |=  SEND_SET_PULSATION_MESSAGES;
                     }
                  break;
                case SWITCH_OPEN:
                case SWITCH_OPEN_HOLD:
                     if ( (PlsState < PLS_STOP_PULSATION) && (PlsState != PLS_IDLE)){
                       PlsState = PLS_STOP_PULSATION;
                       //  in export HandleCentralMessages worden deze berichten gestuurd
                       if ( EERowSize ) ExternMessages |=  SEND_STOP_PULSATION_MESSAGES;
                     }
                  break;
              } // end switch (StartSwitchState)
            }
      break;
    case 1: // onafhankelijk van I2
            if ( MilkMeterType == MMT_MEMOLAC ){
              if ((( CleanPhase == CLEAN_PHASE_1)&& (ValveState == VALVE_ON))||(CleanPhase == CLEAN_PHASE_PRE_CLEAN)){
                if ( PlsState == PLS_IDLE || PlsState == PLS_PRE_IDLE )  PlsState = PLS_SET_PULSATION;
              }
            } else {
              if ((MilkMeterType == MMT_NIVEAU) || (MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)){
                if ( PlsState == PLS_IDLE || PlsState == PLS_PRE_IDLE ){
                	if ( MlkState == MLK_CLEANING ){
                	  PlsState = PLS_SET_PULSATION;
                  }
                }
              }
            }
      break;
  } // end switch( EEI2Control )
}


// =============================================================================
// deze functie wordt aangeroepen van uit een interrupt routine
// =============================================================================
void HndlPulsation( void )
{
  static unsigned short stop_time;

  switch( PlsState ){
    case PLS_IDLE:
      break;
    case PLS_INIT_PULSATION:
         SetPulsation();
         PlsState = PLS_WF_START_PULSATION;
      break;
    case PLS_WF_START_PULSATION:
         if ( CheckStartPulsation() ) PlsState = PLS_START_PULSATION;
      break;
    case PLS_START_PULSATION:
         StartPulsation();
         PlsState = PLS_STANDARD_PULSATION;
      break;
    case PLS_START_STIM_PULSATION:
         StartPulsation();
         PLSCntrl.StimStartTime = Cnt1sec;
         PlsState               = PLS_STIMULATION_PULSATION;
      break;
    case PLS_STANDARD_PULSATION:
    case PLS_STIMULATION_PULSATION:
    case PLS_TA_STIM_PULSATION:
         TogglePulseOutputs();
      break;
    case PLS_END_PULSATION:
         if ( EEPulsation & PLS_HAPPEL_SYSTEM){
           // HAPPEL: normale pulsatie
           SetPulsationTimes(STANDARD_PULSE_MODE);
         } else {
            PulsationToRest();
         }
         stop_time = Cnt10msec;
         PlsState  = PLS_PRE_IDLE;
      break;
    case PLS_PRE_IDLE:
         if ( EEPulsation & PLS_HAPPEL_SYSTEM)   TogglePulseOutputs();
         // wacht 10 seconden voor dat de zuigers  in de rustpositie gaan
         if ( TimeControl( Cnt10msec, stop_time + 1000 ) ){
           StopPulsation();
           PlsState = PLS_IDLE;
         }
      break;
    case PLS_STOP_PULSATION:
         StopPulsation();
         PlsState = PLS_IDLE;
    default:
      break;

  }
}


// =============================================================================
void PulseControl( void )
{
  unsigned char  perc, i, x, y;
  unsigned char *pp;
    signed char *prc;
    signed char *pfc;

  switch( PlsState ){
    case PLS_SET_PULSATION:
         SetPulsationTimes(STANDARD_PULSE_MODE);
         PlsState = PLS_INIT_PULSATION;
      break;
    case PLS_STANDARD_PULSATION:
         // kijk of er gestimuleerd moet worden
         if ( SoftKey & SK_STIM_TRCK_ARM_ENABLE ){
           if ( MlkDurationCur > ((p_CowMlkPlsRef[SideState] != NULL) ?
           	    (LONG_COW_GEN_NUMBER ?
           	    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime) :
           	    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmSrtTime : EEStimStartTime)) :
           	    EEStimStartTime ) &&
                MlkMode != MM_CLEANING){
             if ( PLSCntrl.ManualPulseToggle ){
               if ((MlkActFlow <= ((p_CowMlkPlsRef[SideState] != NULL) ?
               	  (LONG_COW_GEN_NUMBER ?
               	  (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold) :
               	  (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold)):
               	  EEStimTresHold ))  &&
                  !PLSCntrl.FirstTimeStimulation ){
                 PLSCntrl.FirstTimeStimulation = 1;
                 SetPulsationTimes(STIMULATION_PULSE_MODE);
                 PlsState = PLS_START_STIM_PULSATION;
               }
             }

             if ( MlkActFlow > ((p_CowMlkPlsRef[SideState] != NULL) ?
               	  (LONG_COW_GEN_NUMBER ?
               	  (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold) :
               	  (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold)) :
             	  EEStimTresHold ) ){
               PLSCntrl.FirstTimeStimulation = 1;
               if ( PlsState != PLS_STANDARD_PULSATION ) PlsState = PLS_STANDARD_PULSATION;
             }
           }
         }

         if ( !PLSCntrl.PlsMsgControlled && (p_CowMlkPlsRef[SideState] != NULL )){
           // pulsatie bericht ontvangen, pas tijden aan (alleen als status zelfde blijft)
           PLSCntrl.PlsMsgControlled = TRUE;
           if ( PlsState == PLS_STANDARD_PULSATION) SetPulsationTimes(STANDARD_PULSE_MODE);
         }

         //
         if ( (p_CowMlkPlsRef[SideState] != NULL) && (PlsState == PLS_STANDARD_PULSATION) &&
         	 ( LONG_COW_GEN_NUMBER ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed :
         	 	                       ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed ) ){
           // pulsatie eventueel aanpassen aan actuele melkflow
           if (LONG_COW_GEN_NUMBER ){
             perc = (unsigned char)(100 - ((((signed long)((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed - (signed long)MlkActFlow)*100L) /
                                             (signed long)((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed));
           } else {
             perc = (unsigned char)(100 - ((((signed long)((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed - (signed long)MlkActFlow)*100L) /
                                             (signed long)((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsAvgSpeed));
           }
           for( i = 0; i < 10; i++ ){
             if (LONG_COW_GEN_NUMBER ){
               pp = (&(((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsProp01))+i;
             } else {
               pp = (&(((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsProp01))+i;
             }
             if (*pp == 0 ) break;
             if ( perc >= *pp ){
               // pas suction fase aan
               prc = (LONG_COW_GEN_NUMBER ?
                     (&(((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatCorr01)) :
                     (&(((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsRatCorr01)))+i;
               pfc = (LONG_COW_GEN_NUMBER ?
                     (&(((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqCorr01)) :
                     (&(((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsFrqCorr01)))+i;
               if ( (PLSCntrl.PlsRatCorr != *prc) || (PLSCntrl.PlsFreqCorr != *pfc)) {
                 // bewaar even de oude waarde, moet eventueel terug gezet worden
                 x = PLSCntrl.PlsRatCorr;
                 y = PLSCntrl.PlsFreqCorr;
                 PLSCntrl.PlsRatCorr  = *prc;
                 PLSCntrl.PlsFreqCorr = *pfc;
                 // correctie overnemen als die ook werkelijk is toegepast
                 if ( !SetPulsationTimes(STANDARD_PULSE_MODE)){ PLSCntrl.PlsRatCorr = x; PLSCntrl.PlsFreqCorr = y; }
               }
               break;
             }
           }
         }
      break;
    case PLS_STIMULATION_PULSATION:
         // schakel over op normale pulsatie, indien de maximale stimulatie-tijd is verstreken
         // of de melkflow komt boven de treshhold, of stimulatie wordt handmatig onderbroken
         if (  TimeControl( Cnt1sec, PLSCntrl.StimStartTime +
              ((p_CowMlkPlsRef[SideState] != NULL) ?
                (LONG_COW_GEN_NUMBER ?
                  (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmMaxTime ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmMaxTime : EEStimMaxTime) :
                  (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmMaxTime ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmMaxTime : EEStimMaxTime))
                  	 : EEStimMaxTime )) ||
              (MlkActFlow > ((p_CowMlkPlsRef[SideState] != NULL) ?
                (LONG_COW_GEN_NUMBER ?
                  (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold) :
                  (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold ? ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStmTresHold : EEStimTresHold))
                  	: EEStimTresHold ))  ||
              ( !PLSCntrl.ManualPulseToggle) ){
           SetPulsationTimes(STANDARD_PULSE_MODE);
           StartPulsation();
           PlsState = PLS_STANDARD_PULSATION;
         }

         if ( !PLSCntrl.PlsMsgControlled && (p_CowMlkPlsRef[SideState] != NULL )){
           // pulsatie bericht ontvangen, pas tijden aan (alleen als status zelfde blijft)
           PLSCntrl.PlsMsgControlled = TRUE;
           if ( PlsState == PLS_STIMULATION_PULSATION) SetPulsationTimes(STIMULATION_PULSE_MODE);
         }
      break;
    case PLS_START_TA_STIM_PULSATION:
         SetPulsationTimes(PLS_TA_STIM_PULSATION);
         PlsState = PLS_TA_STIM_PULSATION;
      break;
  }
}
