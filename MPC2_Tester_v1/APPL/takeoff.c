// =============================================================================
// MODULE Take OFF
// =============================================================================

#include "takeoff.h"
#include "milk.h"
#include "milkmisc.h"
#include "takeoff.h"
#include "niveau.h"
#include "pulsation.h"
#include "conduct.h"
#include "../include/romstnet.h"
#include "../include/romeehc1.h"
#include "../lib/boolean.h"
#include "../lib/stdlib.h"
#include "../lib/time.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/per_func_test_vcp_flow.h"
#include "../lib/rti_int.h"
#include "../lib/eehc11.h"
#include "../lib/reg_flags.h"
#include "../lib/extport.h"
#include "../lib/io6811.h"
#include "../lib/iic.h"
#include "../key/keyint.h"
#include "../key/key.h"
#include "../init/start.h"
#include "../usx/fake_usx.h"
#include "../comm/flowctrl.h"


#pragma codeseg(EEPAGE)


unsigned char  TakeOffState, VacuumState, ClusterState, ExtOutputP1State, MilkAutomatState, UnicoState;


// =============================================================================
// Handle    Init Take Off
// =============================================================================
void InitTO(void)
{
  TakeOffState     = TO_START_CLEAN;
  ClusterState     = CLUSTER_IDLE;
  VacuumState      = VACUUM_IDLE;
  ExtOutputP1State = P1_IDLE;
  MilkAutomatState = MILK_AUTOMAT_IDLE;
  UnicoState       = UNICO_IDLE;
  LampControl( LAMP_OFF, 0);
}

// =============================================================================
// Handle  Output Ports
// =============================================================================
void PortP3On(void)
{
  PORTA |= P_5;
}

// =============================================================================
void PortP3Off(void)
{
  PORTA &= ~P_5;
}


// =============================================================================
void PortP2On(void)
{
  PORTA |= P_4;
}

// =============================================================================
void PortP2Off(void)
{
  PORTA &= ~P_4;
}

// =============================================================================
void PortP1On(void)
{
  PORTA |= P_3;
}


// =============================================================================
void PortP1Off(void)
{
  PORTA &= ~P_3;
}


// =============================================================================
// Handle    Attention Lamp
// =============================================================================
void LampControl( unsigned char command, unsigned short time )
{
  static BOOLEAN        lamp_flash = FALSE;
  static unsigned short FlashTime = 0;

  switch (command ){
    case LAMP_TOGGLE:
         if ( PORTD & P_5) PORTD &= ~P_5;
         else              PORTD |=  P_5;
      break;
    case LAMP_ON:
         PORTD |= P_5;
         lamp_flash = FALSE;
      break;
    case LAMP_FLASH:
         if ( TimeControl( Cnt10msec, FlashTime ) || (lamp_flash == FALSE)){
           if (PORTD & P_5)   PORTD &= ~P_5;
           else               PORTD |=  P_5;
           FlashTime  = Cnt10msec + (time/10);
           lamp_flash = TRUE;
         }
      break;
    default:
    case LAMP_OFF:
         PORTD &= ~P_5;
         lamp_flash = FALSE;
      break;
  } // end switch( command )

}

// =============================================================================
// Handle    Automatic Take Off   Procedure 1
//
// Used by Levelmeter
// =============================================================================
void HndlTakeOffProc1(void)
{
  static unsigned short StartTakeOffRelTime, StartTakeOffTime, StartP1OffTime, StartUnicoTime;
  static unsigned short MilkSweepStopTime, ClusterDownTime, TractionArmOnOffTime;
  static unsigned short PulsWidthMeasOld;
  static unsigned char  Init = 2;

  switch (TakeOffState) {
    case TO_IDLE :
    case TO_AUTO :
    case TO_MAN  :
    case TO_CLEANING:
      break;

    case TO_CLEANING_END:
         ClusterState = CLUSTER_LOW;
         if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
         else                                      VacuumState = VACUUM_ON;
         if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
         	 ExtOutputP1State = P1_OFF;
         } else {
         	 ExtOutputP1State = P1_CLEAN_END;
         }
         TakeOffState     = TO_IDLE;
         MilkAutomatState = MILK_AUTOMAT_OFF;
         UnicoState       = UNICO_IDLE;
      break;

    case TO_FIRST_START :
         ClusterState = CLUSTER_HIGH;
         VacuumState  = VACUUM_OFF;
         if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
         	 ExtOutputP1State = P1_OFF;
         } else {
         	 ExtOutputP1State = P1_ON;
         }
         TakeOffState     = TO_IDLE;
         PlsState         = PLS_STOP_PULSATION;
         MilkAutomatState = MILK_AUTOMAT_OFF;
         UnicoState       = UNICO_IDLE;
     break;

    case TO_INIT :
         ClusterState     = CLUSTER_LOW;
         VacuumState      = VACUUM_ON;
         if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
         	 ExtOutputP1State = P1_OFF;
         } else {
         	 ExtOutputP1State = P1_ON;
         }
         MilkAutomatState = MILK_AUTOMAT_OFF;
         UnicoState       = UNICO_IDLE;
      break;

    case TO_START_CLEAN:
         ClusterState     = CLUSTER_LOW;
         VacuumState      = VACUUM_ON;
         ExtOutputP1State = P1_OFF;
         MilkAutomatState = MILK_AUTOMAT_OFF;
         TakeOffState     = TO_CLEANING;
      break;

    case TO_START_UNICO:
         // ophalen melkstel
         UnicoState       = UNICO_DETACH;
    case TO_START :
         StartTakeOffTime = Cnt1sec;
         TakeOffState     = TO_IDLE;
         ClusterState     = CLUSTER_LOW_HIGH;
         VacuumState      = VACUUM_ON_OFF;
//         if( MilkMeterType  == MMT_VFLOW) ExtOutputP1State = P1_OFF;
//         else                             ExtOutputP1State = P1_ON_OFF;
         ExtOutputP1State = P1_ON_OFF;
         // uitgangen moeten niet worden aangestuurd indien er geen melkmeter is aangesloten
         if ( PlsState != PLS_IDLE) PlsState = PLS_END_PULSATION;
         MilkAutomatState = MILK_AUTOMAT_OFF;
         // alleen aan K3 komen indien het een carroussel stal is
         if ( ParlourType == PT_CARROUSEL){
           if ( (GetTransferAtt() & EEOutputBlockBits )== 0 ){
             ExtPortK3State = EPK3S_OFF;            // deactivate transfer output
           }
           if ( GetTransferAtt() & TA_MINYIELD ) ExtPortK4State = EPK4S_ON;        // signal low milk_yield
         }
      break;

    case TO_RELEASE_AUTO:
         // onderhangen melkstel; automatisch ophalen
         StartTakeOffRelTime = Cnt1sec;
         TakeOffState        = TO_AUTO;
         ClusterState        = CLUSTER_HIGH_LOW;
         VacuumState         = VACUUM_OFF_ON;
         ExtOutputP1State    = P1_ON;
         if( MilkMeterType  < MMT_NO_MILK_METER) PlsState = PLS_SET_PULSATION;
         MilkAutomatState    = MILK_AUTOMAT_OFF;
         UnicoState          = UNICO_ATTACH_AUTO;
         // alleen aan K3 komen indien het een carroussel stal is
         if ( ParlourType == PT_CARROUSEL){
           ExtPortK3State      = EPK3S_ON;          // activate transfer output
           ExtPortK4State      = EPK4S_OFF;
         }
      break;

    case TO_RELEASE_MAN:
         // onderhangen melkstel; handmatig ophalen
         StartTakeOffRelTime = Cnt1sec;
         TakeOffState        = TO_MAN;
         ClusterState        = CLUSTER_HIGH_LOW;
         VacuumState         = VACUUM_OFF_ON;
         ExtOutputP1State    = P1_ON;
         if( MilkMeterType  < MMT_NO_MILK_METER) PlsState = PLS_SET_PULSATION;
         MilkAutomatState    = MILK_AUTOMAT_OFF;
         UnicoState          = UNICO_ATTACH_MAN;

         // alleen aan K3 komen indien het een carroussel stal is
         if ( ParlourType == PT_CARROUSEL){
           ExtPortK3State      = EPK3S_ON;          // activate transfer output
           ExtPortK4State      = EPK4S_OFF;
         }
         CowMlkYldManDetach++;                      // manual release
     break;
    default :
     break;
  } // end switch (TakeOffState)


  // Relays Output K1 state
  // How to use output K1 (same variable as for output K2 kan be used
  switch (EEOutputK2) {
    case UNICO_CONTROL:
         switch (UnicoState) {
           case UNICO_IDLE:
             break;


           case UNICO_OFF:
           case UNICO_ON:
           case UNICO_DETACH:
                ExtPortCopy   |= K1;
                StartUnicoTime = Cnt10msec;
                UnicoState     = UNICO_ON_OFF;
             break;

           case UNICO_ON_OFF:
                if ( TimeControl( Cnt10msec, StartUnicoTime + 50 )){
                  ExtPortCopy &= ~(K1+K2);
                  UnicoState   = UNICO_IDLE;
                }
             break;

           case UNICO_ATTACH_AUTO:
                ExtPortCopy   |= K2;
                StartUnicoTime = Cnt10msec;
                UnicoState     = UNICO_ON_OFF;
             break;

           case UNICO_ATTACH_MAN:
                ExtPortCopy   |= K1;
                StartUnicoTime = Cnt10msec;
                UnicoState     = UNICO_ON_OFF_MAN;
             break;

           case UNICO_ON_OFF_MAN:
                if ( TimeControl( Cnt10msec, StartUnicoTime + 400 )){
                  ExtPortCopy &= ~(K1+K2);
                  UnicoState   = UNICO_IDLE;
                }
             break;
         } // end switch(UnicoState)
      break;
    case MILK_AUTOMAT_CONTROL:
         switch (MilkAutomatState) {
           case MILK_AUTOMAT_ON:
           case MILK_AUTOMAT_IDLE:
             break;

           case MILK_AUTOMAT_START:
                PORTA               |= P_5;
                if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
                  ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING :
                  ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING )){
                  TractionArmOnOffTime = Cnt1sec + (LONG_COW_GEN_NUMBER ?
                          ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOnTime :
                          ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOnTime);
                } else {
                  TractionArmOnOffTime = Cnt1sec +EETrcArmOnTime;
                }
                MilkAutomatState     = MILK_AUTOMAT_ON_OFF;
                PlsState             = PLS_START_TA_STIM_PULSATION;
             break;

           case MILK_AUTOMAT_ON_OFF:
                if ( TimeControl( Cnt1sec, TractionArmOnOffTime)){
                  PORTA               &= ~P_5;
                  MilkAutomatState     = MILK_AUTOMAT_OFF_ON;
                  if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
                    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ) :
                    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ))){
                    TractionArmOnOffTime = Cnt1sec + (LONG_COW_GEN_NUMBER ?
                      ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOffTime :
                      ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOffTime);
                  } else {
                    TractionArmOnOffTime = Cnt1sec +EETrcArmOffTime;
                  }
                }
             break;
           case MILK_AUTOMAT_OFF_ON:
                if ( TimeControl( Cnt1sec, TractionArmOnOffTime)){
                  PORTA               |= P_5;
                  MilkAutomatState     = MILK_AUTOMAT_ON_OFF;
                  if ( (p_CowMlkPlsRef[SideState] != NULL) && (LONG_COW_GEN_NUMBER ?
                    (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ) :
                    (((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ))){
                    TractionArmOnOffTime = Cnt1sec + (LONG_COW_GEN_NUMBER ?
                      ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOnTime :
                      ((   CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripOnTime);
                  } else {
                    TractionArmOnOffTime = Cnt1sec +EETrcArmOnTime;
                  }
                }
             break;

           case MILK_AUTOMAT_OFF:
                PORTA           &= ~P_5;
                MilkAutomatState = MILK_AUTOMAT_IDLE;
             break;

           default:
             break;
         } // end switch (MilkAutomatState)
      break;

    // Retract cluster control
    default:  break;
  } // end switch( EEOutputK2)

  if ( EEOutputK2 != UNICO_CONTROL ){
    // Cluster handling
    switch (ClusterState) {
      case CLUSTER_IDLE:
        break;

      case CLUSTER_HIGH:
           // K1 moet worden aangesproken via PIC-A, dus via IIC
           ExtPortCopy |= K1;
           if ( EEClusterDown ) {
             // cluster laten zakken na bepaalde tijd, inclusief milk-sweep tijd
             ClusterDownTime = Cnt1sec + EEClusterDown + EEMilkSweepDlyTime + EEMilkSweepActTime/10;
             ClusterState    = CLUSTER_DOWN_LOW;
           } else  ClusterState = CLUSTER_IDLE;
        break;

      case CLUSTER_LOW:
           ExtPortCopy &= ~K1;
           ClusterState = CLUSTER_IDLE;
        break;

      case CLUSTER_HIGH_LOW:
           if (Cnt1sec >= StartTakeOffRelTime) {
             ClusterState = CLUSTER_LOW;
           }
        break;

      case CLUSTER_LOW_HIGH:
           if (Cnt1sec >= (StartTakeOffTime + EEClusterDly)) {
             ClusterState = CLUSTER_HIGH;
           }
        break;
      case CLUSTER_DOWN_LOW:
           if ( TimeControl( Cnt1sec, ClusterDownTime)){
             ClusterState = CLUSTER_LOW;
           }
        break;
      case CLUSTER_DOWN_HIGH:
           ExtPortCopy |= K1;
           VacuumState  = VACUUM_OFF;
           ClusterState = CLUSTER_IDLE;
      default :
      case CLUSTER_SET_DOWN_HIGH:
           ClusterDownTime = Cnt1sec + EEClusterDown;
           ClusterState = CLUSTER_DELAY_DOWN_HIGH;
        break;
      case CLUSTER_DELAY_DOWN_HIGH:
           if ( TimeControl( Cnt1sec, ClusterDownTime)){
             ClusterState = CLUSTER_DOWN_HIGH;
           }
        break;
    } // end  switch (ClusterState)

    // Vacuum handling
    switch (VacuumState) {
      case VACUUM_IDLE:
           if ( (ExtPortCopy &  K2 ) && (EEOutputK2 != VACUUM_CONTROL_INVERS)){
             if (  ((MlkState == MLK_CLEANING_END) || (MlkState == MLK_CLEANING_END_I2))){
               if (  !((CleanPhase == CLEAN_PHASE_I2_CONTROLLED ) ||
                        (CleanPhase == CLEAN_PHASE_I2_CONTROLLED_0) ||
                        (CleanPhase == CLEAN_PHASE_I2_CONTROLLED_1)) ){
                 ExtPortCopy &=  ~K2;           // deactivate
               }
             }
           }
        break;

      case VACUUM_OFF:                       // led-on
           // K2 moet worden aangesproken via PIC-A, dus via IIC
           if ( EEOutputK2 == VACUUM_CONTROL_INVERS) ExtPortCopy &= ~K2;
           else                                      ExtPortCopy |=  K2;
           VacuumState  = VACUUM_IDLE;
        break;

      case VACUUM_ON:                        // led-off
           if (( Init) && (EEOutputK2 == VACUUM_CONTROL_INVERS)){
             ExtPortCopy &=  ~K2;            // activate
             Init -= 1;
           } else {
             if ( EEOutputK2 == VACUUM_CONTROL_INVERS) ExtPortCopy |=  K2;
             else                                      ExtPortCopy &= ~K2;
           }
           VacuumState  = VACUUM_IDLE;
        break;

      case VACUUM_OFF_ON:
           if ( TimeControl( (unsigned short)Cnt1sec, (StartTakeOffRelTime + EEVacuumDly))) {
             VacuumState  = VACUUM_ON;
           }
        break;

      case VACUUM_ON_OFF:
           if ( TimeControl( (unsigned short)Cnt1sec, StartTakeOffTime)){
             // schakel vacuum af
             if ( EEOutputK2 == VACUUM_CONTROL_INVERS) ExtPortCopy &= ~K2;
             else                                      ExtPortCopy |=  K2;
             // voer eventueel een 'MilkSweep' uit, laatste restje melk
             if ( EEMilkSweepDlyTime && EEMilkSweepActTime ) VacuumState  = VACUUM_MLK_SWEEP_DLY_TIME;
             else                                            VacuumState  = VACUUM_IDLE;
           }
        break;
      case VACUUM_MLK_SWEEP_DLY_TIME:
           if ( TimeControl( (unsigned short)Cnt1sec, StartTakeOffTime + (unsigned short)EEMilkSweepDlyTime)) {
             if ( EEOutputK2 == VACUUM_CONTROL_INVERS) ExtPortCopy |=  K2;
             else                                      ExtPortCopy &= ~K2;
             MilkSweepStopTime = Cnt10msec + (((unsigned short)EEMilkSweepActTime)*10);
             VacuumState       = VACUUM_MLK_SWEEP_ACT_TIME;
           }
        break;
      case VACUUM_MLK_SWEEP_ACT_TIME:
           if ( TimeControl( Cnt10msec, MilkSweepStopTime )){
             VacuumState  = VACUUM_OFF;
           }
        break;
      default :
        break;
    } // end switch (VacuumState)
  }

  switch (MilkMeterType) {
    case MMT_NIVEAU:
      // Output P1 control
      switch (ExtOutputP1State) {
        case P1_IDLE:
          break;

        case P1_CLEAN_END:
             PortP1Off();
             ExtOutputP1State = P1_IDLE;
          break;

        case P1_ON :
             // inverteren output P1
             if (EEOutputP1Inv == TRUE) PortP1Off();
             else                       PortP1On();
          break;

        case P1_OFF:
             // inverteren output P1
             if (EEOutputP1Inv == TRUE) PortP1On();
             else                       PortP1Off();
          break;

        // Start ledigen melkglas
        case P1_ON_OFF:
             if (((GetCowGls() == 0) && (EmptyGlassAuto == TRUE)) || (EEAddressCopy == 0 && MlkSepStandAlone == 0)) {
               if (Cnt1sec >= (StartTakeOffTime + EEOutputP1Dly)) {
                 PulsWidthMeasOld = PulsWidthMeas;
                 StartP1OffTime = 0;
                 // inverteren output P1
                 if (EEOutputP1Inv == TRUE) PortP1On();
                 else                       PortP1Off();
                 ExtOutputP1State = P1_OFF_ON;
               }
             }
          break;


        case P1_OFF_ON:
             // Test of vlotter nog verder zakt
             // voor dat test begint, moet vlotter vrijwel onderin liggen
             if ((PulsWidthAve < (PulsWidthMeasOld+10)) && (PulsWidthAve > (PulsWidthMeasOld-10))
                 &&(PulsWidthAve < (EECal2+200)) && (PulsWidthAve > (EECal2-200)) ) {
               //Start Timer
               if (StartP1OffTime == 0) {
                 StartP1OffTime = Cnt1sec;
               } else {
                 if (Cnt1sec > (StartP1OffTime + EEOutputP1Time)) {
                   //Stop ledigen melkglas
                   ExtOutputP1State = P1_ON;
                 }
               }
             } else {
               StartP1OffTime = 0;
             }
             PulsWidthMeasOld = PulsWidthAve;
          break;

        case P1_START_MAN:
             StartP1OffTime = Cnt1sec;
             // inverteren output P1
             if (EEOutputP1Inv == TRUE) PortP1On();
             else                       PortP1Off();
             ExtOutputP1State = P1_OFF_ON;
          break;

        default :
          break;
      } // end switch (ExtOutputP1State)

      break;
    case MMT_VFLOW:
    case MMT_VP4001:	
      switch (ExtOutputP1State) {
        case P1_ON_OFF:
             if (Cnt1sec >= (StartTakeOffTime + EEOutputP1Dly)) {
               StartP1OffTime = 0;
               // inverteren output P1
               if (EEOutputP1Inv == TRUE) PortP1On();
               else                       PortP1Off();
//               ExtOutputP1State = P1_OFF_ON;
             }
          break;
        case P1_ON :
             // inverteren output P1
             if (EEOutputP1Inv == TRUE) PortP1Off();
             else                       PortP1On();
          break;

        case P1_OFF:
             // inverteren output P1
             if (EEOutputP1Inv == TRUE) PortP1On();
             else                       PortP1Off();
          break;
      }
      break;
    default:                                          // switch (MilkMeterType)
      break;
  } // end switch(MilkMeterType)
}


// =============================================================================
// Handle    Attention Lamp
//
// Used by Levelmeter/memolac/SURGE
// =============================================================================
void HndlAttLamp1(void)
{
    signed char  idx;
  unsigned short AttentionLamp;


  // Check Attention Lamp
  //********************

  // Stand-alone mode
  if (EEAddressCopy != 0) {

    if (KeyMode == KM_MILKING) {
      idx = GetSideIndex( SideState );
      AttentionLamp = GetCowAtt(idx);

      if  ( AttentionLamp & CA_SEP )                         LampControl(LAMP_ON, 0);
      else {
        idx = GetSideIndex( SideState );
        if (  MlkState == MLK_INIT    || MlkState == MLK_IDLE ||
              MlkState == MLK_NEUTRAL || MlkState == MLK_WAIT_FOR_START  /*||*/
            /*((MlkState == MLK_DONE)   && (MeasState[idx] != MEAS_MEAS ))*/){

          // TOT EN MET NEUTRALE MELKFASE
          if (AttentionLamp != 0) {
            if      ( AttentionLamp & CA_NOCOW  )            LampControl(LAMP_FLASH, 500);
            else if ( AttentionLamp & CA_COLOST )            LampControl(LAMP_ON, 0);
            else if ( AttentionLamp & CA_DRY    )            LampControl(LAMP_ON, 0);
            // lamp knipperd indien geleidbaarheid-led knipperd
            else if ( DispBlinkLeds & IIC_CONDUCT_ATTENTION) LampControl(LAMP_FLASH, 500);
            else if ( ConDataAttentions & 0x0f )             LampControl(LAMP_ON, 0);
            else                                             LampControl(LAMP_OFF, 0);
          } else                                             LampControl(LAMP_OFF, 0);
        } else {
           if ( ConDataAttentions & 0x0f )                   LampControl(LAMP_ON, 0);
           else {
            // na het melken
            // indien geen melkmeter: geen attentie
            if ( (MilkMeterType < MMT_NO_MILK_METER )                       &&
                ((MlkState == MLK_SEND_WAIT && MeasState[idx] == MEAS_MEAS)  ||
                 (MlkState == MLK_DONE      && MeasState[idx] == MEAS_DONE)) ){
              AttentionLamp = GetCowGls();
              // Milkyield less then expected
              if ( AttentionLamp & CG_MINYIELD)              LampControl(LAMP_FLASH, 250);
              else                                           LampControl(LAMP_OFF, 0);
            } else                                           LampControl(LAMP_OFF, 0);
          }
        }
      }
    } else {
      if (KeyMode == KM_DHIA) {
        // Test of melkgift gedurende bepaalde tijd constant is
        if ( GetDhiaAtt(FALSE) )                             LampControl(LAMP_ON,  0);
        else                                                 LampControl(LAMP_OFF, 0);
      } else {
        // in test mode van de lamp blijven
        if (KeyMode != KM_P_TEST)                            LampControl(LAMP_OFF, 0);
      }
    }
  } else                                                     LampControl(LAMP_OFF, 0);
}

// =============================================================================
// Handle    Automatic Take Off
// =============================================================================
void HndlTakeOff(void)
{
  switch (MilkMeterType%100) {
    case MMT_NIVEAU :
    case MMT_MEMOLAC:
    case MMT_SURGE:
    case MMT_VFLOW:
    case MMT_VP4001:
    case MMT_TAKE_OFF_SENSOR:
         HndlTakeOffProc1();
         HndlAttLamp1();
      break;
    default :
      break;
  }
}
