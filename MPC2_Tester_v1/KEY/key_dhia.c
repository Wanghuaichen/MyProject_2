#include "../key/key.h"
#include "../key/keyint.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/intr6811.h"
#include "../lib/convert.h"
#include "../lib/text.h"
#include "../lib/extport.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/takeoff.h"


void HandleKeyDhia( void )
{
  KeyValue = KeyA + KeyState;        

  switch (KeyValue) {
    case KEY_WASH:
      switch (MilkMeterType) {
        case MMT_NIVEAU:
        case MMT_MEMOLAC:
        case MMT_SURGE:
        case MMT_TAKE_OFF_SENSOR:
        default:
          KeyMode        = KM_CLEANING;
          MlkState       = MLK_CLEANING;
          ScreenNumber   = SCREEN_RESET;
          ul_CleanStartTime = Cnt1sec;
        break;
      }
    break;

    // Toggle P1 output Milkglass,   Milkmeter Valve
    case KEY_MANUAL_REL_CLUSTER:
    case KEY_ACT_MILK_VALVE :
      switch (MilkMeterType) {
        case MMT_NIVEAU:
             // P1 output High 
             if (PORTA & P_3)  {
               if (EEOutputP1Inv == FALSE) ExtOutputP1State = P1_OFF;
               else                        ExtOutputP1State = P1_ON;
             } else {
               if (EEOutputP1Inv == FALSE) ExtOutputP1State = P1_ON;
               else                        ExtOutputP1State = P1_OFF;
             }
          break;
        case MMT_MEMOLAC:
             ValveState = VALVE_START;
          break;
      } // end switch ( MilkMeterType) 
      break;

    case KEY_CLUSTER_REMOVAL:
         // Toggle Cluster output 
         if ( ExtPortCopy & K1 ) ClusterState = CLUSTER_LOW;
         else                    ClusterState = CLUSTER_HIGH;
      break;

    case KEY_AUTO_RELEASE:
         // Toggle Vacuum output
         if ( EEOutputK2 == VACUUM_CONTROL_INVERS ){ 
           if ( ExtPortCopy & K2 ) VacuumState  = VACUUM_OFF;
           else                    VacuumState  = VACUUM_ON;
         } else {
           if ( ExtPortCopy & K2 ) VacuumState  = VACUUM_ON;
           else                    VacuumState  = VACUUM_OFF;
         }
      break;
    case KEY_MILK_INFO:
         // toggle correctiefactor screen 
         if (ScreenNumber == SCREEN_CORFAC) ScreenNumber = SCREEN_RESET;
         else                               ScreenNumber = SCREEN_CORFAC;
      break;

    case KEY_CLEAR:
         // input mode
         if (ScreenNumber == SCREEN_CORFAC) {
           if (MilkMeterType == MMT_NIVEAU ) {
             //geef correctie waarde zonder + en - teken weer 
             DispScreenCorFacLvl();
           }
           KeyModeOld = KeyMode;
           KeyMode    = KM_INPUT;
         }
      break;
    case KEY_RESET:
         if (MilkMeterType == MMT_NIVEAU ) {
           // reset dhialamp, continue measuring 
           GetDhiaAtt(TRUE);
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

    case KEY_SHIFT:
         KeyState     = KS_CONTINUE;
         KeyShiftTime = Cnt10msec;
      break;
  }
}