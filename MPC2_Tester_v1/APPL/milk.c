// =============================================================================
// MODULE MILKdex
// =============================================================================

#include "milk.h"
#include "mmmodes.h"
#include "mm_4001.h"
#include "niveau.h"
#include "memolac.h"
#include "take_off_sensor.h"
#include "tandem.h"
#include "takeoff.h"
#include "conduct.h"
#include "temp.h"
#include "feed.h"
#include "milkmisc.h"
#include "pulsation.h"
#include "../key/key.h"
#include "../key/key_input.h"
#include "../key/keyint.h"
#include "../key/disp.h"
#include "../lib/time.h"
#include "../lib/rti_int.h"
#include "../lib/mem.h"
#include "../lib/item.h"
#include "../lib/time.h"
#include "../lib/boolean.h"
#include "../lib/io6811.h"
#include "../lib/intr6811.h"
#include "../lib/stdlib.h"
#include "../lib/eehc11.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../lib/burnin.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../lib/error.h"
#include "../lib/reg_flags.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../include/romstnet.h"
#include "../include/romeehc1.h"
#include "../init/start.h"
#include "../comm/flowctrl.h"

extern void VflowMeasuring(void);

#pragma codeseg(EEPAGE)


MlkTbl         a_MlkTbl[MLK_TBL_MAX];
unsigned char  MlkAmountProfileTable[MLK_AMOUNT_PROFILE_TABLE_INDEX_MAX];
unsigned char  MlkState, MlkAttention, MlkContinue, FlowState, MlkSepStandAlone, MlkMode;
unsigned char  RobotState;
unsigned char  MilkMeterType, MilkManualInput, ValveState;
unsigned long  MlkMaxFlow, MlkAmount, SendMlkAmount, StoredMlkAmount, MlkAmountRobot, MlkFlowLast, MlkActFlow;
unsigned long  ul_MlkKickOffTime, ul_MlkNeutralTime, ul_MlkTakeOffTime;
unsigned long  Cnt1secOld, Cnt1secLast;
unsigned short MlkDuration, MlkFlowProc, MlkDurationCur, MlkAmountKg, MlkResOtherSide;
unsigned short MlkTakeOffThreshold, MlkKickOffThreshold, MlkAutomatThreshold;
unsigned long  ul_MlkStartTime, ul_CleanStartTime;
unsigned short MlkSendTime, SwingTime;
unsigned short ValveStartTime, GateK3ActiveTime, GateK4ActiveTime, GateK5ActiveTime;
unsigned char  CleanState, CleanPhase;
unsigned char  MlkUpdateCntr,StartMilkAutomat;
unsigned char  swing_init,FirstAttach;

unsigned char  CowMlkYldNrOfAttach2min;     // aantal keren opnieuw onderhangen gedurende de eerste 2 minuten.
unsigned char  CowMlkYldNrOfAttach;         // totaal aantal keren opnieuw onderhangen.
unsigned char  CowMlkYldManDetach;          // manual.
unsigned short CowMlkYldAttachTime;         // tijd dat de klauw voor de eerste keer werd ondergehangen.

unsigned long  CowMlkYldStartTime[MAX_NODES];
unsigned char  CowMlkYldManInput[MAX_NODES];
unsigned long  CowMlkYldStartTime_B[MAX_NODES];
unsigned char  CowMlkYldManInput_B[MAX_NODES];

short          MlkTblIndex;
unsigned char  MlkAmountProfileTableIndex;
void          *p_MlkRef[MAX_NODES];
void          *p_MlkRef_B[MAX_NODES];
unsigned char  MlkRefState[MAX_NODES];
unsigned char  MeasState[MAX_NODES];
BOOLEAN        SepMsgSend;
PerMatMlkRef   PerRbtMlkRef;
unsigned short CowMlkYldAmount00_15, CowMlkYldAmount15_30, CowMlkYldAmount30_60, CowMlkYldAmount60_120;
unsigned long  CowMilked[MAX_NODES];
BOOLEAN        MlkSessionAvailable[MAX_NODES];


BOOLEAN meas;
//  unsigned long ActMlkFlw;

extern unsigned short VflowMlkAmountStart;
extern unsigned short VflowMlkAmountEnd;

// prototype's -----------------------------------------------------------------
void           InitMilk(void);
unsigned long  CalcActMlkFlow(void);
unsigned char  CalcTakeOff(void);
void           HndlTakeOff(void);
unsigned char  CalcKickOff(void);

extern void ReportGateSwitch(unsigned char Gate, unsigned char onOff);

// =============================================================================
// Initialize MlkAmountProfileTable
// =============================================================================
void InitMlkAmountProfileTable(void)
{
  unsigned char i;

  for (i = 0; i < MLK_AMOUNT_PROFILE_TABLE_INDEX_MAX; i++) MlkAmountProfileTable[i] = 0;
  MlkAmountProfileTableIndex = 0;
}

// =============================================================================
// Geef alle gebruikte pointers weer vrij
// =============================================================================
void FreePointers( void)
{
  if (p_CowActRef[0]   != NULL) { free(p_CowActRef[0]);   p_CowActRef[0]   = NULL; }
  if (p_CowActRef[1]   != NULL) { free(p_CowActRef[1]);   p_CowActRef[1]   = NULL; }
  if (p_CowConRef[0]   != NULL) { free(p_CowConRef[0]);   p_CowConRef[0]   = NULL; }
  if (p_CowConRef[1]   != NULL) { free(p_CowConRef[1]);   p_CowConRef[1]   = NULL; }
  if (p_CowTmpRef[0]   != NULL) { free(p_CowTmpRef[0]);   p_CowTmpRef[0]   = NULL; }
  if (p_CowTmpRef[1]   != NULL) { free(p_CowTmpRef[1]);   p_CowTmpRef[1]   = NULL; }
  if ( EEAddressCopy) {
    // alleen opruimen indien NIET stand-alone
    if (p_CowFedParlRef[0] != NULL) { free(p_CowFedParlRef[0] ); p_CowFedParlRef[0] = NULL; }
    if (p_CowFedParlRef[1] != NULL) { free(p_CowFedParlRef[1] ); p_CowFedParlRef[1] = NULL; }
    if (p_CowFedParlRes[0] != NULL) { free(p_CowFedParlRes[0] ); p_CowFedParlRes[0] = NULL; }
    if (p_CowFedParlRes[1] != NULL) { free(p_CowFedParlRes[1] ); p_CowFedParlRes[1] = NULL; }
  }
  if (p_CowSepInfoMail[0]      != NULL) { free(p_CowSepInfoMail[0]);      p_CowSepInfoMail[0]      = NULL; }
  if (p_CowSepInfoMail[1]      != NULL) { free(p_CowSepInfoMail[1]);      p_CowSepInfoMail[1]      = NULL; }
  if (p_CowDiseaseMail[0]      != NULL) { free(p_CowDiseaseMail[0]);      p_CowDiseaseMail[0]      = NULL; }
  if (p_CowDiseaseMail[1]      != NULL) { free(p_CowDiseaseMail[1]);      p_CowDiseaseMail[1]      = NULL; }
  if (p_CowGenRef[0]           != NULL) { free(p_CowGenRef[0]);           p_CowGenRef[0]           = NULL; }
  if (p_CowGenRef[1]           != NULL) { free(p_CowGenRef[1]);           p_CowGenRef[1]           = NULL; }
  if (p_CowMlkPlsRef[0]        != NULL) { free(p_CowMlkPlsRef[0]);        p_CowMlkPlsRef[0]        = NULL; }
  if (p_CowMlkPlsRef[1]        != NULL) { free(p_CowMlkPlsRef[1]);        p_CowMlkPlsRef[1]        = NULL; }
  if (p_CowActMemAttRef[0]     != NULL) { free(p_CowActMemAttRef[0]);     p_CowActMemAttRef[0]     = NULL; }
  if (p_CowActMemAttRef[1]     != NULL) { free(p_CowActMemAttRef[1]);     p_CowActMemAttRef[1]     = NULL; }

}

#if _DEBUG_MESSAGES_
void ReportStateChange(void)
{
  unsigned char  buffer[MESS_LEN(DbgMessStateRes)+2];
  PerFuncTest    *PerFuncTestPtr;

  void          *DbgMessStateResPtr = (void *)(buffer + 4);

  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = DbgMessStateResId;
     
  ((DbgMessStateRes *)DbgMessStateResPtr)->DbgMlkState = MlkState;
  ((DbgMessStateRes *)DbgMessStateResPtr)->DbgToState = TakeOffState;

  while (SendMessage( ConLow, GetNodeAddress(SideState), 0, MESS_LEN(DbgMessStateRes)+2, buffer) == 0)TaskExit();
}
#endif

void FreeMlkRef(signed char side, unsigned char call_id)
{
  unsigned char  buffer[MESS_LEN(DbgMessRes)+2];
  PerFuncTest    *PerFuncTestPtr;

  void          *DbgMessResPtr = (void *)(buffer + 4);

  if (side != -1){
    if (p_MlkRef[side] != NULL) { free(p_MlkRef[side]); p_MlkRef[side] = NULL; }
  }

  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = DbgMessResId;
     
  ((DbgMessRes *)DbgMessResPtr)->DbgMessErrorNr = call_id;
  ((DbgMessRes *)DbgMessResPtr)->DbgMlkState = MlkState;

  while (SendMessage( ConLow, GetNodeAddress(side), 0, MESS_LEN(DbgMessRes)+2, buffer)==0)TaskExit();
}

void FreeMlkRefB(signed char side)
{
  if (side == -1) return;
  if (p_MlkRef_B[side] != NULL) { free(p_MlkRef_B[side]); p_MlkRef_B[side] = NULL; }
}

// =============================================================================
// SendStartEndWeight
// Sends start and end weight of the VFlow
// =============================================================================
void SendStartEndWeight(signed char side)
{
  unsigned char  buffer[MESS_LEN(DbgMessResL)+6];
  PerFuncTest    *PerFuncTestPtr;
  void          *DbgMessResLPtr = (void *)(buffer + 4);

  if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
   
    *((unsigned short *)buffer) = PerFuncTestId;
    PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
    PerFuncTestPtr->PerFuncTestCommand = DbgMessResLId;

    ((DbgMessResL *)DbgMessResLPtr)->DbgMessId = DBG_VFLOW_WEIGHT_RES_ID;
    (((DbgMessResL *)DbgMessResLPtr)->Array)[0].DbgData = VflowMlkAmountStart;
    (((DbgMessResL *)DbgMessResLPtr)->Array)[1].DbgData = VflowMlkAmountEnd;
    (((DbgMessResL *)DbgMessResLPtr)->Array)[2].DbgData = EECorVFlow;

    SendMessage( ConLow, GetNodeAddress(side), 0, MESS_LEN(DbgMessResL)+6, buffer);
  }
}

// =============================================================================
// Initialize milk
// =============================================================================
void InitMlk(void)
{
  FreeMlkRef(0, FNC_INIT);
  FreeMlkRef(1, FNC_INIT);
  FreeMlkRefB(0);
  FreeMlkRefB(1);

  FreePointers();


  MlkRefState[0] = COWS_NONE;
  MlkRefState[1] = COWS_NONE;
  MeasState[0]   = MEAS_IDLE;
  MeasState[1]   = MEAS_IDLE;

  CowMlkYldStartTime[0] = 0;
  CowMlkYldStartTime[1] = 0;
  CowMlkYldManInput[0]  = 0;
  CowMlkYldManInput[1]  = 0;

  CowMlkYldStartTime_B[0] = 0;                       // ivm swing_over
  CowMlkYldStartTime_B[1] = 0;
  CowMlkYldManInput_B[0]  = 0;
  CowMlkYldManInput_B[1]  = 0;

  InitGateState();
  InitOpenTandem();
  MilkMeterType = EEMilkMeterType;    //
  if ( MilkMeterType == MMT_NIVEAU) MlkState = MLK_CLEANING_END;
  else                              MlkState = MLK_CLEANING;

  MlkAttention                 = 0;
  ul_CleanStartTime            = Cnt1sec;
  MlkSepStandAlone             = 0;
  PerRbtMlkRef.PerMatMlkStart  = 0;
  PerRbtMlkRef.PerMatMlkEnd    = 0;
  PerRbtMlkRef.PerMatMlkFailed = 0;
  RobotState                   = RBT_MLK_CLEANING;
  EmptyGlassAuto               = TRUE;
  MlkDurationCur               = 0;
  InitConductivity();
  InitTemperature();
  DispFlash                    = 0;
  SepMsgSend                   = FALSE;

  memory_clear ( CowDisease[0], 14 );
  memory_clear ( CowDisease[1], 14 );
  nr_of_disease_trt_items[0]   = nr_of_disease_trt_items[1] = disease_trt_number = 0;
}

unsigned short GetMlkTakeOffTime(void)
{
  unsigned short x =   Cnt1sec + abs(EETakeOffTime);

  if ( p_CowMlkPlsRef[SideState] != NULL ){
    if ( LONG_COW_GEN_NUMBER ){
      if (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_TAKE_OFF){
        x = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsTakeOffTime;
      }
    } else {
      if (((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_TAKE_OFF){
        x = ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsTakeOffTime;
      }
    }
  }
  return x;
}

// =============================================================================
unsigned short GetStrippingTreshold(void)
{
  unsigned short x = MlkAutomatThreshold;

  if ( p_CowMlkPlsRef[SideState] != NULL ){
    if ( LONG_COW_GEN_NUMBER ){
      if ( ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ){
         x = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripThreshold;
      }
    } else {
      if ( ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_STRIPPING ){
         x = ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsStripThreshold;
      }
    }
  }
  return x;
}

// =============================================================================
unsigned short GetTakeOffTreshold(void)
{
  unsigned short x = EETakeOffThreshold;

  if (p_CowMlkPlsRef[SideState] != NULL) {
    if ( LONG_COW_GEN_NUMBER ){
      if (((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_TAKE_OFF){
        x = ((ExtCowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsTakeOffThresHold;
      }
    } else {
      if (((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsSetting & ENABLE_TAKE_OFF){
        x = ((CowMlkPlsRef **)p_CowMlkPlsRef)[SideState]->CowMlkPlsTakeOffThresHold;
      }
    }
  }
  return x;
}

// =============================================================================
// Initialize milk table
// =============================================================================
void InitMilkTable(void)
{
  unsigned short i;

  MlkTakeOffThreshold = GetTakeOffTreshold();
  MlkAutomatThreshold = EEFlowLvl2Threshold;
  FlowState = 0;
  MlkAttention = 0;
  MilkManualInput = FALSE;
  Cnt1secLast = 0;
  MlkDurationCur = 0;
  MlkActFlow = 0;
  MlkMaxFlow = 0;
  MlkUpdateCntr = 0;
//  VflowPulses = VflowPulses > 300 ? 0 : VflowPulses;
//  VflowPulsesStart = VflowPulses;
  for (i = 0; i < MLK_TBL_MAX; i++) {
    a_MlkTbl[i].MlkTblYield = 0L;
    a_MlkTbl[i].MlkTblTime = 0;
  }
  MlkTblIndex = -1;
  InitMlkAmountProfileTable();
  InitConductivity();
  InitTemperature();

  // tbv pulsatie, indien automatisch dan togglepuls op 1 zodat bij bediening
  // toets de stimulatie uit gezet wordt
  PLSCntrl.FirstTimeStimulation = 0;
  if ((EEPulsation & (PLS_ENABLE+PLS_MANUAL_ENABLE)) == PLS_ENABLE) PLSCntrl.ManualPulseToggle = 1;
  else                                                              PLSCntrl.ManualPulseToggle = 0;
}



// =============================================================================
// Put values in MlkTbl ( elke seconde )
// =============================================================================
void PutMlkTbl(unsigned long Yield, unsigned short Time)
{
  unsigned long flow;

  MlkTblIndex++;
  if (MlkTblIndex == EEMlkFlwCalcTime) MlkTblIndex = 0;

  a_MlkTbl[MlkTblIndex].MlkTblYield = Yield;
  a_MlkTbl[MlkTblIndex].MlkTblTime  = Time;

  flow = CalcActMlkFlow();
  if ( flow >= EENeutralThreshold) FlowState = 1;
}


// =============================================================================
// Put values in MlkTable, every 5 seconds
// =============================================================================
void PutMlkAmountInTable(unsigned char yield )
{
  if ( MlkAmountProfileTableIndex < MLK_AMOUNT_PROFILE_TABLE_INDEX_MAX ){
    MlkAmountProfileTable[ MlkAmountProfileTableIndex++] = yield;
  }
}


// =============================================================================
// Calculate Actual Milk Flow & Update MlkMaxFlow if needed
// =============================================================================
unsigned long CalcActMlkFlow(void)
{
  long flow;
  unsigned short t;
  short i;

  t = 0;
  flow = 0L;

  if (MlkTblIndex != -1) {
//    if ( MilkMeterType == MMT_VFLOW ){
//      flow = MlkActFlow;
//    } else {
      i = MlkTblIndex;
      do {
        t += a_MlkTbl[i].MlkTblTime;
        i--; if (i < 0) {i = EEMlkFlwCalcTime - 1;}
      } while ((i != MlkTblIndex) && (a_MlkTbl[i].MlkTblTime > 0));
      i++; if (i == EEMlkFlwCalcTime) {i = 0;}
      t -= a_MlkTbl[i].MlkTblTime;

      if (i != MlkTblIndex) {
        if (a_MlkTbl[MlkTblIndex].MlkTblYield > a_MlkTbl[i].MlkTblYield) {
          flow = a_MlkTbl[MlkTblIndex].MlkTblYield - a_MlkTbl[i].MlkTblYield;
          flow = (flow * 6000L) / (long)t;
          if (MilkMeterType == MMT_NIVEAU) {
            //minimaal 1 minuut melken
            if (flow > MlkMaxFlow && MlkDurationCur > 60) { MlkMaxFlow = flow; }
          } else {
            if( flow > MlkMaxFlow && (((short)(t/100)) >= (MlkFlwCalcTimeEE.MinVal-1))) {
              MlkMaxFlow = flow;
            }
          }
          if( ((short)(t/100)) >= (MlkFlwCalcTimeEE.MinVal-1)) {
            MlkActFlow = flow;
#if DEBUG_VP4001
            if (MlkActFlow > 5000) {
              MlkActFlow = 0;
            }
#endif            
          } else {
            flow = 0;
          }
        }
      }
//    }
  }
  return(flow);
}

// =============================================================================
// Calculate Kick Off
// =============================================================================
unsigned char KickedOff(void)
{
  long           flow;
  unsigned short t;
  short          i;

  if (Cnt1sec >= ul_MlkKickOffTime) {
    t = 0;
    flow = 0L;
    if (MlkTblIndex >= 0) {

      i = MlkTblIndex;
      do {
        t += a_MlkTbl[i].MlkTblTime;
        i--; if (i < 0) {i = EEMlkFlwCalcTime - 1;}
      } while ((i != MlkTblIndex) && (a_MlkTbl[i].MlkTblTime > 0) && (t <= 1000));

      i++; if (i == EEMlkFlwCalcTime) {i = 0;}
      t -= a_MlkTbl[i].MlkTblTime;

      if (i != MlkTblIndex) {
        flow = a_MlkTbl[MlkTblIndex].MlkTblYield - a_MlkTbl[i].MlkTblYield;
        flow = (flow * 6000L) / (long)t;
        if (flow >= MlkKickOffThreshold) {
          if (flow < MlkFlowLast) {
            flow = MlkFlowLast - flow;
            if ((unsigned short)((flow * 100) / MlkFlowLast) >EEKickOffFlowProc) {
              return(1);
            }
          }
        } else {
          MlkFlowLast = flow;
        }
      }
    }
    ul_MlkKickOffTime = Cnt1sec + EEKickOffSampleTime;
  }
  return(0);
}


// =============================================================================
unsigned char CheckMilkAutomatFlow(void)
{

  if ( MilkMeterType ==  MMT_TAKE_OFF_SENSOR ){
    if ( take_off_sensor_ratio <= EEFlowLvl2Threshold ) return 1;
  } else {
    if ( CalcActMlkFlow() <= GetStrippingTreshold()) return 1;
  }
  return 0;
}

// =============================================================================
void CheckMilkAutomat(void)
{
  static unsigned short MlkAutomatTime;

  if (EEOutputK2 == MILK_AUTOMAT_CONTROL) {
    //Procedure namelk automaat starten
    switch ( StartMilkAutomat ){
      case MA_MANUAL_START:
           StartMilkAutomat = MA_MANUAL_STARTED;
           MlkAutomatTime = Cnt1sec + EEVacuumDly;
        break;
      case MA_MANUAL_STARTED:
           if (Cnt1sec > MlkAutomatTime) {
             if ( MilkAutomatState != MILK_AUTOMAT_ON_OFF &&
                  MilkAutomatState != MILK_AUTOMAT_OFF_ON ){
               MilkAutomatState = MILK_AUTOMAT_START;
             }
           }
        break;
      case MA_AUTO_START:
           if ( CheckMilkAutomatFlow() ){
             if (Cnt1sec > MlkAutomatTime) {
               if ( MilkAutomatState != MILK_AUTOMAT_ON_OFF &&
                    MilkAutomatState != MILK_AUTOMAT_OFF_ON ){
                 MilkAutomatState = MILK_AUTOMAT_START;
               }
             }
           } else {
             StartMilkAutomat = 0;
           }
        break;
    default:
           if ( CheckMilkAutomatFlow() ){
             if ( TakeOffState != TO_MAN ){
               StartMilkAutomat = MA_AUTO_START;
               MlkAutomatTime   = Cnt1sec + EEVacuumDly;
             }
           }
    } // end switch (..)
  }
}

// =============================================================================
// Calculate Automatic Take Off (call ones per second)
// =============================================================================
unsigned char CalcTakeOff(void)
{
  unsigned long ActMlkFlw;

  if (Cnt1sec > ul_MlkNeutralTime) {
    ActMlkFlw = CalcActMlkFlow();
    if (ActMlkFlw > MlkTakeOffThreshold) {
    } else {
    }
    return(0);
  } else {
    return(0);
  }
}

// =============================================================================
// UpdateMlkProfileTable
// =============================================================================
static void UpdateMlkProfileTable(unsigned long MlkAmount)
{
  PutMlkAmountInTable((unsigned char)(((MlkAmount-StoredMlkAmount)+5)/10));
  StoredMlkAmount = MlkAmount;
}

// =============================================================================
// SendMlkState
// =============================================================================
static void HandleMlkStateUpdateRes(unsigned long MlkAmount, unsigned char MlkState)
{
  if (MlkUpdateCntr) {
    MlkUpdateCntr--;
  } else {
    MlkUpdateCntr = EEMlkStateUpdateCntr;
    if ((SendMlkAmount != MlkAmount) && (MlkState != MLK_IDLE) && (MlkState != MLK_DONE)){
      HndlMlkStatusUpdateRes(MlkAmount, MlkState);
      SendMlkAmount = MlkAmount;
    }
  }
}

// =============================================================================
// UpdateAndSendMlkState
// =============================================================================
void UpdateAndSendMlkState(unsigned long MlkAmount, unsigned char MlkState)
{
  if (Cnt1sec != Cnt1secLast) {
    // VP4001: MlkAmounts already stored in MlkTable, done when message is received.
  	if (MilkMeterType != MMT_VP4001){
      PutMlkTbl(MlkAmount,100);
    }
      
    // bewaar ook het verloop van de melkgift iedere 5 sec.
    if ((Cnt1sec % 5) == 0 ){
      UpdateMlkProfileTable(MlkAmount);
      HandleMlkStateUpdateRes(MlkAmount, MlkState);
    }
    Cnt1secLast = Cnt1sec;
  }
}


// =============================================================================
// Handle    Milk Measuring
// =============================================================================
void HndlMeasuring(void)
{
  switch (MilkMeterType) {
    case  MMT_TAKE_OFF_SENSOR:
          TakeOffSensorMeasuring();
      break;
    case  MMT_NIVEAU :
          NiveauMeasuring();
      break;
    case  MMT_MEMOLAC:
          MemolacMeasuring();
      break;
    case  MMT_VFLOW:
          HandleMMModes(MMT_VFLOW);
      break;
    case  MMT_VP4001:
//#if DEBUG_VP4001
//          if ((Cnt1sec % 5) == 0) {
//            if (meas == 0) {
//              meas = 1;
//              if (isMilkingActive()) {
//                TestMeasureVP4001();
//              } else {
//                TestIdleVP4001();
//              }
//            }
//          } else {
//            meas = 0;
//          }
//#endif
          HandleMMModes(MMT_VP4001);
      break;
    default :
      break;
  }
}


// =============================================================================
// HandleGateK3
// =============================================================================
void HandleGateK3( void)
{
  if ( !(SwingOver() && EEFeeding)){
    if ( (EEParlourType == PT_HERRING_BONE) && EEGateK3Time ) {
      if ( ExtPortCopy & K3 ){
        if ( TimeControl( Cnt10msec, GateK3ActiveTime + (EEGateK3Time * 10))){
          ExtPortK3State = EPK3S_OFF;
        }
      }
    }
  }
}

// =============================================================================
// HandleGateK4
// =============================================================================
void HandleGateK4( void)
{
  if ( (EEParlourType == PT_HERRING_BONE) && EEGateK4Time ) {
    if ( ExtPortCopy & K4 ){
      if ( TimeControl( Cnt10msec, GateK4ActiveTime + (EEGateK4Time * 10))){
        ExtPortK4State = EPK4S_OFF;
      }
    }
  }
}

// =============================================================================
// HandleGateK5
// =============================================================================
void HandleGateK5( void)
{
  if ( (EEParlourType == PT_HERRING_BONE) && EEGateK5Time ) {
    if ( ExtPortCopy & K5 ){
      if ( TimeControl( Cnt10msec, GateK5ActiveTime + (EEGateK5Time * 10))){
        ExtPortK5State = EPK5S_OFF;
      }
    }
  }
}

// =============================================================================
// HandleCowMlkYldAmount
// =============================================================================
void HandleCowMlkYldAmount(void)
{
  if ( CowMlkYldAmount00_15 == 0){
    if ( TimeControl( Cnt1sec, CowMlkYldAttachTime + 15 )){
      CowMlkYldAmount00_15 = (unsigned short)((MlkAmount+5)/10);
      if ( !CowMlkYldAmount00_15) CowMlkYldAmount00_15 = 1;
    }
  }  else {
    if ( CowMlkYldAmount15_30 == 0){
      if ( TimeControl( Cnt1sec, CowMlkYldAttachTime + 30 )){
        if (((MlkAmount+5)/10) > (CowMlkYldAmount00_15+1) ) CowMlkYldAmount15_30 = ((unsigned short)((MlkAmount+5)/10) - CowMlkYldAmount00_15);
        else                                                CowMlkYldAmount15_30 = 1;
      }
    }  else {
      if ( CowMlkYldAmount30_60 == 0){
        if ( TimeControl( Cnt1sec, CowMlkYldAttachTime + 60 )){
          if (((MlkAmount+5)/10) > (CowMlkYldAmount00_15+CowMlkYldAmount15_30+2) ) CowMlkYldAmount30_60 = ((unsigned short)((MlkAmount+5)/10) - CowMlkYldAmount00_15 - CowMlkYldAmount15_30);
          else                                                                     CowMlkYldAmount30_60 = 1;
        }
      } else {
        if ( CowMlkYldAmount60_120 == 0){
          if ( TimeControl( Cnt1sec, CowMlkYldAttachTime + 120 )){
            if (((MlkAmount+5)/10) > (CowMlkYldAmount00_15+CowMlkYldAmount15_30+CowMlkYldAmount30_60+3) ) CowMlkYldAmount60_120 = ((unsigned short)((MlkAmount+5)/10) - CowMlkYldAmount00_15 - CowMlkYldAmount15_30 - CowMlkYldAmount30_60);
            else                                                                                          CowMlkYldAmount60_120 = 1;
          }
        }
      }
    }
  }
}


// =============================================================================
// MainMilking
// =============================================================================
void MainMilking(void)
{
  static unsigned short OldCowNumber, us_LastResultTime;

  if (TestMode == BURN_IN) {
    KeyMode = KM_BURN_IN;
    BurnInProg();
  }
  SwingTime= Cnt10msec;

  // Robot melken ?
  if (EERobot == FALSE) {
    while (1) {

      GetCowRefs();
      SetGateState();
      HndlMeasuring();
      HndlTakeOff();
      HndlConductMeasuring(SideState);
      HndlTempMeasuring();
      PulseControl();
      HandleGateK3();
      HandleGateK4();
      HandleGateK5();

      if (Cnt1sec != Cnt1secOld) {
        Cnt1secOld = Cnt1sec;

        // SwingOver  **********************************************************
        // Set SideState
        if ( SwingOver()                      &&
             MlkState != MLK_CLEANING         &&
             MlkState != MLK_START_CLEANING   &&
             MlkState != MLK_RESTART_CLEANING &&
//             MlkState != MLK_CLEANING_END_I2  &&
             MlkState != MLK_CLEANING_END )    {
          switch (GateState) {
            case GATE_CLOSED:
                 if (SideState != SIDE_A) {
                   // side switch: handel 'oude' zijde af  (SIDE_B)
                   // alleen overnemen als koe ook daadwerkelijk gemolken is
                   if (MlkSessionAvailable[SIDE_B]){
                     MlkResOtherSide = MlkAmountKg;
                     CowMilked[SIDE_B] = GetCowNumber(SIDE_B);
                   }
                   MlkSessionAvailable[SIDE_B] = FALSE;
                   
                   if ( !((MlkState == MLK_DONE) || (MlkState == MLK_WAIT_FOR_START)) ) {
                     HndlMlkRes(p_MlkRef_B[SIDE_B] != NULL ? NORMAL_RES : NORMAL_RES_NO_CLEAR, SEND_ALL_RESULTS, TRUE);

                     // eventueel ook afname initieren
                     MlkAmountKg = 0;
                   } else {
                        // clear results indien volgende koe al klaar staat
                     if (MeasState[SIDE_B] == MEAS_DONE && p_MlkRef_B[SIDE_B] != NULL ) {

                       FreeMlkRef(SIDE_B, FNC_SWING_OVER);
                       FreePointers();              // gooi resterende data ook weg
                       if (p_MlkRef_B[SIDE_B] != NULL) {
                         // pointer wordt gekopieerd, geheugen mag niet worden vrijgegeven
                         p_MlkRef[SIDE_B]   = p_MlkRef_B[SIDE_B];
                         p_MlkRef_B[SIDE_B] = NULL;

                         CowMlkYldStartTime[SIDE_B]   = CowMlkYldStartTime_B[SIDE_B];
                         CowMlkYldManInput[SIDE_B]    = CowMlkYldManInput_B[SIDE_B];
                         CowMlkYldManInput_B[SIDE_B]  = 0;
                         CowMlkYldStartTime_B[SIDE_B] = 0;
                       }
                       if (p_MlkRef[SIDE_B]) MlkRefState[SIDE_B] = COWS_ONE;
                       else                  MlkRefState[SIDE_B] = COWS_NONE;

                       // vraag koe gegevens op voor deze koe
                       SetGetCowRefs(SIDE_A);
                     }
                   }

                   // anders wordt in MLK_INIT p_MlkRef weer NULL gemaakt
                   MeasState[SIDE_B] = MEAS_IDLE;

                   // clear aantal attenties
                   MlkAttention = CowGenInsemDate[SIDE_A] = CowGenHeatDate[SIDE_A] = CowGenDryOffDate[SIDE_A] = 0;
                   ConDataAttentions = CowTempAttention = 0;
                   CowGenReproState[SIDE_A] = 0xff;

                   MlkState  = MLK_WAIT_FOR_START;
                   SideState = SIDE_A;
                   SwingTime = Cnt10msec;
                   ReportGateSwitch(FNC_SWING_GATE, SideState);
                 }
              break;

            case GATE_OPEN:
                 if (SideState != SIDE_B) {
                   // side switch: handel 'oude' zijde af  (SIDE_A)
                   if (MlkSessionAvailable[SIDE_A]){
                     MlkResOtherSide = MlkAmountKg;
                     CowMilked[SIDE_A] = GetCowNumber(SIDE_A);
                   }
                   MlkSessionAvailable[SIDE_A] = FALSE;

                   if ( !((MlkState == MLK_DONE) || (MlkState == MLK_WAIT_FOR_START)) ) {
                        // clear results indien volgende koe al klaar staat
                     HndlMlkRes(p_MlkRef_B[SIDE_A] != NULL ? NORMAL_RES : NORMAL_RES_NO_CLEAR, SEND_ALL_RESULTS, TRUE);

                     // eventueel ook afname initieren
                     MlkAmountKg = 0;


                   } else {
                     // wis oude gegevens alleen indien volgende koe al klaar staat
                     if (MeasState[SIDE_A] == MEAS_DONE && p_MlkRef_B[SIDE_A] != NULL ) {
                       FreeMlkRef(SIDE_A, FNC_SWING_OVER);
                       FreePointers();              // gooi resterende data ook weg
                       if (p_MlkRef_B[SIDE_A] != NULL) {
                         // pointer wordt gekopieerd, geheugen mag niet worden vrijgegeven
                         p_MlkRef[SIDE_A]   = p_MlkRef_B[SIDE_A];
                         p_MlkRef_B[SIDE_A] = NULL;

                         CowMlkYldStartTime[SIDE_A]   = CowMlkYldStartTime_B[SIDE_A];
                         CowMlkYldManInput[SIDE_A]    = CowMlkYldManInput_B[SIDE_A];
                         CowMlkYldManInput_B[SIDE_A]  = 0;
                         CowMlkYldStartTime_B[SIDE_A] = 0;
                       }
                       if (p_MlkRef[SIDE_A]) MlkRefState[SIDE_A] = COWS_ONE;
                       else                  MlkRefState[SIDE_A] = COWS_NONE;

                       // vraag koe gegevens op voor deze koe
                       SetGetCowRefs(SIDE_B);
                     }
                   }

                   // anders wordt in MLK_INIT p_MlkRef weer NULL gemaakt
                   MeasState[SIDE_A] = MEAS_IDLE;

                   // clear aantal attenties
                   MlkAttention = CowGenInsemDate[SIDE_B] = CowGenHeatDate[SIDE_B] = CowGenDryOffDate[SIDE_B] = 0;
                   ConDataAttentions = CowTempAttention = 0;
                   CowGenReproState[SIDE_B] = 0xff;

                   MlkState  = MLK_WAIT_FOR_START;
                   SideState = SIDE_B;
                   SwingTime = Cnt10msec;
                   ReportGateSwitch(FNC_SWING_GATE, SideState);
                 }
              break;
          } // end switch(GateState)
        }   // end swing-over ****************************************************


        // MlkState ------------------------------------------------------------
        switch (MlkState) {
          case MLK_CLEANING_END:                                 // MlkState 23 ---
          break;

          case MLK_START_CLEANING:                               // MlkState 20 ---
            //Stuur eventueel nog aanwezig koedata over
            if (EEAddressCopy != 0) {
              if (MlkCowAvailable(SideState) && MeasState[SideState] == MEAS_MEAS) {
                HndlMlkRes(NORMAL_RES, SEND_ALL_RESULTS, FALSE);
              }
            }
            MlkState = MLK_CLEANING;
          break;

          case MLK_RESTART_CLEANING:                             // MlkState 22 ---
               ul_CleanStartTime = Cnt1sec;
               MlkState = MLK_CLEANING;
          break;

          case MLK_CLEANING :                                    // MlkState 21 ---
            switch(MilkMeterType%100) {
              case MMT_NIVEAU:
              case MMT_VFLOW:
              case MMT_VP4001:
              case MMT_TAKE_OFF_SENSOR:
                   if ( (EEI2Control & 0x07) == 0x01 ){
                     if (Cnt1sec == ul_CleanStartTime + (60*EECleanTime)) {
                       if ( MilkMeterType == MMT_NIVEAU) MlkState = MLK_CLEANING_END;
                       TakeOffState = TO_CLEANING_END;
                       PlsState = PLS_STOP_PULSATION;
                     }
                   } else {
                     if ( MilkMeterType != MMT_NIVEAU) MlkState = MLK_CLEANING_END;              // MlkState 19
                     CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
                   }
              break;

              case MMT_MEMOLAC:
                   if ( (EEI2Control & 0x07) == 0x01 ){
                     if (Cnt1sec == (ul_CleanStartTime +
                             ( CleanPhase == CLEAN_PHASE_PRE_CLEAN) ? (60*EEClnPhs4): (60*(EEClnPhs1 + EEClnPhs2)))) {

                       TakeOffState = TO_CLEANING_END;
                       MlkState = MLK_CLEANING_END;
                     }
                   } else {
                     MlkState = MLK_CLEANING_END;
                     CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
                   }
              break;
            }
          break;
          case MLK_WAIT_FOR_START:                               // MlkState 11 ---
               if ( SwingOver() ){
                 // auto start?
                 if ( EEI1Control == 1){                 // swing-over
                   if ( TimeControl( (unsigned short) Cnt10msec, SwingTime+80) ){
                     SwingTime = Cnt10msec+400;
                     if ( swing_init == 0 ){
                        if (((CowMilked[SideState] != (LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[SideState]->CowGenNumber : ((MlkRef **)p_MlkRef)[SideState]->CowGenNumber)) && p_MlkRef[SideState] != NULL) || (EEAddressCopy == 0) ) {
                          KeyExtSwitch = KEY_AUTO_RELEASE_EXT;  // rename key(EXT) tbv logging
                       }
                     }
                   }
                 }
               }
          break;

          case MLK_INIT :                                        // MlkState  0 ---
            swing_init = 0;                                      // melken mag niet automatisch starten vanuit reinigen
            InitMilkTable();
            // indien geen melkmeter en unico-control dan zet neutraltime op 3 sec.
            ul_MlkNeutralTime   = Cnt1sec + (((EEOutputK2 == UNICO_CONTROL) && (MilkMeterType >= MMT_NO_MILK_METER))? 3 : EENeutralTime1);
            MlkTakeOffThreshold = GetTakeOffTreshold();
            ul_MlkTakeOffTime   = Cnt1sec;

            // Wacht op nieuwe koe
            // Oude koe al gewist, gooi alle andere pointers etc ook weg
            
            // BUG: MlkRes.CowGenNumber == 0:
            // MeasState wordt niet IDLE omdat MlkRes normaal wordt opgestuurd met een "NORMAL_RES_NO_CLEAR"!
            // Tgv een nieuw koe nummer zal p_MlkRef_B != 0 zijn en in MLK_DONE wordt MeasState IDLE.
            // Dit gebeurt niet indien er gemolken wordt met CowGenNumber = 0. p_MlkRef_B blijft nul daardoor
            // blijft MeasState DONE en dus werden de pointer gecleared. CowGenNumber verdwijnt van display
            // zodra melken start en MlkRes bevat CowGenNumber 0.
            if ((MeasState[SideState] == MEAS_DONE) && (CowMilked[SideState] != 0)) {          // CHANGED 17/02/14 
              FreeMlkRef(SideState, FNC_MLK_INIT);
              FreePointers();

              if (p_MlkRef_B[SideState] != NULL) {
                // pointer wordt gekopieerd, geheugen mag niet worden vrijgegeven
                p_MlkRef[SideState]   = p_MlkRef_B[SideState];
                p_MlkRef_B[SideState] = NULL;
                SetGetCowRefs(SideState);                    // version 047
              }
              if (p_MlkRef[SideState]) MlkRefState[SideState] = COWS_ONE;
              else                     MlkRefState[SideState] = COWS_NONE;
            }

            MlkSessionAvailable[SideState] = TRUE;
            MeasState[SideState] = MEAS_IDLE;
            MlkMode              = MM_INIT;
            if ( (MilkMeterType >= MMT_NO_MILK_METER) || (MilkMeterType ==  MMT_TAKE_OFF_SENSOR) ){
              // onderstaande variabelen worden niet meer geinitialiseerd igv. NO_MILK_METER_TYPE
              StoredMlkAmount = 0;
              SendMlkAmount = 0;
              MlkAmount   = 0;
              MlkAmountKg = 0;
              MlkDuration = 0;
              MlkActFlow  = 0;
              MlkMaxFlow  = 0;
              if ( MilkMeterType ==  MMT_TAKE_OFF_SENSOR ){
                FlowState = 1;
                take_off_sensor_ratio = 0;
                take_off_sensor_level = 0;
              }
            }
            MlkState                = MLK_IDLE;
            OldCowNumber            = 0;
            ul_MlkTakeOffTime       = Cnt1sec;
            us_LastResultTime       = Cnt1sec;

            CowMlkYldNrOfAttach2min = 0;     // aantal keren opnieuw onderhangen gedurende de eerste 2 minuten
            CowMlkYldNrOfAttach     = 0;     // totaal aantal keren opnieuw onderhangen
            CowMlkYldManDetach      =  (TakeOffState == TO_MAN) ? 1 : 0;     // manual
            CowMlkYldAmount00_15    = 0;
            CowMlkYldAmount15_30    = 0;
            CowMlkYldAmount30_60    = 0;
            CowMlkYldAmount60_120   = 0;
#if _DEBUG_MESSAGES_
            ReportStateChange();
#endif
          break;

          case MLK_IDLE :                                        // MlkState 1  ---
          	// MMT_VP4001 puts MlkAmounts into MlkTbl when milking is active not before 
            if ((MlkTblIndex != -1 && TakeOffState != TO_IDLE) || (MilkMeterType == MMT_TAKE_OFF_SENSOR) || (MilkMeterType == MMT_VP4001)) {
              ul_MlkStartTime     = Cnt1sec;
              CowMlkYldAttachTime = Cnt1sec;
              ul_MlkKickOffTime   = Cnt1sec + EEKickOffSampleTime;
              MlkKickOffThreshold = EEKickOffThreshold;
              MeasState[GetSideIndex(SideState)] = MEAS_MEAS;
              MlkState = MLK_NEUTRAL;
#if _DEBUG_MESSAGES_
              ReportStateChange();
#endif
            }
          break;

          case MLK_NEUTRAL :                                     // MlkState  2 ---
            // versie 019: het was mogelijk dat de melkgift de gedurende de neutrale tijd
            // niet werd weergegeven ( eerst 'Hand' dan 'Auto' zonder melk-puls )
            MeasState[GetSideIndex(SideState)] = MEAS_MEAS;

            if (Cnt1sec > ul_MlkNeutralTime) {
              // Geiten melkglass: Melkgift < dan minimale waarde
              if ((EEGoatMilkGlass == TRUE) && (MlkAmount < EEMmGoatVal1) && (EEMmGoatVal1 != 0)) {
                if (Cnt1sec >= (ul_MlkStartTime + EENeutralTime1 + EENeutralTime2)) {
                  // Neutraletijd 1 en 2 verstreken, afnemen
                  if ( MilkMeterType < MMT_NO_MILK_METER) MlkAttention = 1;
                  MlkState     = MLK_TAKE_OFF_WAIT;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                } else {
                  // Extra neutrale wachttijd (geiten)
                  ul_MlkNeutralTime = ul_MlkStartTime + EENeutralTime1 + EENeutralTime2;
                }
              } else {
                // FlowState = 1 indien: CalcActMlkFlow() >= EENeutralThreshold
                if (FlowState) {
                  MlkContinue  = 0;
                  MlkState     = MLK_MEASURING;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                } else {
                  if ( MilkMeterType < MMT_NO_MILK_METER) MlkAttention = 1;
                  MlkState     = MLK_TAKE_OFF_WAIT;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                }
              }
            }

            // Init namelk automaat
            StartMilkAutomat = FALSE;
            MlkDurationCur   = MlkDuration + (Cnt1sec - ul_MlkStartTime);
            HandleCowMlkYldAmount();
          break;

          case MLK_MEASURING :                                   // MlkState  3 ---
            if (MilkMeterType == MMT_TAKE_OFF_SENSOR){
              if ( !FlowState ){
                ul_MlkTakeOffTime = GetMlkTakeOffTime();
                MlkState          = MLK_TAKE_OFF_WAIT;
#if _DEBUG_MESSAGES_
                ReportStateChange();
#endif
              }
            } else {
              if ( CalcActMlkFlow() <= MlkTakeOffThreshold ) {
                ul_MlkTakeOffTime = GetMlkTakeOffTime();
                MlkState          = MLK_TAKE_OFF_WAIT;
#if _DEBUG_MESSAGES_
                ReportStateChange();
#endif
              }
            }
            // Check voor na melk automaat
            CheckMilkAutomat();

            MeasState[SideState] = MEAS_MEAS;
            MlkDurationCur       = MlkDuration + (Cnt1sec - ul_MlkStartTime);
            HandleCowMlkYldAmount();
          break;

          case MLK_TAKE_OFF_WAIT:                                // MlkState  4 ---
          case MLK_TAKE_OFF_MAN:                                 // MlkState 10 ---
            if ( ((CalcActMlkFlow() > MlkTakeOffThreshold) && (FlowState)) ||
                 ((MilkMeterType == MMT_TAKE_OFF_SENSOR) && FlowState)    ) {
              MlkAttention = 0;
              MlkState     = MLK_MEASURING;
#if _DEBUG_MESSAGES_
              ReportStateChange();
#endif
              break;
            }
            if ( MilkMeterType >= MMT_NO_MILK_METER ){
              if ( EEOutputK2 == UNICO_CONTROL ){
                // controleer op terugmelding UNICO
                if ( ReadM1Sensor() == SENSOR_STATE_CLOSE ){
                  MlkState     = MLK_TAKE_OFF;
                  TakeOffState = TO_START;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                }
              }
            } else {
              if ((Cnt1sec >= ul_MlkTakeOffTime) || ((!FlowState) && ( MilkMeterType != MMT_TAKE_OFF_SENSOR)) ){
                // Manual TakeOff
                if (TakeOffState == TO_MAN) {
                  MlkState = MLK_TAKE_OFF_MAN;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                } else {
                  MlkState     = MLK_TAKE_OFF;
                  TakeOffState = TO_START;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                }
              }
            }
            MlkDurationCur = MlkDuration + (Cnt1sec - ul_MlkStartTime);
            CheckMilkAutomat();
            HandleCowMlkYldAmount();
          break;

          case MLK_TAKE_OFF:                                     // MlkState  5 ---
            MlkDuration   += (Cnt1sec - ul_MlkStartTime);
            MlkDurationCur = MlkDuration;
//            if (MlkCowAvailable(SideState)) {
            if (EEAddressCopy != 0) {
              if ( EEGoatMilkGlass == TRUE && EmptyGlassAuto == FALSE) {
                MlkState = MLK_WAIT_FOR_SEND;
#if _DEBUG_MESSAGES_
                ReportStateChange();
#endif
              } else {
                MlkSendTime = Cnt1sec + EESendTime;
                MlkState    = MLK_SEND_WAIT;
#if _DEBUG_MESSAGES_
                ReportStateChange();
#endif
              }
            } else {
              // standalone
              MlkState = MLK_DONE;
#if _DEBUG_MESSAGES_
              ReportStateChange();
#endif
            }
            us_LastResultTime = Cnt1sec;
          break;

          case MLK_KICKED_OFF :                                  // MlkState 12
          break;

          // Handmatig oversturen melkdata bij monsteren geiten met CLEAN toets of
          // hekschakelaar sluiten
          case MLK_WAIT_FOR_SEND:                                // MlkState 13 ---
            if (GateState == GATE_OPEN) {
              MlkSepStandAlone = 0;
              // Melkglas leeg laten lopen
              if (EEOutputP1Inv == TRUE) {
                if (PORTA & P_3) ExtOutputP1State = P1_ON;
                else             ExtOutputP1State = P1_START_MAN;
              } else {
                if (PORTA & P_3) ExtOutputP1State = P1_START_MAN;
                else             ExtOutputP1State = P1_ON;
              }
              MlkState = MLK_SEND_MAN;
#if _DEBUG_MESSAGES_
              ReportStateChange();
#endif
            }
          break;

          case MLK_SEND_MAN:                                     // MlkState 14 ---
               MlkSendTime = Cnt1sec;
               MlkState    = MLK_SEND_WAIT;
#if _DEBUG_MESSAGES_
               ReportStateChange();
#endif
            break;


          case MLK_SEND_WAIT:                                    // MlkState  6 ---
            if ( (ExtPortCopy & K3 ) && (ParlourType == PT_CARROUSEL) ){
              MlkSendTime = Cnt1sec + EESendTime;
            }

            // disable feeding
            FEEDING_ENABLE[SideState] = FALSE;

            if (Cnt1sec >= MlkSendTime) {
              if (EEAddressCopy != 0) {
                if (MlkCowAvailable(SideState)) {
                  // check minimum milk-yield attention
                  if ( (SoftKey & SK_STIM_TRCK_ARM_ENABLE) && (EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD) ){
                    if ( GetCowGls() & CG_MINYIELD ){
                      //HandleKeySave(long Value, unsigned char SMode, unsigned char SNumber, unsigned char EItem, unsigned char idx, unsigned char val2 )
                      HandleKeySave(EESepMinYldFuncNr ,SM_COW_INFO, SCREEN_COW_INFO_1, 1, SideState, 1);
                      SepMsgSend = TRUE;
                    }
                  }
                  CowMilked[SideState] = GetCowNumber(SideState);
                } else {
                  CowMilked[SideState] = 0;
                }
                HndlMlkRes(NORMAL_RES_NO_CLEAR, SEND_ALL_RESULTS, TRUE);
                SendStartEndWeight(SideState);
                MlkState             = MLK_DONE;
                MeasState[SideState] = MEAS_DONE;
#if _DEBUG_MESSAGES_
                ReportStateChange();
#endif
              }
            }
          break;

          // aanroep alleen bij robot melken
          case MLK_SEND:                                         // MlkState  7 ---
            HndlMlkRes(NORMAL_RES, SEND_ALL_RESULTS, FALSE);
            MlkState = MLK_INIT;
          break;

          case MLK_CONTINUE :                                    // MlkState  9 ---
            // enable feeding
            if ( EEAddressCopy )FEEDING_ENABLE[SideState] = TRUE;

            // Registreer het aantal keren onderhangen
            if ( (Cnt1sec - CowMlkYldAttachTime) <= 120 )CowMlkYldNrOfAttach2min++;
            CowMlkYldNrOfAttach++;

            if (MeasState[SideState] != MEAS_MEAS) {
              // gegevens zijn al opgestuurd, vraag opnieuw op
              while(HndlMlkRefReq(0, COWNULL_REF, SideState) ==0)TaskExit();
            }

            if ( (SoftKey & SK_STIM_TRCK_ARM_ENABLE) && (EESepMsgCntrl & SEND_SEP_MSG_MIN_YLD) && (SepMsgSend == TRUE)){
              if ( GetCowGls() & CG_MINYIELD ){
                // zet het aantal separaties tgv minimum milk yield weer op nul
                HandleKeySave(EESepMinYldFuncNr ,SM_COW_INFO, SCREEN_COW_INFO_1, 1, SideState, 0);
                SepMsgSend = FALSE;
              }
            }

            MlkAttention         = 0;
            ul_MlkNeutralTime    = Cnt1sec + (EEOutputK2 == UNICO_CONTROL ? 3 : EENeutralTime2);
            MlkState             = MLK_NEUTRAL;
            MeasState[SideState] = MEAS_MEAS;
            ul_MlkStartTime      = Cnt1sec;
#if _DEBUG_MESSAGES_
            ReportStateChange();
#endif
          break;

          case MLK_DONE :                                        // MlkState  8 ---
            if ( EECleanStartTime != 0 ){
              if ( TimeControl( (unsigned short) Cnt1sec, us_LastResultTime+(EECleanStartTime * 60)) ){
                if ( (ParlourType >= PT_SEMI_TANDEM)  &&
                     (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR)  ){
                  // Indien SLAVE wacht op bericht van de MASTER om in reinigen te gaan
                  if ( EERowSize && ( EEAddress == OTRowMaster) && !(ExternMessages & SEND_CLEAN_STOP_MESSAGES) ){
                    // OpenTandem ROW_MASTER, zet alle MPC's in CLEANING
                    ExternMessages |= SEND_CLEAN_START_MESSAGES;
                  }
                } else {
                  MlkState          = MLK_START_CLEANING;
                  KeyMode           = KM_CLEANING;
                  TakeOffState      = TO_START_CLEAN;
                  ScreenNumber      = SCREEN_RESET;
                  ul_CleanStartTime = Cnt1sec;
                  DispFlash         = 0;
#if _DEBUG_MESSAGES_
                  ReportStateChange();
#endif
                }
              }
            }

            //Wacht op nieuwe koe
            //Is er een koe aanwezig
            if (OldCowNumber == 0) {
              if (p_MlkRef[SideState] != NULL) {
                OldCowNumber = LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[SideState]->CowGenNumber : (long)((MlkRef **)p_MlkRef)[SideState]->CowGenNumber;
              }
            }

            if ((OldCowNumber != (LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[SideState]->CowGenNumber : (long)((MlkRef **)p_MlkRef)[SideState]->CowGenNumber)) && p_MlkRef[SideState] != NULL ) {
              MeasState[SideState] = MEAS_IDLE;
              OldCowNumber = LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[SideState]->CowGenNumber : (long)((MlkRef **)p_MlkRef)[SideState]->CowGenNumber;

              // clear old data
              InitMilkTable();
              MlkAmountKg = 0;
              MlkAmount = 0;

              // Test of cluster weer opgehaald moet worden
              if ( EEClusterDown && (!(ExtPortCopy & K1)) ){
                if ( EEClusterDownDelayHigh) ClusterState = CLUSTER_DOWN_HIGH;
              }
            }
            // first time attach look at 'CowGenManualRelease'
            // Attaching the same cow requiers 'shift' key
            FirstAttach = 0;

            //Nieuwe koe automatisch geidentificeerd
            if (p_MlkRef_B[SideState] != NULL) {
              FreeMlkRef(SideState, FNC_MLK_DONE);
              FreePointers();      // gooi resterende data ook weg

              p_MlkRef[SideState]   = p_MlkRef_B[SideState];
              p_MlkRef_B[SideState] = NULL;

              CowMlkYldStartTime[SideState]   = CowMlkYldStartTime_B[SideState];
              CowMlkYldManInput[SideState]    = CowMlkYldManInput_B[SideState];
              CowMlkYldManInput_B[SideState]  = 0;
              CowMlkYldStartTime_B[SideState] = 0;

              MlkRefState[SideState] = COWS_ONE;
              SetGetCowRefs(SideState);
              MeasState[SideState] = MEAS_IDLE;

              // Test of cluster weer opgehaald moet worden
              if ( EEClusterDown && (!(ExtPortCopy & K1)) ){
                if ( EEClusterDownDelayHigh) ClusterState = CLUSTER_DOWN_HIGH;
              }
            }
          break;

          default :
            break;

        } // end switch ( MilkState ) ------------------------------------------
      }  // end if ( count == 1sec ) -------------------------------------------
      TaskExit();
    } // end while (1) ---------------------------------------------------------
  } else {
      // *************************************************************************
      // ROBOT MILKING ***********************************************************
      // *************************************************************************
    while (1) {

      GetCowRefs();
      SetGateState();
      HndlMeasuring();
      HndlTakeOff();
      HndlConductMeasuring(SideState);
      HndlTempMeasuring();
      PulseControl();

      if (Cnt1sec != Cnt1secOld) {
        Cnt1secOld = Cnt1sec;

        switch(MlkState) {

          case MLK_INIT :                                        // STATE  0 ---
            InitMilkTable();
            us_LastResultTime = Cnt1sec;

            // Wacht op nieuwe koe
            // Oude koe al gewist
            if (MeasState[SideState] == MEAS_DONE) {
              free(p_MlkRef[SideState]);
              p_MlkRef[SideState] = NULL;
              FreePointers();                   // gooi resterende data ook weg
              if (p_MlkRef_B[SideState] != NULL) {
                p_MlkRef[SideState] = p_MlkRef_B[SideState];
                p_MlkRef_B[SideState] = NULL;
              }
              if (p_MlkRef[SideState]) {
                MlkRefState[SideState] = COWS_ONE;
              } else {
                MlkRefState[SideState] = COWS_NONE;
              }
            }
            MeasState[SideState] = MEAS_IDLE;
            MlkMode  = MM_INIT;
            MlkState = MLK_IDLE;
            OldCowNumber = 0;
          break;

          case MLK_IDLE :                                        // STATE  1 ---
            if ((MlkTblIndex != -1)) {
              ul_MlkStartTime = Cnt1sec;
              MeasState[GetSideIndex(SideState)] = MEAS_MEAS;
              MlkState = MLK_MEASURING;
            }
          break;

          case MLK_MEASURING :                                   // STATE  3 ---
            MeasState[SideState] = MEAS_MEAS;
            MlkDurationCur = MlkDuration + (Cnt1sec - ul_MlkStartTime);
          break;

          case MLK_SEND:                                         // STATE  7 ---
            MlkDuration = MlkDurationCur;
            if (EEAddressCopy != 0) {
              MeasState[SideState] = MEAS_DONE;
              // also send mlkamout when no cow number
              HndlMlkRes(NORMAL_RES, SEND_ALL_RESULTS, TRUE);
              MlkState = MLK_DONE;
              us_LastResultTime = Cnt1sec;
            }
          break;

          case MLK_DONE:                                         // STATE  8 ---
            if (Cnt1sec > (us_LastResultTime + (EECleanStartTime * 60))) {
              MlkState          = MLK_START_CLEANING;
              KeyMode           = KM_CLEANING;
              TakeOffState      = TO_START_CLEAN;
              ScreenNumber      = SCREEN_RESET;
              ul_CleanStartTime = Cnt1sec;
              DispFlash = 0;
            }

            MeasState[SideState] = MEAS_IDLE;
          break;

          case MLK_CLEANING_END:                                 // STATE 18 ---
          break;

          case MLK_START_CLEANING:                               // STATE 10 ---
            //Stuur eventueel nog aanwezig koedata over
            if (EEAddressCopy != 0) {
//              if (MlkCowAvailable(SideState)) {
                HndlMlkRes(NORMAL_RES, SEND_ALL_RESULTS, FALSE);
//              }
            }
            MlkState = MLK_CLEANING;
          break;

          case MLK_CLEANING :                                    // STATE 16 ---
            switch(MilkMeterType%100) {
              case MMT_NIVEAU:
                if (Cnt1sec == ul_CleanStartTime + (60*EECleanTime)) {
                  TakeOffState = TO_CLEANING_END;
                  MlkState = MLK_CLEANING_END;
                }
              break;

              case MMT_VFLOW:
              case MMT_VP4001:
              case MMT_MEMOLAC:
                if (Cnt1sec == ul_CleanStartTime + (60*(EEClnPhs1 + EEClnPhs2))) {
                  TakeOffState = TO_CLEANING_END;
                  MlkState = MLK_CLEANING_END;
                }
              break;
            }
          break;
        }
      }
    }
  }
}

