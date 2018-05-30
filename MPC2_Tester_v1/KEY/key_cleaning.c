#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/input.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../lib/mem.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../lib/typedef.h"
#include "../lib/text.h"
#include "../lib/stdlib.h"
#include "../lib/watchdog.h"
#include "../include/cstartup.h"
#include "../appl/feed.h"
#include "../appl/milk.h"
#include "../appl/takeoff.h"
#include "../appl/memolac.h"
#include "../appl/pulsation.h"
#include "../appl/tandem.h"
#include "../appl/milkmisc.h"
#include "../comm/send_msg.h"
#include "../comm/startnet.h"
#include "../comm/flowrcv.h"
#include "../comm/flowctrl.h"
#include "../net/local_net.h"
#include "../net/ext_net.h"

extern unsigned char msgTimeoutTime;

// =============================================================================
// Restart Network
// =============================================================================
void RestartNetwork(void)
{
  unsigned char  idx;
  unsigned short CheckTime;
  unsigned char  done, first;
  unsigned long  Time=0, NextTime;

  // Check netwerk
  Net.NetStat.Arbit = FALSE;
  CheckTime = Cnt1sec;

  // Wordt netwerk gearbitreerd
  while((Net.NetStat.Arbit == FALSE) && (Cnt1sec < (CheckTime +2)))WatchDog();

  if ((Net.NetStat.Arbit == FALSE) || (msgTimeoutTime==0)) {
    // Initialiseer Netwerk,stop Net Receive task
    StopNetReceive = TRUE;
    CheckTime = Cnt1sec;
    while ((StopNetReceive == TRUE) && (Cnt1sec < (CheckTime +5)))WatchDog();

    CheckTime = Cnt1sec;
    while (NetClose(CHANNEL)== -1 && (Cnt1sec < (CheckTime +1)))WatchDog();
    InitStartNet();

    TaskStart(NET_REC_TASK);
  }

  if (EEAddressCopy !=0) {
    CheckTime = Cnt1sec;
    // Test of node wordt gearbitreerd
    while (Net.NetStat.Arbit == FALSE) {
      WatchDog();
      if (Cnt1sec > (CheckTime + 2)) {
        if (Net.NetStat.BusAct == TRUE) DispScreenFail(CommunicationError = COMM_INIT_VC4);
        else                            DispScreenFail(CommunicationError = COMM_ERR_VC4);
        Display();
      }

      // indien bus activiteit dan wacht maximaal 50 sec. 
      if (Cnt1sec > (CheckTime + (Net.NetStat.BusAct?50:10))) {
        NetClose(CHANNEL);
        EEAddressCopy = 0;
        CheckTime  = Cnt1sec;
        break;
      }
    }
  }

  if (Net.NetStat.Arbit == TRUE && EEAddressCopy != 0) {
    CommunicationError = 0;

    idx = GetNodeIndex(EEAddressCopy);
    if (p_PerGenRef[idx] != NULL) {free(p_PerGenRef[idx]); p_PerGenRef[idx] = NULL;}
    done = 0; first = 1; NextTime = 0;
    while (!done) {
      WatchDog();
      Time = Cnt1sec;
      if (PerGenNumber[idx] == 0) {
        if (Time >= NextTime) {
          NextTime = Cnt1sec + 15;
          HndlPerGenNumberRefReq(EEAddressCopy);
        }
      } else {
        if (first) {NextTime = 0; first = 0;}
        if (p_PerGenRef[idx] == NULL) {
          if (Time >= NextTime) {
            if (HndlPerGenRefReq(PerGenNumber[idx], EEAddressCopy)){
              NextTime = Cnt1sec + 15;
            }
          }
        } else {
          done = 1;
        }
      }
      TaskExit();
    }



    if (EEAddress2 != 0) {
      idx = GetNodeIndex(EEAddress2);
      if (p_PerGenRef[idx] != NULL) {free(p_PerGenRef[idx]); p_PerGenRef[idx] = NULL;}
      //check second  PerGenNumber
      if (EEAddress2 != 0 && EEAddressCopy != 0) {
        done = 0; first = 1; NextTime = 0;
        while (!done) {
          WatchDog();
          Time = Cnt1sec;
          if (PerGenNumber[idx] == 0) {
            if (Time > NextTime) {
              NextTime = Cnt1sec + 15;
              HndlPerGenNumberRefReq(EEAddress2);
            }
          } else {
            if (first) {NextTime = 0; first = 0;}
            if (p_PerGenRef[idx] == NULL) {
              if (Time > NextTime) {
                if ( HndlPerGenRefReq(PerGenNumber[idx], EEAddress2)){
                  NextTime = Cnt1sec + 15;
                }
              }
            } else {
              done = 1;
            }
          }
          TaskExit();
        }
      }
    }

    // Request for cow  on both sides
    idx = GetNodeIndex(EEAddressCopy);
    while(HndlMlkRefReq(0, COWNULL_REF, idx) ==0)TaskExit();
    if ( SwingOver() ) {
      while(HndlMlkRefReq(0, COWNULL_REF, SIDE_B) == 0)TaskExit();
    }
  }
}


// =============================================================================
// HandleKeyClear
// =============================================================================
void HandleKeyClear( void )
{
  unsigned short DummyValue = 0;
           long  value;

  if ( KeyHold ){
    // toegang tot service menu
    // edit eerste item, ga naar input mode
    DispValue((unsigned char *)&DummyValue, _U2, 4, 0, WRITE, POS_RIGHT);
    memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_SERVICE_MODE]), 6 );

    Display();

    if ( GetNValue(Item.Size, Item.NrDec, Item.Pos-1, &value)){
      if ( value == 1397L ){
        // ga naar de service mode
        switch (MilkMeterType%100) {
          case MMT_NIVEAU:
               FieldNrMax = (sizeof(NiveauDefSet)/sizeof(ITEM_INFO*))-1;
               DefTable   = NiveauDefSet;
            break;
          case MMT_MEMOLAC:
               FieldNrMax = (sizeof(MemolacDefSet)/sizeof(ITEM_INFO*))-1;
               DefTable   = MemolacDefSet;
            break;
          case MMT_VFLOW:
          case MMT_VP4001:
               FieldNrMax = (sizeof(VFlowDefSet)/sizeof(ITEM_INFO*))-1;
               DefTable   = VFlowDefSet;
            break;
          default:
               // default set Memolac
               FieldNrMax = (sizeof(MemolacDefSet)/sizeof(ITEM_INFO*))-1;
               DefTable   = MemolacDefSet;
          break;
        } // end switch ( MilkMeterType)
        FieldNr         = 0;
        KeyMode         = KM_SERVICE;
        ScreenMode      = SM_SERVICE;
        MlkState        = MLK_SERVICE;
        ScreenEEPtr     = DefTable[FieldNr];
        ScreenNumber    = 0;
        ScreenNumberOld = !ScreenNumber;
        while ( ReadKey() == KEY_ENTER );
      }
    }
  }
}


// =============================================================================
// HandleKeyShiftActMilkValve
// =============================================================================
void HandleKeyShiftActMilkValve( void )
{
  if ( (MilkMeterType == MMT_MEMOLAC) && !(EEI2Control & 0x04) ){
    if ( EERowSize && !(ExternMessages & SEND_EXTRA_DUMP_MESSAGES) ){
      ExternMessages |= SEND_EXTRA_DUMP_MESSAGES;
    }
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyActMilkValve
// =============================================================================
void HandleKeyActMilkValve( void )
{
  if ( (MilkMeterType == MMT_MEMOLAC )                                  &&
     ((CleanPhase == CLEAN_PHASE_0 ) || (CleanPhase == CLEAN_PHASE_END )) ){
    ValveState = VALVE_START_EXTRA_DUMP;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyShiftFour
// =============================================================================
void HandleKeyShiftFour( void )
{
  // indien MASTER zet alle ander MPC's in de rij in PreWashMode
  if ( ((MilkMeterType == MMT_MEMOLAC) ||
  	    (MilkMeterType == MMT_VFLOW)   ||
  	    (MilkMeterType == MMT_VP4001)  ||
  	    (MilkMeterType == MMT_TAKE_OFF_SENSOR)) && ((EEI2Control & 0x04) == 0x00) ){
    if ( EERowSize && !(ExternMessages & SEND_WASH_MODE_4_START_MESSAGES) ){
      ExternMessages |= SEND_WASH_MODE_4_START_MESSAGES;
    }
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyCleaningFour
// =============================================================================
void HandleKeyCleaningFour( void )
{
  if ( ((MilkMeterType == MMT_MEMOLAC) ||
  	    (MilkMeterType == MMT_VFLOW)   ||
  	    (MilkMeterType == MMT_VP4001)  ||
  	    (MilkMeterType == MMT_TAKE_OFF_SENSOR)) && ((EEI2Control & 0x04) == 0x00) ) {
    StartCleanPhase4 = Cnt1sec;
    ClnPhsStartTime  = Cnt1sec;
    if (MilkMeterType == MMT_MEMOLAC)ValveState  = VALVE_START;
    VacuumState      = VACUUM_ON;
    MlkState         = MLK_RESTART_CLEANING;
    CleanPhase       = CLEAN_PHASE_PRE_CLEAN;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyCleaningShiftThree
// =============================================================================
void HandleKeyCleaningShiftThree( void )
{
  // indien MASTER zet alle ander MPC's in de rij in wash-mode 3
  if ( EERowSize && ((EEI2Control & 0x04) == 0x00) && (!(ExternMessages & SEND_WASH_MODE_3_START_MESSAGES)) ){
    ExternMessages |= SEND_WASH_MODE_3_START_MESSAGES;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyCleaningThree
// =============================================================================
void HandleKeyCleaningThree( void )
{
  if ( ((MilkMeterType == MMT_MEMOLAC) ||
  	    (MilkMeterType == MMT_VFLOW)   ||
  	    (MilkMeterType == MMT_VP4001)  ||
  	    (MilkMeterType == MMT_TAKE_OFF_SENSOR)) && ((EEI2Control & 0x04) == 0x00) ) {
    PlsState    = PLS_STOP_PULSATION;
    CleanPhase  = CLEAN_PHASE_END;
    VacuumState = VACUUM_ON;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyCleaningTwo
// =============================================================================
void HandleKeyCleaningTwo( void )
{
  if ( ((MilkMeterType == MMT_MEMOLAC) ||
  	    (MilkMeterType == MMT_VFLOW)   ||
  	    (MilkMeterType == MMT_VP4001)  ||
  	    (MilkMeterType == MMT_TAKE_OFF_SENSOR)) && ((EEI2Control & 0x04) == 0x00) ) {
    StartCleanPhase2 = Cnt1sec;
    ClnPhsStartTime  = Cnt1sec;
    if (MilkMeterType == MMT_MEMOLAC)ValveState       = VALVE_START;
    VacuumState      = VACUUM_ON;
    CleanPhase       = CLEAN_PHASE_2;
  }
  KeyState = KS_NORMAL;
}


// =============================================================================
// HandleKeyCleaningOne
// =============================================================================
void HandleKeyCleaningOne( void )
{

  if ( (MilkMeterType == MMT_MEMOLAC) ||
  	   (MilkMeterType == MMT_VFLOW)   ||
  	   (MilkMeterType == MMT_VP4001)  ||
  	   (MilkMeterType == MMT_TAKE_OFF_SENSOR)) {
    if ((EEI2Control & 0x04) == 0x00){
      StartCleanPhase1 = Cnt1sec;
      ClnPhsStartTime  = Cnt1sec;
      if ( MilkMeterType == MMT_MEMOLAC){
        ValveState = VALVE_START;
        CleanState       = EMPTY_MILK_METER;
      }
      VacuumState      = VACUUM_ON;
      CleanPhase       = CLEAN_PHASE_1;
    }
  } else {
    if ( MilkMeterType == MMT_NIVEAU ) {
      ul_CleanStartTime = Cnt1sec;
      TakeOffState      = TO_START_CLEAN;
      MlkState          = MLK_CLEANING;
    }
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyCleaningZero
// =============================================================================
void HandleKeyCleaningZero( unsigned char master )
{
  if ( (MilkMeterType == MMT_MEMOLAC) ||
  	   (MilkMeterType == MMT_VFLOW)   ||
  	   (MilkMeterType == MMT_VP4001)  ||
  	   (MilkMeterType == MMT_TAKE_OFF_SENSOR)) {
    if ( ((EEI2Control & 0x07) == 6 ) || ((EEI2Control & 0x07) == 2 )) {
      // nieuwe reinigen
      // stop I2 controlled washing
      CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
      if ( MilkMeterType == MMT_MEMOLAC) ValveState = VALVE_START_EXTRA_DUMP;
      PlsState   = PLS_STOP_PULSATION;
      if ( EERowSize && master){
        ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;    // stop reinigen, centraal
      }
//       else {
        if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
//      }
    } else {
      // gaat direct door naar phase 0, pulsatie wordt eventueel in phase 0 aangezet
      PlsState    = PLS_STOP_PULSATION;
      CleanPhase  = CLEAN_PHASE_START;
      VacuumState = VACUUM_ON;
    }
  } else {
    if ( MilkMeterType == MMT_NIVEAU ) {
      PlsState     = PLS_STOP_PULSATION;
      TakeOffState = TO_CLEANING_END;
      MlkState     = MLK_CLEANING_END;
    }
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyCleaningK5Gate
// =============================================================================
void HandleKeyCleaningK5Gate( void )
{
  if(!KeyHold){
    ExtPortK5State = EPK5S_TOGGLE;
    KeyState = KS_NORMAL;
  }
}


// =============================================================================
// HandleKeyCleaningK4Gate
// =============================================================================
void HandleKeyCleaningK4Gate( void )
{
  if ( !KeyHold ){
    ExtPortK4State = EPK4S_TOGGLE;
    KeyState = KS_NORMAL;
  }
}


// =============================================================================
// HandleKeyCleaningK3Gate
// =============================================================================
void HandleKeyCleaningK3Gate( void )
{
 if(!KeyHold && !(SwingOver() && EEFeeding) ){
   if ( (EEParlourType == PT_HERRING_BONE) && EEGateK3Time ) {
      ExtPortK3State = EPK3S_ON;
     GateK3ActiveTime = Cnt10msec;
   } else {
      ExtPortK3State = EPK3S_TOGGLE;
   }
 }
}


// =============================================================================
// HandleKeyCleaningClusterRemoval
// =============================================================================
void HandleKeyCleaningClusterRemoval( void )
{
  if (EERobot == FALSE) {
    TaskStop(MILK_CONTROL_TASK);
    TaskStop(FEED_A_CONTROL_TASK);
    if ( EEAddress2 )TaskStop(FEED_B_CONTROL_TASK);      // swing-over
    TaskStop(TANDEM_CONTROL_TASK);
    InitMlk();
    InitFeed();
    if ( (SoftKey & SK_TANDEM_ENABLE ) &&
         ((EEParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) && (EEParlourType >= PT_SEMI_TANDEM)) ){
      OTStartFromCleaning();                    // Initieer open-tandem
      TaskStart(TANDEM_CONTROL_TASK);
    }
    ParlourType     = EEParlourType;
    MlkState        = MLK_WAIT_FOR_START;
    InitRti();
    TakeOffState    = TO_FIRST_START;
    KeyState        = KS_NORMAL;
    KeyMode         = KM_MILKING;
    EditItem        = 1;
    ScreenNumber    = SCREEN_MILK_0;
    ScreenMode      = SM_MILKING;
    ScreenNumberOld = !ScreenNumber;
    WaitForI2High   = TRUE;
    if ( SwingOver() || ( EEI1Control == 2)){
    	SwingTime = Cnt10msec;
      swing_init = 1;
      GateState = 0xff;
      MlkSessionAvailable[SIDE_A] = FALSE;
      MlkSessionAvailable[SIDE_B] = FALSE;
    }
    CowMilked[0] = 0;
    CowMilked[1] = 0;

    TaskStart(MILK_CONTROL_TASK);
    if (EEFeeding && ( SoftKey & SK_FEEDING_ENABLE) ){
      TaskStart(FEED_A_CONTROL_TASK);
      if ( EEAddress2 ) TaskStart(FEED_B_CONTROL_TASK);     // swing-over
    }
    DispScreen();
    // Check NetWork; first wait for all message buffers empty
//    while (NetCheckSending(1) == 1)TaskExit();
    RestartNetwork();
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyCleaningShiftClusterRemoval
// =============================================================================
void HandleKeyCleaningShiftClusterRemoval( void)
{
  if ( EERowSize && !(ExternMessages & SEND_CLEAN_START_MESSAGES) ){
    ExternMessages |= SEND_CLEAN_STOP_MESSAGES;    // stop reinigen, centraal
  }
  KeyState = KS_NORMAL;
}

// =============================================================================
// HandleKeyCleaningWash
// =============================================================================
void HandleKeyCleaningWash( void )
{
 if (KeyHold == FALSE) {
   switch (MilkMeterType%100) {
     case MMT_NIVEAU:
     case MMT_MEMOLAC:
     case MMT_VFLOW:
     case MMT_VP4001:	
     case MMT_TAKE_OFF_SENSOR:
          PlsState     = PLS_STOP_PULSATION;
          KeyMode      = KM_DHIA;
          MlkState     = MLK_DHIA;
          ScreenNumber = SCREEN_RESET;
       break;
     default:
       break;
   }
 }
 KeyState = KS_NORMAL;
}



// =============================================================================
// HandleKeyCleaning
// =============================================================================
void HandleKeyCleaning( void)
{
  KeyValue = KeyA + KeyState;

  switch (KeyValue) {
    case KEY_WASH:
         HandleKeyCleaningWash();
      break;

    case KEY_SHIFT_CLUSTER_REMOVAL:                //  pijl up
         HandleKeyCleaningShiftClusterRemoval();
      break;

    case KEY_CLUSTER_REMOVAL:
         HandleKeyCleaningClusterRemoval();
      break;

    case KEY_K3_GATE :
         HandleKeyCleaningK3Gate();
      break;

    case KEY_K4_GATE :
         HandleKeyCleaningK4Gate();
      break;
    case KEY_K5_GATE :
         HandleKeyCleaningK5Gate();
      break;

    // memolac meters kunnen direct in een bepaalde reinigings-fase gezet worden
    case '0':
         HandleKeyCleaningZero(0);
      break;
    case KEY_SHIFT_0:
         HandleKeyCleaningZero(1);
      break;
    case '1':
         HandleKeyCleaningOne();
      break;

    case '2':
         HandleKeyCleaningTwo();
      break;

    case '3':
         HandleKeyCleaningThree();
      break;

    case KEY_SHIFT_3:
         HandleKeyCleaningShiftThree();
      break;

    case '4':
         HandleKeyCleaningFour();
      break;
    case KEY_SHIFT_4:
         HandleKeyShiftFour();
      break;

    case KEY_ACT_MILK_VALVE :
         HandleKeyActMilkValve();
      break;

    case KEY_SHIFT_ACT_MILK_VALVE:
         HandleKeyShiftActMilkValve();
      break;

    case KEY_SHIFT:
         KeyState = KS_CONTINUE;
         KeyShiftTime = Cnt10msec;
      break;

    case KEY_CLEAR:
         HandleKeyClear();
      break;

    default :
      break;
  }
}
