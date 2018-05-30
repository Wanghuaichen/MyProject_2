/*****************************************************************/
/* Memolac Milkmeter                                            */
/****************************************************************/

#include "manuflow.h"
#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h" 
#include "../lib/intr6811.h"
#include "../key/key.h" 
#include "../key/keyint.h"                                
#include "../appl/takeoff.h" 
#include "../appl/milk.h"
 
#pragma codeseg(EEPAGE) 

 
unsigned char SensorStateMan, MilkFlowCorMan, NrOfReedPulsesMan;  
unsigned short TimeLastFlowCalcMan;
unsigned long MilkAmountLastFlowCalcMan;
                                                               
/****************************************************************/
/*MANUFLOW KLEP besturings ROUTINES                             */
/****************************************************************/
void PortP1ManOn(void) {
  
  //Inverteren Poort uitgang P1, melkmeter klep 
  if (EEOutputP1Inv == FALSE) {
    PortP1On();
  } else {
    PortP1Off();
  }                                          
}                                         

void PortP1ManOff(void) {
  
  //Inverteren Poort uitgang P1, melkmeter klep 
  if (EEOutputP1Inv == FALSE) {
    PortP1Off();
  } else {
    PortP1On();
  }                                          
}                                         


/****************************************************************/
/*MANUFLOW MEASURING INTERRUPT ROUTINES                         */
/****************************************************************/
void HndlManuflowM1Int(void) {
                       
  //Disable input capture 3 interrupt                      
  TMSK1 &= 0xff - IC3I;
  TFLG1 = IC3F;
                                     
  SensorStateMan |= REED_SENSOR;
                                   
  //Activate milkmeter valve
  PortP1ManOn();
  ValveStartTime = Cnt10msec;
  ValveState = VALVE_ON;
                                      
}


/****************************************************************/
/*MANUFLOW VALVE HANDLING                                       */
/****************************************************************/
void HndlValveMan(void) {
    
  switch (ValveState) {
    case VALVE_ON: 
      if (Cnt10msec > (ValveStartTime + (EEValveTimeMan/10))) {
        //Test reed contact
        //reedcontact normally open
        if (PORTA & P_0) {
          //reedcontact closed, vlotter gezakt
          
          PortP1ManOff();
          ValveState = VALVE_OFF;
          TFLG1 = IC3F;
          TMSK1 |= IC3I;         
        } else {
          //reedcontact open, vlotter niet gezakt
        }
      }
    break; 
    
    case VALVE_START: 
      //Activate milkmeter valve
      PortP1ManOn();
      ValveStartTime = Cnt10msec;
      ValveState = VALVE_ON;
    break;  
    case VALVE_RELEASE: 
      //de-Activate milkmeter valve
      PortP1ManOff();
      ValveState = VALVE_OFF;
    break;
       
    case VALVE_CONT_ON: 
      //Activate milkmeter valve continiously
      PortP1ManOn();
    break;
       
    default:
    break; 
  }    
}
 
 
 
/****************************************************************/
/*Bereken Flow Correctie factor                                 */
/****************************************************************/
void CalcFlowCorMan(void) { 
unsigned long ActFlowGr;
unsigned char ActFlowKg; 
  
  
     
  ActFlowGr = ((MlkAmount - MilkAmountLastFlowCalcMan)*120)/ (Cnt500msec -TimeLastFlowCalcMan);
  ActFlowKg = ActFlowGr/1000;                                       
  if (ActFlowKg > 12) ActFlowKg = 12; 
  
  MilkAmountLastFlowCalcMan = MlkAmount;
  TimeLastFlowCalcMan = Cnt500msec;                
 
  switch (ActFlowKg) {
    case 0:
      MilkFlowCorMan = EECorFlow0Man;
    break;
    case 1:
      MilkFlowCorMan = EECorFlow1Man;
    break;
    case 2:
      MilkFlowCorMan = EECorFlow2Man;
    break;
    case 3:
      MilkFlowCorMan = EECorFlow3Man;
    break;
    case 4:             
      MilkFlowCorMan = EECorFlow4Man;
    break;
    case 5:
      MilkFlowCorMan = EECorFlow5Man;
    break;
    case 6:
      MilkFlowCorMan = EECorFlow6Man;
    break;
    case 7:                   
      MilkFlowCorMan = EECorFlow7Man;
    break;
    case 8:
      MilkFlowCorMan = EECorFlow8Man;
    break;
    case 9:
      MilkFlowCorMan = EECorFlow9Man;
    break;
    case 10:
      MilkFlowCorMan = EECorFlow10Man;
    break;
    case 11:
      MilkFlowCorMan = EECorFlow11Man;
    break;
    case 12:
      MilkFlowCorMan = EECorFlow12Man;
    break;                  
  }
}

/****************************************************************/
/*Bereken Melk gift                                             */
/****************************************************************/
unsigned short CalcMilkYieldMan(void)
{
unsigned short MilkAdd;   
 
  MilkAdd = 0;

  if (MlkMode == MM_DHIA) {
    if (SensorStateMan & REED_SENSOR) {
      MilkAdd += EEMmWaterTestMan;          
      disable_interrupt();
      SensorStateMan &= 0xff - REED_SENSOR;
      enable_interrupt();
    }  
  } else {
  
    if (SensorStateMan & REED_SENSOR) {
      MilkAdd +=  (EEMmOffsetMan + MilkFlowCorMan);
      disable_interrupt();
      SensorStateMan &= 0xff - REED_SENSOR;
      enable_interrupt();
      NrOfReedPulsesMan++;
    }
    //na elke puls melkstroom bepalen
    if (NrOfReedPulsesMan >= 1) {
      CalcFlowCorMan();
      NrOfReedPulsesMan = 0;
    }
  }  
   
  return(MilkAdd);
}

/****************************************************************/
/*MANUFLOW MEASURING                                            */
/****************************************************************/

void ManuflowMeasuring(void) {
static unsigned short Cnt1secLast;
static unsigned short StartCleanPhase, StartCleaning;
static unsigned char Init = FALSE, LastMilkCalc;


  switch (MlkState) {
    case MLK_START_CLEANING:
    case MLK_CLEANING:
    case MLK_CLEANING_END:       
      if (Init == FALSE) {
        MlkMode = MM_INIT;
        CleanPhase = CLEAN_PHASE_START;
      } else {
        MlkMode = MM_CLEANING;
      }  
    break;
    case MLK_DHIA:
      //Reset melkgift
      if ((KeyA == KEY_CLEAR && ScreenNumber == SCREEN_RESET) || MlkMode ==MM_CLEANING) {
        MlkMode = MM_INIT;
      } else {                  
        MlkMode = MM_DHIA;
      }
    break;
    case MLK_WAIT_FOR_START:
      if (MlkMode == MM_CLEANING) {
        MlkMode = MM_INIT; 
        CleanPhase = CLEAN_PHASE_START;
      } 
    break;   
      
    case MLK_SERVICE:
      MlkMode = MM_SERVICE;
    break;
      
    default:
      CleanPhase = CLEAN_PHASE_START;
    break;
  }

   
  switch (MlkMode) {
    case MM_INIT    :
      //Init manuflow milk measuring
       
      SensorStateMan =0;  
      //Set Interrupt registers
      TCTL2 = 0;              
      //Capture on falling edge 
      TCTL2 = EDG3B;
      TFLG1 = IC3F;
      TMSK1 |= IC3I;
      
      //ResetFloatPosition(); 
      ValveState = VALVE_START;
      HndlValveMan();
      MilkFlowCorMan = EECorFlow0;
      MilkAmountLastFlowCalcMan = 0;
      TimeLastFlowCalcMan = Cnt500msec;                

      NrOfReedPulsesMan = 0;
      StoredMlkAmount = 0;
      MlkAmount = 0;
      MlkAmountKg = 0;
      MlkDuration = 0;
      MlkActFlow = 0;
      MlkMaxFlow = 0;
      MlkMode = MM_MEAS;
      Init = TRUE;
      LastMilkCalc = FALSE;
    break;


    case MM_MEAS: 
      HndlValveMan();
      switch(MlkState) {
      
        //Miling??
        case MLK_IDLE:
        case MLK_NEUTRAL:
        case MLK_MEASURING:
        case MLK_TAKE_OFF_WAIT:
        case MLK_TAKE_OFF_MAN:
          //Milk yield calculation:  
          if (MilkManualInput == FALSE) {
            MlkAmount += CalcMilkYieldMan();
            MlkAmountKg = (MlkAmount+50)/100;
          }
          
          //save milk yield every second, to determine milk flow
          if (Cnt1sec != Cnt1secLast) {
            PutMlkTbl(MlkAmount,100);

            // bewaar ook het verloop van de melkgift iedere 5 sec. ------------    
            if ( (Cnt1sec % 5) == 0 ){
            	PutMlkAmountInTable((unsigned char)(((MlkAmount-StoredMlkAmount)+5)/10));
              StoredMlkAmount = MlkAmount; 
            }  
            Cnt1secLast = Cnt1sec;
          }
        break;

        case MLK_SEND_WAIT: 
        case MLK_DONE:                                 
        
          // Na de  laatste melkdump een correctie van 0.5*offset +  0.5* correctiefactor laatst gemeten flowrate
          if (LastMilkCalc == FALSE && MilkManualInput == FALSE) {
             LastMilkCalc = TRUE;
             MlkAmount += ((EEMmOffsetMan/2) + (MilkFlowCorMan/2));  
             MlkAmountKg = (MlkAmount+50)/100;
             ValveState = VALVE_START;
          }   
          HndlValveMan();
        break;


        default:
        break;
      }
     
    break;

    case MM_DHIA:
      HndlValveMan();

      //Melkberekenen 
      MlkAmount += CalcMilkYieldMan();
      MlkAmountKg = (MlkAmount+50)/100;

    break; 
     
    case MM_SERVICE:
      HndlValveMan();

    break; 
     
    case MM_CLEANING:
      switch (CleanPhase) { 
        case CLEAN_PHASE_START:
          HndlValveMan();

          //Tien seconden wachten na opstarten (anders start reiniging direct omdat
          // klep tijdens initialiseren wordt bekrachtigd
          if (Cnt1sec > 10  ) {
            //Bedien klep eenmalig
            ValveState = VALVE_START;
            HndlValveMan();
            CleanPhase = CLEAN_PHASE_START_1;
          }
        break;  
             
        case CLEAN_PHASE_START_1:
          HndlValveMan();  
          //Wacht tot klep in rust is 
          if (ValveState == VALVE_OFF) {
            CleanPhase = CLEAN_PHASE_START_2;
          }  
        break;  
             
        case CLEAN_PHASE_START_2:
          //Wacht op reedpulse voordat reiniging wordt gestart
          HndlValveMan();  
          //Wacht tot klep in rust is 
          if (ValveState != VALVE_OFF) { 
            //Start reinigings fasen
            CleanPhase = CLEAN_PHASE_0;
            StartCleanPhase = Cnt1sec;
            StartCleaning = Cnt1sec;
          }  
        break;  
              
        case CLEAN_PHASE_0: 
          //Handel klep af na reed puls en start reinigings fase 1 hierna
          if (ValveState == VALVE_ON) {
            HndlValveMan();
          } else {
            CleanPhase = CLEAN_PHASE_1;
            StartCleanPhase = Cnt1sec;
            //Disable input capture 3 interrupt                      
            TMSK1 &= 0xff - IC3I;
            TFLG1 = IC3F;
          }
            
        break;
        
        //Reinig melk verzamel kamer
        case CLEAN_PHASE_1:
          //Reinigingsfase 1 verstreken
          if (Cnt1sec > (StartCleanPhase + EEClnPhs1)) {
            CleanPhase = CLEAN_PHASE_2;
            StartCleanPhase = Cnt1sec;
            ValveState = VALVE_RELEASE;
            HndlValveMan();
            //Enable input capture 3 interrupt                      
            TFLG1 = IC3F;
            TMSK1 |= IC3I;         

          } else {
            //Klep continue bedienen
            ValveState = VALVE_CONT_ON;
            HndlValveMan();
          }
        break;
            
        //Klep bedienen bij volle meetkamer     
        case CLEAN_PHASE_2:
          //Reinigingsfase 2 verstreken
          if (Cnt1sec > (StartCleanPhase + EEClnPhs2)) {
            
            //Activeren melkmeter klep na reinigingsfase 2
            ValveState = VALVE_START;
            HndlValveMan(); 

            StartCleanPhase = Cnt1sec;
            CleanPhase = CLEAN_PHASE_3;

          } else{
            HndlValveMan();
          }    
        break;
         
        case CLEAN_PHASE_3:
          if (Cnt1sec > (StartCleanPhase + EEClnPhs4)) {
            //Einde reinigings programma
            if (Cnt1sec > (StartCleaning + (60 * EECleanTime))) { 
              CleanPhase = CLEAN_PHASE_END;
              ValveState = VALVE_START;
            } else {
              //Opnieuw starten reinigings cyclus
              StartCleanPhase = Cnt1sec;
              CleanPhase = CLEAN_PHASE_1;
              //Enable input capture 3 interrupt                      
              TFLG1 = IC3F;
              TMSK1 |= IC3I;         
            }
          } else{
            if (ValveState == VALVE_ON) {
              HndlValveMan();
            } else {
              //Disable input capture 3 interrupt                      
              TMSK1 &= 0xff - IC3I;
            }
          }    
             
        case CLEAN_PHASE_END:
          HndlValveMan();
        break;  
      }    
            
    break;
    
    default :
    break;
  }
}


