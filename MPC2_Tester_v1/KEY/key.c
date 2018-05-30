
// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// =============================================================================

// include files  --------------------------------------------------------------


#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/key_milk.h"
#include "../key/key_cleaning.h"
#include "../key/key_service.h"
#include "../key/key_robot.h"
#include "../key/key_input.h"
#include "../key/key_ptest.h"
#include "../key/key_dhia.h"
#include "../key/key_cal.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../lib/eehc11.h"
#include "../include/romeehc1.h"
#include "../lib/rti_int.h"
#include "../lib/taskdef.h"
#include "../lib/mem.h"
#include "../lib/iic.h"
#include "../lib/convert.h"
#include "../lib/eeprom.h"
#include "../lib/extport.h"
#include "../lib/typedef.h"
#include "../lib/intr6811.h"
#include "../lib/stdlib.h"
#include "../lib/text.h"
#include "../lib/schedule.h"
#include "../lib/watchdog.h"
#include "../comm/flowctrl.h"
#include "../include/romver.h"
#include "../appl/temp.h"
#include "../appl/pulsation.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/takeoff.h"
#include "../motor/motor.h"

#include "input.h"
#include "disp.h"


#pragma codeseg(EEPAGE)

extern char   Resources[10];
BOOLEAN       ModeChange;
BOOLEAN       SendCowInfo;

unsigned char KeyA;
unsigned char EditItem;
unsigned char WaitForI2High;

unsigned char  KeyMode;
unsigned char  KeyModeOld;
unsigned char  KeyOld;
unsigned char  KeyState;
unsigned char  KeyValue;
unsigned char  KeyHold;
unsigned short KeyShiftTime;
unsigned short KeyHoldTime;

unsigned char FieldNr, FieldNrMax;
ITEM_INFO   **DefTable;

unsigned char  pt_result;
unsigned short error;
unsigned short ExternMessages;
unsigned char  KeyExtSwitch;
unsigned char  nr_of_cal_feed_portions;

// =============================================================================
// GetManualorAutoRelease; called from handle external key
// =============================================================================
unsigned char GetManualorAutoRelease(void)
{
  signed char  idx;

  idx = GetSideIndex(SideState);

  if ( p_MlkRef != NULL ){
    if ( LONG_COW_GEN_NUMBER ){
      if ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 1) return KEY_MANUAL_REL_CLUSTER;
    } else {
      if ( ((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease == 1) return KEY_MANUAL_REL_CLUSTER;
    }
  }
  return KEY_AUTO_RELEASE;
}


// =============================================================================
// Initieer Key function
// =============================================================================
void InitKey(void)
{
#if ( _EMC_TEST_ )
  SetFactorySettings();
  SCCR2 = 0x08;          // transmit enable
  KeyState     = KS_NORMAL;
  ScreenMode   = SM_EMC_TEST;
  KeyMode      = KM_EMC_TEST;
  ScreenNumber = SCREEN_MILK_0;
  WriteEEpromByte(40, &EEMilkMeterType);
#else
  KeyState     = KS_NORMAL;
  KeyMode      = KM_ADDRESS;
  ScreenNumber = SCREEN_MILK_0;
  ScreenMode   = SM_SERVICE;
  ScreenEEPtr  = (ITEM_INFO *)&AddressEE;
#endif
  error = 0;
}


#if ( _EMC_TEST_ )
// =============================================================================
void Send( unsigned char result)
{
  unsigned char x;

  while( !(x = ( SCSR & 0x80 )) );
  // disable receiver
  SCCR2 &= ~0x24;
  SCDR = (unsigned char)(result);

  SCCR2 |= 0x04;
}
#endif

// =============================================================================
// lees externe Key in, kan ook via communicatie binnen komen
// =============================================================================
unsigned char GetKeyExtSwitch( void )
{
  unsigned char r_val;

  // tot versie 20 kan KeyExtSwitch veranderen tijdens taak-switch
  disable_interrupt();
  r_val = KeyExtSwitch;
  KeyExtSwitch = 0;
  enable_interrupt();
  return r_val;
}

// =============================================================================
// Lees toets in
// =============================================================================
unsigned char ReadKey(void)
{
  unsigned char KeyA1[3];

#if ( _EMC_TEST_ )              // tbv EMC testen
  static unsigned short key_time;
  unsigned char         rval = KEY_REL;

  if (KeyMode == KM_CLEANING )KeyMode = KM_EMC_TEST;

  if ( HandleReadIic( KB_ADDRESS, IIC_GET_KEY, 2, KeyA1 ) ){
    Send(0x01);
    rval = KeyA = KEY_REL;
  } else {
    Send(0x22);
    if ( KeyA1[0] == IIC_GET_KEY ){
      if ( KeyA1[1] >= 0x30 ) rval = KeyA = KeyA1[1];
      if ( KeyA >= 0x40 ) rval = (KeyA -= 0x3f);
    }
  }

  if ( rval == KEY_REL  ){
    if ( ((Cnt1sec % 2) == 0) && key_time != Cnt1sec) {
      key_time = Cnt1sec;
      rval = KeyA = KeyA1[1] = KEY_ENTER;
    }
  } else {
    if ( rval == KEY_RESET ) WriteEEpromByte(0, &EEAddress);
  }
  return rval;
#else
  // ***************************************************************************
  // normale inlees routine
  // ***************************************************************************
  if ( !HandleReadIic( KB_ADDRESS, IIC_GET_KEY, 2, KeyA1 ) ) {
    if ( KeyA1[0] == IIC_GET_KEY ) {
      if ( KeyA1[1] >= 0x30  ) {
        // vanaf versie 20 zijn de return waardes verschoven, bit6 wordt gebruikt
        // om een extra 'shift' key aan te geven
        if ( KeyA1[1] >= 0x40 ){
          // *******************************************************************
          // test expliciet op KEY_ERROR, deze kan continue komen tgv fout in PIC
          if ( KeyA1[1] == KEY_ERROR  ) return (KeyA = KEY_REL);
          else                          return (KeyA = KeyA1[1] - 0x3f);
        } else                          return( KeyA = KeyA1[1] );
        // *********************************************************************
      } else { KeyA = 0; error++; }
    } else { KeyA = 0; error++; }
  } else { KeyA = 0; error++; }
  return 0;

#endif
}

// =============================================================================
// Set default waarden
// =============================================================================
void SetFactorySettings(void)
{
  ITEM_INFO *ItemDef;

  // afhankelijk van item volgorde in 'item.c'
  for (ItemDef = (ITEM_INFO *)&MmCorMemEE; ItemDef != &LastEE; ItemDef++) {
    // laatste 10 bytes zijn
    if ((unsigned short)ItemDef->PEEAddress >= 0xb600 && (unsigned short)ItemDef->PEEAddress < 0xb7f0) {
      switch (ItemDef->Type) {
        case _S1:
        case _U1 :
        case _BL :
             WriteEEpromByte(  (unsigned char)ItemDef->Default,  (unsigned char *)ItemDef->PEEAddress);
          break;
        case _U2 :
        case _S2:
             WriteEEpromShort((unsigned short)ItemDef->Default, (unsigned short *)ItemDef->PEEAddress);
        break;
      }
    }
  }
  if ((EEMilkMeterType == MMT_VFLOW) || (EEMilkMeterType == MMT_VP4001)){
    WriteEEpromShort(300, (unsigned short *)&EENeutralThreshold);
    WriteEEpromShort(300, (unsigned short *)&EETakeOffThreshold);
    WriteEEpromShort(0, (unsigned short *)&EETakeOffTime);
    WriteEEpromByte(100, (unsigned char *)&EECorFlow0);
  }
}






void HandleKeyExternal(void)
{
  unsigned char  KeyExtSwitchCopy;

  // Key's kunnen ook via de communicatie binnen komen
  switch ( KeyExtSwitchCopy = GetKeyExtSwitch() ){
    case START_CLEAN_PROCESS:                    // start cleaning, centraal
         if ( KeyMode != KM_CLEANING ){
           KeyExtSwitchCopy = KEY_WASH;
           KeyOld           = 0;
           KeyHold          = TRUE;
           KeyA             = KEY_WASH;
         } else {
           KeyExtSwitchCopy = 0;
         }
      break;
    case STOP_CLEAN_PROCESS:                    // stop cleaning, centraal
         KeyOld              = 0;
         KeyA                = KEY_CLUSTER_REMOVAL;
         WaitForI2High       = TRUE;
         KeyExtSwitchCopy    = 0;
      break;
    case SET_WASH_MODE_4:                       // start reinigingsmode 4
         KeyOld              = 0;
         KeyA                = '4';
         KeyExtSwitchCopy    = 0;
      break;
    case SET_WASH_MODE_3:                       // start reinigingsmode 4
         KeyOld              = 0;
         KeyA                = '3';
         KeyExtSwitchCopy    = 0;
      break;
    case SET_PRE_WASH_MODE:                     // zet MPC in pre wash mode (ClusterDown)
         if ( KeyMode == KM_MILKING ){
           KeyOld           = 0;
           KeyState         = KS_NORMAL;
           KeyA             = KEY_SHIFT_2_MANUAL_REL_CLUSTER;
         }
         KeyExtSwitchCopy = 0;
      break;
    case EXTRA_DUMP:
         KeyOld           = 0;
         KeyState         = KS_NORMAL;
         KeyA             = KEY_ACT_MILK_VALVE;
         KeyExtSwitchCopy = 0;
      break;
    case START_I2C_WASH_MODE_MESSAGE:
         if ( (((MilkMeterType%100) == MMT_MEMOLAC) ||
         	     ((MilkMeterType%100) == MMT_VP4001)  || 
         	     ((MilkMeterType%100) == MMT_VFLOW))
             && ((EEI2Control & 0x07) != 0x01)){
           MlkMode     = MM_CLEANING;
           CleanPhase  = CLEAN_PHASE_I2_CONTROLLED_0;
           if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_ON;
           TMSK1 |= IC1I +IC2I + IC3I;
           ValveState       = VALVE_START;
           KeyOld              = 0;
           KeyA                = KeyOld;
         } else {
           KeyOld              = 0;
           KeyA                = '1';
         }
         KeyExtSwitchCopy    = 0;
      break;
    case STOP_I2C_WASH_MODE_MESSAGE:
         KeyOld              = 0;
         KeyA                = '0';
         KeyExtSwitchCopy    = 0;
      break;
    case KEY_AUTO_RELEASE:
    case KEY_AUTO_RELEASE_EXT:
    case KEY_MANUAL_REL_CLUSTER:
         if ( (KeyMode == KM_MILKING) && SwingOver() ){
           KeyOld           = 0;
           KeyState         = KS_NORMAL;
           KeyA             = KeyExtSwitchCopy;
         }
         KeyExtSwitchCopy = 0;
      break;
    default:
         // Gebruik ingangsschakelaar I2 voor bedienen melkstel
         KeyExtSwitchCopy = 0;

         if ( (EEI1Control != 2 )|| (swing_init == 0)){
           switch (StartSwitchState) {
             case SWITCH_CLOSED:
                  if ( KeyState != KS_CONTINUE){

                  	KeyExtSwitchCopy = GetManualorAutoRelease();
                  }
               break;
             case SWITCH_CLOSED_HOLD:
                  if ( KeyState != KS_CONTINUE){
                  	KeyExtSwitchCopy = GetManualorAutoRelease();
                    if ( KeyMode == KM_MILKING) {
                      KeyHold = TRUE;                       // anders wordt in NIET melkmode de hekken niet bediend
                      KeyOld  = 0;                          // anders wordt 'Key' niet in behandeling genomen
                    }
                  }
               break;
             case SWITCH_OPEN:
             case SWITCH_OPEN_HOLD:
                  // geef melken pas weer vrij indien I2 hoog is
                  if ( KeyMode == KM_MILKING )WaitForI2High = FALSE;
               break;
           }
         }
      break;
  } // end switch ( KeyExtSwitchCopy )

  if ( KeyExtSwitchCopy ){
    // wacht tot I2 hoog wordt, einde cleaning
    // alleen programma wordt uit de reiniging-fase gehaald, hek bediening is nog mogelijk
    if ( !((KeyA == KEY_CLUSTER_REMOVAL) || (KeyA == KEY_K3_GATE) || (KeyA == KEY_K4_GATE) || (KeyA == KEY_K5_GATE ))){
      if ( WaitForI2High && (KeyMode != KM_MILKING)) {
        KeyA = KEY_REL;
        DisplayMessage(TXT_MSG_I2_LOW);
      } else {
      	if ( EEI1Control == 2 ){
      	  if ( KeyA == KEY_REL )KeyA = KeyExtSwitchCopy;
      	} else {
      	  KeyA = KeyExtSwitchCopy;
        }
      }
    }
  }
}

// =============================================================================
// Handle Key if pressed
// =============================================================================
void HandleKey(void)
{
    signed char  idx;
  unsigned char  k;
  unsigned short KeyResetTime;
  unsigned short key_hold_time;

  // tbv opstarten uit reinigings-fase
  WaitForI2High = FALSE;
  KeyOld = 0;

  while(1){
    ReadKey();

    if ((KeyA == KeyOld) && (KeyA !=0) && (KeyA != KEY_REL) ) {
      if ( KeyA == KEY_AUTO_RELEASE || KeyA == KEY_MANUAL_REL_CLUSTER ) key_hold_time = 250;
      else                                                              key_hold_time = 100;

      if ( TimeControl( Cnt10msec, KeyHoldTime+key_hold_time)){
        // hold-key van 0,5 -> 1 sec
        KeyHold     = TRUE;
        KeyOld      = !KeyA;
        KeyHoldTime = Cnt10msec;
      }
    } else {
      if ( KeyA != 0 ){
        KeyHoldTime = Cnt10msec;
        KeyHold     = FALSE;
      }
    }

    DispScreen();
    Display();

    // Test of externe toets voor laten zakken melkstel is ingedrukt
    SetStartSwitchState();

    // Test of er een robot bericht is binnen gekomen
    if (EERobot == TRUE ) CheckForRobotMsg();

    // Key's kunnen ook via de communicatie binnen komen
    HandleKeyExternal();

    if ( ModeChange || KeyA != KeyOld || KeyMode == KM_RBT_MILKING) {
      if( (KeyMode != KM_RBT_MILKING) && (KeyA != 0)) KeyOld = KeyA;
      ModeChange = FALSE;

      idx = GetSideIndex(SideState);

      switch (KeyMode) {
// =============================================================================
// MILKING
// =============================================================================
        case KM_MILKING :
             HandleKeyMilking();
          break;


// =============================================================================
// CLEANING
// =============================================================================
        case KM_CLEANING:
             HandleKeyCleaning();
          break;


// =============================================================================
// SERVICE
// =============================================================================
        case KM_SERVICE  :
             HandleKeyService();
          break;

// =============================================================================
// CALIBRATIE
// =============================================================================
        case KM_CAL  :
             HandleKeyCalibration();
          break;

// =============================================================================
// DHIA
// =============================================================================
        case KM_DHIA :
             HandleKeyDhia();
          break;

// =============================================================================
// INPUT
// =============================================================================
        case KM_INPUT :
             HandleKeyInput();
          break;

// =============================================================================
// TEST MODE
// oa. Test peipherals, inputs /outputs
// =============================================================================
        case KM_P_TEST  :
             HandleKeyPeripheralTest();
          break;

// =============================================================================
// ROBOT
// =============================================================================
        case KM_RBT_MILKING :
             HandleKeyRobot();
             break;
// =============================================================================
// ADDRESS
// =============================================================================
        case KM_ADDRESS:
             switch (KeyA) {
               case KEY_SHIFT :
                    if (ScreenEEPtr == &AddressEE) {
                      ScreenNumber = (unsigned char)(&Address2EE- &AddressEE);
                      ScreenEEPtr  = (ITEM_INFO *)&Address2EE;
                    }
                    ScreenEEPtr--;
                    ScreenNumber--;
                    KeyState = KS_NORMAL;
                break;
              case KEY_ENTER:
                   ScreenEEPtr++;
                   ScreenNumber++;
                   if (ScreenEEPtr == ((ITEM_INFO *)&Address2EE+1)) {
                     ScreenEEPtr  = (ITEM_INFO *)&AddressEE;
                     ScreenNumber = 0;
                   }
                break;
              case KEY_CLEAR:
                   // input mode
                   KeyModeOld = KeyMode;
                   KeyMode = KM_INPUT;
                break;
            case KEY_WASH:
                 KeyMode = KM_CLEANING;
              break;
            default :
              break;
          }
          break;

        case KM_TEST:
             if ( KeyA == KEY_WASH && KeyHold == FALSE ){
               KeyMode      = KM_P_TEST;
               MlkState     = MLK_TEST;
               ScreenNumber = 0;
               pt_result    = 0;
             } else {
               ScreenNumber = KeyA;
             }
          break;

        case KM_BURN_IN:
          break;

        case KM_ERROR:
             if ( (KeyA != KEY_REL) && (KeyA )){
               // reset error info, reset stroombewaking en uitgang weer onder spanning
               init_motor(20);

               KeyMode      = KeyModeOld;
               ScreenMode   = ScreenModeOld;
               ScreenNumber = ScreenNumberOld;
             }
          break;
        case KM_DISP_MSG:
          break;


#if ( _EMC_TEST_ )
        case KM_EMC_TEST:
             switch (KeyA) {
               case KEY_ENTER:
                    if ( ScreenMode != SM_EMC_TEST ){
                      ScreenMode      = SM_EMC_TEST;
                      ScreenNumber    = SCREEN_EMC_TEST_0;
                      ScreenNumberOld = !ScreenNumber;
                    } else {
                      ScreenNumber++;
                      if (ScreenNumber == SCREEN_EMC_TEST_MAX) {
                        ScreenNumber = SCREEN_EMC_TEST_2;
                      }
                    }
                    KeyState = KS_NORMAL;
                 break;
             }
          break;
#endif
        default :
             // Indien keymode om een of andere reden niet is gedefinieerd,
             // Geef dan een fout melding
             DispScreenFail(KEY_MODE_ERR);
         break;

      }

      // wijzig scherm als toets nog niet is losgelaten
      DispScreen();
      Display();

      if (KeyHold == TRUE) {
        KeyResetTime = Cnt500msec;
        while ( (k = ReadKey()) != KEY_REL ){

          // anders worden poorten niet aangestuurd zolang een toets word ingedrukt!!
          HndlExtPort();

          // Manuele Reset na 4 seconde reset toets
          if (KeyState == KS_CONTINUE && KeyA == KEY_ACT_MILK_VALVE){
            if ( TimeControl( Cnt500msec, KeyResetTime+8 )){
              RomReset();
            }
          } else {
            if ( TimeControl( Cnt500msec, KeyResetTime+30 )){
              // zet een bericht op het scherm
              DispScreenFail(KEY_HOLD_ERR);
              DispValue((unsigned char *)&k, _U1, 3, 0, READ_ONLY, POS_8);
              Display();
            }
          }
        }
      }
    }

    // reset eventuele shift key
    if ( KeyState ){
      if ( TimeControl( Cnt10msec, KeyShiftTime+300 )){
        KeyState = KS_NORMAL;
      }
    }

    HndlExtPort();
    TaskExit();
  } // while loop
}


