// =============================================================================
// SURGE Milkmeter                                              
// =============================================================================

#include "surge.h" 
//#include "uart_dri.h" 
//#include "st16c1450.h" 
#include "../appl/takeoff.h" 
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"                             
#include "../lib/convert.h"             
#include "../lib/intr6811.h" 
#include "../lib/schedule.h"
#include "../key/key.h" 
#include "../key/keyint.h" 


 
#pragma codeseg(EEPAGE) 

SURGE_MILKMETER_INFO  MilkMeter;
unsigned short   StartTimeTakeOffInt, TempMlkAmount;
unsigned char    TakeOffInt, MlkControl;                                   

// =============================================================================
// SURGE TAKE OFF INTERRUPT ROUTINES                          
// =============================================================================
void HndlSurgeTakeOffInt(void) 
{
  // Disable input capture 1 interrupt -----------------------------------------                     
  TMSK1 &= 0xff - IC1I;
  TFLG1 = IC1F;      
   
  StartTimeTakeOffInt = Cnt10msec;
  TakeOffInt = TRUE;                                   
                                      
} 
   
// =============================================================================
// ENABLE TAKE OFF INTERRUPT ROUTINES                           
// =============================================================================
void EnableTakeOffInt(void) 
{
  // Enable input capture 1 interrupt ------------------------------------------
  TFLG1 = IC1F;

  // Interrupt on falling ------------------------------------------------------
  TCTL2 &= 0xff - EDG1A - EDG1B;
  TCTL2 |= EDG1B;       

  TMSK1 |= IC1I;     
  TakeOffInt = FALSE; 
}    
   
// =============================================================================
// HANDLE TAKE INT.                                            
// =============================================================================
void HndlTakeOffInt(void) 
{
  // Was there a take off int --------------------------------------------------
  if (TakeOffInt == TRUE) {
    if (PORTA & P_2) {                      
      // signal 200 msec high --------------------------------------------------
      if (Cnt10msec > StartTimeTakeOffInt+20) {
        // Start Take Off ------------------------------------------------------ 
        MlkState = MLK_TAKE_OFF; 
      }  
    } else {
      EnableTakeOffInt();
    }
  }    
}


// =============================================================================
// SURGE VALVE ON                                          
// =============================================================================
void SurgeValveOn(void) 
{
  // nog doen ------------------------------------------------------------------
//  LCR |= LcrSB;
}

// =============================================================================
// SURGE VALVE OFF                                          
// =============================================================================
void SurgeValveOff(void) 
{
  // nog doen ------------------------------------------------------------------
//  LCR &= 0xff - LcrSB;
}

    
// =============================================================================
// SURGE VALVE HANDLING                                      
// =============================================================================
void HndlSurgeValve(void) 
{
  switch (ValveState) {
    case VALVE_ON: 
      if (Cnt10msec > (ValveStartTime + VALVE_TIME)) {
        SurgeValveOff();
        ValveState = VALVE_OFF;
      }
    break; 
    
    case VALVE_START: 
      // Activate milkmeter valve ----------------------------------------------
      SurgeValveOn();
      ValveStartTime = Cnt10msec;
      ValveState = VALVE_ON;
    break;  
    case VALVE_RELEASE: 
      //de-Activate milkmeter valve --------------------------------------------
      SurgeValveOff();
      ValveState = VALVE_OFF;
       
    default:
    break; 
  }    
}

// =============================================================================
// CHECK ALARM                                             
// =============================================================================
unsigned char CheckAlarm(void) 
{
  unsigned char Alarm, idx;
  unsigned short ExpMilkYield;

  Alarm = 42;             

  if (MlkCowAvailable()) {
    idx = GetSideIndex(SideState);

    // Zend alarm status voor melken alleen voordat melken begint --------------
    if ((MilkMeter.MilkmeterCode == METER_WITH_NO_DATA) &&
       (MilkMeter.CumMilkYield == 0)) { 
        //melkseparatie, Biest, Droog  
      if (p_MlkRef[idx]->CowMlkNrOfSeps|| p_MlkRef[idx]->CowGenColost
        || p_MlkRef[idx]->CowGenDryOffDate){
        // Led knipperen -------------------------------------------------------
        Alarm = ALARM_FLASH;
      }
    } else {
      ExpMilkYield = p_MlkRef[idx]->CowMlkExpMinAmount / 10;
      // Melkgift < Verwachte melkgift -----------------------------------------
      if ((MilkMeter.MilkmeterCode == METER_WITH_NO_DATA) &&
          (MilkMeter.ResetFlag == 1) &&
          (MilkMeter.CumMilkYield < ExpMilkYield)){
        Alarm = LOW_PRODUCTION;
      }  
    }
  }        
  return(Alarm);
}           
         
         
 
// =============================================================================
// SEND SERIAL NUMBER TO MILKMETER                           
// =============================================================================
void SendSerialNumber(void) 
{
  SERIAL_NUMBER SerialNumberMsg;
 
//  STInit();
  
  SerialNumberMsg.SerialCode = SERIAL_CODE;
  SerialNumberMsg.RelAddress = MILK_METER_ADDRESS;
  SerialNumberMsg.NA         = 0;
  SerialNumberMsg.SerialNumber = EESerialNr;
                                                  
//  SendMsg(MILK_METER_ADDRESS, sizeof(SERIAL_NUMBER), (const unsigned char *)&SerialNumberMsg);       
     
}

// =============================================================================
// SEND DETACH DELAY TIME                                    
// =============================================================================
void SendDetachDelayTime(void) 
{
  static unsigned short LastSendTime=0;
  unsigned char DhiaModeNew, idx;
  DETACH_DHIA DetachDelayMsg;
        
  DhiaModeNew = 0;
         
  // Do not take off in neutral time or in manual takeoff state ----------------
  if (MlkState == MLK_NEUTRAL  || TakeOffState == TO_MAN ) { 
    DhiaModeNew = 6;
  } else {
    // Vindt er melkcontrole plaats --------------------------------------------
    if (MlkCowAvailable()) {
      idx = GetSideIndex(SideState);
      DhiaModeNew = !(p_MlkRef[idx]->HerdMlkEmptyGlassAuto);
    }  
  }  

  // Moet dhiamode string overgestuurd worden ----------------------------------
  if (Cnt1sec > LastSendTime || MilkMeter.DHIAStatus != DhiaModeNew) {
    MilkMeter.DHIAStatus = DhiaModeNew;
              
    LastSendTime = Cnt1sec + 5;  
                     
    DetachDelayMsg.DetachCode  = DETACH_CODE;
    DetachDelayMsg.NA          = 0;
    DetachDelayMsg.DhiaMode    = MilkMeter.DHIAStatus;
    DetachDelayMsg.DetachDelay = MilkMeter.DelayTime;  
                                                  
//    SendMsg(BROADCAST_MSG, sizeof(DETACH_DHIA), (const unsigned char *)&DetachDelayMsg);
  }     
}
                          
                         
// =============================================================================
// SEND COW NUMBER / ALARM                                
// =============================================================================
void SendCowNumberAlarm(void) 
{
  COW_ALARM CowAlarmMsg;  
  unsigned char Alarm, idx;
  static unsigned short LastSendTime = 0;

  if (Cnt1sec > LastSendTime) {
  
  LastSendTime = Cnt1sec + 2;
  
    Alarm = CheckAlarm();

    if (MlkCowAvailable()) {
      idx = GetSideIndex(SideState);
      MilkMeter.CowNumber = (p_MlkRef[idx]->CowGenNumber);
    } else {
      MilkMeter.CowNumber = 0;
    }
      
    if (Alarm != MilkMeter.AlarmStatus || MilkMeter.CowNumber != MilkMeter.PrevCowNumber ) {
      MilkMeter.AlarmStatus = Alarm;
      CowAlarmMsg.SendData = COW_ALARM_CODE;
      CowAlarmMsg.RelAddress = MilkMeter.RelAddress;
      CowAlarmMsg.CowNumber = MilkMeter.CowNumber;
      CowAlarmMsg.AlarmCode = MilkMeter.AlarmStatus;

//      SendMsg(MilkMeter.RelAddress, sizeof(COW_ALARM), (const unsigned char *)&CowAlarmMsg);
    }
  }  
} 

// =============================================================================
// SEND CLEAR METER-PRODUCTION TO MILKMETER                
// =============================================================================
void ClearMeterProduction(void) 
{
  CLEAR_METER ClearMeterMsg;                     
   
  typedef struct {
    unsigned char  ClearMilk;    // Clear Meter Production code
    unsigned char  RelAddress;   // Relative Address
  } CLEAR_METER;
 
  ClearMeterMsg.ClearMilk  = CLEAR_CODE;
  ClearMeterMsg.RelAddress = MilkMeter.RelAddress;
  if (ClearMeterMsg.RelAddress != 0) {
//    SendMsg(MilkMeter.RelAddress, sizeof(CLEAR_METER), (const unsigned char *)&ClearMeterMsg);
  }     
}

 
 
// =============================================================================
// POLL MILKMETER                                               
// =============================================================================
void PollMilkMeter(void) 
{
  static unsigned short Cnt1secLast;
  POLL_METER PollMeterMsg;

  if (Cnt1sec != Cnt1secLast) { 
      Cnt1secLast = Cnt1sec;      
 
    if (MilkMeter.RelAddress != MILK_METER_ADDRESS)  { 
      SendSerialNumber();
    } else { 
      if (MilkMeter.RelAddress == MILK_METER_ADDRESS) {
        PollMeterMsg.PollCode      = POLL_CODE;
        PollMeterMsg.RelAddress    = MILK_METER_ADDRESS;
                                                   
//       SendMsg(MILK_METER_ADDRESS, sizeof(POLL_METER), (const unsigned char *)&PollMeterMsg);
      }
      SendDetachDelayTime() ; 
      SendCowNumberAlarm(); 
  
    }  
  }
}
  
// =============================================================================
// Start AD-converter                                       
// =============================================================================
void StartADSurge(void) 
{ 
  ADCTL &= 0xff - CC;
}                 

// =============================================================================
// Stop  AD-converter                                       
// =============================================================================
void StopADSurge(void) 
{ 
  ADCTL |= CC;
}                 

// =============================================================================
// Calculate temp. milkyield                                
// =============================================================================
void CalcTempMilk(void) 
{ 
  static unsigned char ADValue[8], i=0, Dump = FALSE;
  unsigned short ADTot;
  unsigned char j;
  static unsigned short ADMax = 0;
  
  if (i >7) i = 0;
  
  // Bepaal ad-waarde ----------------------------------------------------------
  ADValue[i] = ADR1;
  i++;
  
  ADTot = 0; 
  // Bereken gemiddelde over de laatste acht waarden ---------------------------
  for (j=0; j<8; j++) {
    ADTot += ADValue[j];
  }
                        
  ADTot /=8;
  
  if (ADTot >ADMax) {
    ADMax = ADTot; 
  }  
    
    
  // Melk dump gesignaleerd ----------------------------------------------------
  if ((ADTot > 125 ) && (Dump == FALSE)) {
    if (TempMlkAmount == 0) {
      TempMlkAmount = 300;
    } else {
      TempMlkAmount += 450;
    }  
    Dump = TRUE;
  } else {
    if (ADTot <40) {
      Dump = FALSE;
    }  
  }  
}                 
                  
   
// =============================================================================
// HANDLE SURGE MILKMETER MESSAGES                          
// =============================================================================
void HndlSurgeMsg(void) 
{
  RESULT_METER ResultMsg;
  static unsigned short CheckTime;

   
  ResultMsg.ResultCode = 0; 
//  if (STReceive(MILK_METER_ADDRESS, (unsigned char*)&ResultMsg) != 0) {
    switch (ResultMsg.ResultCode) {
      case METER_WITH_DATA:     
        CheckTime = Cnt1sec;
        MilkMeter.RelAddress    = ResultMsg.RelAddress;
        MilkMeter.MilkmeterCode = ResultMsg.ResultCode;
        MilkMeter.MilkYield     = ResultMsg.MilkYield;
        MilkMeter.PrevCowNumber = ResultMsg.CowNumber;
        MilkMeter.ResetFlag     = ResultMsg.ResetFlag;
      break;
      case METER_WITH_NO_DATA:
        CheckTime = Cnt1sec;
        MilkMeter.RelAddress    = ResultMsg.RelAddress;
        MilkMeter.MilkmeterCode = ResultMsg.ResultCode;
      break;
      default:
      break;
    }
//  }
  // Vijf seconden niets ontvangen, zend serienummer ---------------------------
  if (Cnt1sec > (CheckTime + 5)) {
    MilkMeter.RelAddress = !(MILK_METER_ADDRESS);
    CheckTime = Cnt1sec;
  }                               
}

 
// =============================================================================
// SURGE MEASURING                                          
// =============================================================================
void SurgeMeasuring(void) 
{
  static unsigned short StartTime, StartCleanPhase;
  static unsigned char Init = FALSE, ResetFlag;
                 
  PollMilkMeter();                
    
  switch (MlkState) {
    case MLK_CLEANING:
    case MLK_CLEANING_END:       
      if (Init == FALSE) {
        MlkMode = MM_INIT;
        // Start Reinigen ------------------------------------------------------
        CleanPhase = CLEAN_PHASE_0;
      } else {
        MlkMode = MM_CLEANING;
      }  
    break; 

    case MLK_WAIT_FOR_START:
      if (MlkMode == MM_CLEANING) {
        MlkMode = MM_INIT;
      }
    default:
      CleanPhase = CLEAN_PHASE_0;
    break;
  }

  switch (MlkMode) {
    case MM_INIT    : 
     
      // Init Surge milk measuring ---------------------------------------------
      MlkAmount = 0;
      MlkAmountKg = 0;
      TempMlkAmount = 0;
      MlkDuration = 0;
      MlkActFlow = 0;
      MlkMaxFlow = 0;
      ResetFlag = 1;
      MlkMode = MM_MEAS;
      Init = TRUE; 
      MilkMeter.DelayTime = 590 * abs(EETakeOffTime); 
      MilkMeter.MilkYield = 0; 
      MilkMeter.CumMilkYield = 0;
      ClearMeterProduction();
 
      
      // Init TakeOff Interrupt ------------------------------------------------
      EnableTakeOffInt();
      
    break; 
     
    case MM_MEAS:
      switch(MlkState) {
      
        // Milking?? -----------------------------------------------------------
        case MLK_IDLE:
        case MLK_NEUTRAL:
        case MLK_MEASURING:
        case MLK_TAKE_OFF_WAIT:
        case MLK_TAKE_OFF_MAN:
        case MLK_SEND_WAIT: 
        case MLK_DONE:
          StartADSurge(); 

          // save milk yield every second, to determine milk flow --------------
          if (Cnt1sec != Cnt1secLast) {
            PutMlkTbl(TempMlkAmount,100);

            // bewaar ook het verloop van de melkgift iedere 5 sec. ------------    
            if ( (Cnt1sec % 5) == 0 ) PutMlkAmountInTable((unsigned short)TempMlkAmount);

            Cnt1secLast = Cnt1sec;
          }  
          
          // Milk yield calculation: -------------------------------------------
          if (MilkMeter.ResetFlag == 0) {
            ResetFlag = 0;
            MlkAmountKg = TempMlkAmount/100; 
            
            // Indien melkgift nog niet is ontvangen in dhiamode, dan
            // mag de melkgift niet overgestuurd worden ------------------------
            if  (MilkMeter.DHIAStatus != 0){
              MlkSendTime = Cnt1sec + EESendTime;
            } 
             
          } else {
            if (ResetFlag == 0) {
              MlkAmount += MilkMeter.MilkYield*100;
              MilkMeter.CumMilkYield += MilkMeter.MilkYield; 
              TempMlkAmount = MlkAmount;
              MlkAmountKg = (MlkAmount+50)/100;
              ClearMeterProduction();

              // Melkgift direct oversturen in DHIA mode -----------------------
              if  (MilkMeter.DHIAStatus != 0){
                MlkSendTime = Cnt1sec;
              } 

              ResetFlag = 1;
            }
          }    
          
          HndlTakeOffInt();
          // Calculate temp. milkyield -----------------------------------------
          CalcTempMilk();

          StopADSurge(); 
        break;
         
        default:
        break;
      }
     
    break;

    case MM_CLEANING:
      switch (CleanPhase) { 
      
        case CLEAN_PHASE_0:
          StartCleanPhase = Cnt1sec;
          ValveState = VALVE_START;
          StartTime = Cnt1sec;
          CleanPhase = CLEAN_PHASE_1;
        break;
          
        case CLEAN_PHASE_1:  
          // Reinigingsfase  verstreken ----------------------------------------
          if (Cnt1sec > (StartCleanPhase + (60 * EEClnPhs1))) {
            CleanPhase = CLEAN_PHASE_END;
          } else{
            if (Cnt1sec > (StartTime + EEClnPhs2)) {
              ValveState = VALVE_START;
              StartTime = Cnt1sec;
            }
            HndlSurgeValve();
          }    
      
        break;
           
        case CLEAN_PHASE_END:
          ValveState = VALVE_RELEASE;
          HndlSurgeValve();
        break;  
      }    
    break;
    
    default :
    break;
  }
}


