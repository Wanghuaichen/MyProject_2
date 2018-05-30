#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/key_robot.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h"
#include "../lib/schedule.h"
#include "../lib/eehc11.h"
#include "../lib/taskdef.h"
#include "../appl/milk.h"
#include "../appl/pulsation.h"

// ===========================================================================e==
// Handle Robot Status
// =============================================================================
void CheckForRobotMsg(void)
{
  // Check binnen gekomen robot berichten 
  if (PerRbtMlkRef.PerMatMlkFailed == TRUE) {
    RobotState = RBT_MLK_FAIL;
    PerRbtMlkRef.PerMatMlkFailed = FALSE;
  } else {
    if (PerRbtMlkRef.PerMatMlkEnd == TRUE){
      RobotState = RBT_MLK_END;
      PerRbtMlkRef.PerMatMlkEnd = FALSE;
    } else {
      if (PerRbtMlkRef.PerMatMlkStart == TRUE){
        RobotState = RBT_MLK_START;
        PerRbtMlkRef.PerMatMlkStart = FALSE;
        ScreenNumber = 0;
        ScreenNumberOld == ScreenNumber;
        ScreenTime -= EEDispTime;
        KeyMode = KM_RBT_MILKING;
      }
    }
  }
}

// ===========================================================================e==
// HandleKeyRobot
// =============================================================================
void HandleKeyRobot( void )
{
  switch (RobotState) {
    case RBT_MLK_START:
         if (MlkState == MLK_START_CLEANING  ||
             MlkState == MLK_CLEANING        ||
             MlkState == MLK_CLEANING_END) {

           TaskStop(MILK_CONTROL_TASK);
           InitMlk();
           InitRti();
           TaskStart(MILK_CONTROL_TASK);
           RestartNetwork();
         }
         KeyMode    = KM_RBT_MILKING;
         MlkState   = MLK_INIT;
         PlsState   = PLS_SET_PULSATION;
         RobotState = RBT_MLK_IDLE;
       break;
    case RBT_MLK_FAIL:
         MlkState   = MLK_DONE;
         PlsState   = PLS_END_PULSATION;
         RobotState = RBT_MLK_IDLE;
      break;

    case RBT_MLK_END:
         MlkState   = MLK_SEND;
         PlsState   = PLS_END_PULSATION;
         RobotState = RBT_MLK_IDLE;
      break;

    case RBT_MLK_CLEANING:
         MlkState   = MLK_START_CLEANING;
         PlsState   = PLS_STOP_PULSATION;
         RobotState = RBT_MLK_IDLE;
      break;

    default:
      break;
  } // end switch (RobotState) 

  if ( KeyA != KeyOld ){
    KeyOld = KeyA;
    switch (KeyA) {
      case KEY_WASH:
           switch (MilkMeterType) {
             case MMT_NIVEAU:
             case MMT_MEMOLAC:
             case MMT_SURGE:
             default:
                  KeyMode           = KM_CLEANING;
                  MlkState          = MLK_CLEANING;
                  ScreenNumber      = SCREEN_RESET;
                  ul_CleanStartTime = Cnt1sec;
               break;
           }
        break;

      case KEY_MANUAL_REL_CLUSTER:
      case KEY_CLUSTER_REMOVAL:
      case KEY_AUTO_RELEASE:
        break;
      case KEY_CLEAR:
           if (KeyHold == TRUE) {
             // input mode  
             KeyModeOld = KeyMode;
             KeyMode = KM_INPUT;
           }
        break;
      case KEY_DISEASE_INFO:
           if (ScreenMode != SM_DISEASE_MILK_INFO ){
             ScreenMode      = SM_DISEASE_MILK_INFO;
             ScreenNumber    = SCREEN_DISEASE_0;
             ScreenNumberOld = !ScreenNumber;
           } else {
             ScreenNumber++;
             if (ScreenNumber == SCREEN_DISEASE_MAX) {
               ScreenNumber = SCREEN_DISEASE_0;
             }
           }
        break;
      case KEY_COW_INFO:
           if ( ScreenMode != SM_COW_INFO ){
             ScreenMode      = SM_COW_INFO;
             ScreenNumber    = SCREEN_COW_INFO_0;
             ScreenNumberOld = !ScreenNumber;
           } else {
             ScreenNumber++;
             if (ScreenNumber == SCREEN_COW_INFO_MAX) {
               ScreenNumber = SCREEN_COW_INFO_0;
             }
           }
        break;
      case KEY_MILK_INFO:
           if ( ScreenMode != SM_MILK_INFO ){
             ScreenMode      = SM_MILK_INFO;
             ScreenNumber    = SCREEN_MILK_INFO_0;
             ScreenNumberOld = !ScreenNumber;
           } else {
             ScreenNumber++;
             if (ScreenNumber == SCREEN_MILK_INFO_MAX) {
               ScreenNumber = SCREEN_MILK_INFO_0;
             }
           }
        break;
      case KEY_FEED_INFO:
           if ( ScreenMode != SM_FEED_INFO ){
             ScreenMode      = SM_FEED_INFO;
             ScreenNumber    = SCREEN_FEED_INFO_0;
             ScreenNumberOld = !ScreenNumber;
           } else {
             ScreenNumber++;
             if (ScreenNumber == SCREEN_FEED_INFO_MAX) {
               ScreenNumber = SCREEN_FEED_INFO_0;
             }
           }
        break;
      case KEY_CALENDAR_INFO:
           if ( ScreenMode != SM_CALENDAR_INFO ){
             ScreenMode      = SM_CALENDAR_INFO;
             ScreenNumber    = SCREEN_CALENDAR_INFO_0;
             ScreenNumberOld = !ScreenNumber;
           } else {
             ScreenNumber++;
             if (ScreenNumber == SCREEN_CALENDAR_INFO_MAX) {
               ScreenNumber = SCREEN_CALENDAR_INFO_0;
             }
           }
     default:
        break;
    } // end switch( KM_RBT_MILKING ) 
  }
}