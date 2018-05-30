#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/disp.h"
#include "../lib/io6811.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/text.h"
#include "../lib/extport.h"
#include "../lib/reg_flags.h"
#include "../appl/milk.h"
#include "../appl/takeoff.h"

void HandleKeyCalibration( void )
{
  switch (KeyA) {
    case KEY_WASH:
         if (KeyHold == FALSE) {
           switch (MilkMeterType) {
             case MMT_NIVEAU:
                  KeyMode        = KM_CLEANING;
                  MlkState       = MLK_CLEANING;
                  ScreenNumber   = SCREEN_RESET;
                  ul_CleanStartTime = Cnt1sec;
               break;
           }
         }
      break;

    case KEY_ACT_MILK_VALVE :
         // Toggle P1 output  milkglass 
         if (PORTA & P_3)  {
           if (EEOutputP1Inv == FALSE) ExtOutputP1State = P1_OFF;
           else                        ExtOutputP1State = P1_ON;
         } else {
           if (EEOutputP1Inv == FALSE) ExtOutputP1State = P1_ON;
           else                        ExtOutputP1State = P1_OFF;
         }
      break;

    case KEY_CLUSTER_REMOVAL:
         // Toggle Cluster output
         if ( ExtPortCopy & K1 ) ClusterState = CLUSTER_LOW;
         else                    ClusterState = CLUSTER_HIGH;
      break;

    case KEY_AUTO_RELEASE:
         // Toggle Vacuum output
         if ( ExtPortCopy & K2 ) VacuumState  = VACUUM_ON;
         else                    VacuumState  = VACUUM_OFF;
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
    case KEY_K4_GATE :  if(!KeyHold) ExtPortK4State = EPK4S_TOGGLE;  break;
    case KEY_K5_GATE :  if(!KeyHold) ExtPortK5State = EPK5S_TOGGLE;  break;
  }
}