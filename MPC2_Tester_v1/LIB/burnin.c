// =============================================================================
// 
// Description: Brunin Programma Modules for MPC 2
//                                                
//                                                
// =============================================================================




// include files                                                

#include "burnin.h"
#include "ramtest.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../lib/watchdog.h"
#include "../lib/typedef.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"
#include "../lib/hptest.h"
#include "../lib/text.h"
#include "../lib/mem.h"
#include "../lib/iic.h"
#include "../lib/adconv.h"
#include "../lib/delay.h"
#include "../include/cstartup.h"
#include "../include/romeehc1.h"
#include "../comm/startnet.h"
#include "../comm/flowctrl.h"
#include "../motor/motor.h"
#include "../net/ext_net.h"
#include "../usx/fake_usx.h"
#include "../key/keyint.h"
#include "../net/ext_net.h"


#pragma codeseg(EEPAGE)


#define FALSE   0
#define TRUE    1  
#define TOL     15 

 

const unsigned char AddressArray[6][6] = {
     1,  2,  3,  4,  5,  6,
     7,  8,  9, 10, 11, 12,
    13, 14, 15, 16, 17, 18,
    19, 20, 21, 22, 23, 24,
    25, 26, 27, 28, 29, 30,
    31, 32, 33, 34, 35, 36
};    

unsigned char TestCommand, TestMode; 
BURN_IN_RESULT TestResult; 
BURN_IN_TOT_RESULT TotalTestResult; 
 
#if ( _HP_TEST_ )
  
// =============================================================================
// WAIT LOOP
// =============================================================================
void WaitXSec(unsigned char Sec) 
{
  unsigned long Time;

  Time = Cnt1sec;
  while(Cnt1sec < (Time +Sec)) WatchDog();                    
}
 
// =============================================================================
unsigned char ConvertResult(unsigned char Result) {

  if (Result == 0x3A) {Result = 'A';}
  if (Result == 0x3B) {Result = 'B';}
  if (Result == 0x3C) {Result = 'C';}
  if (Result == 0x3D) {Result = 'D';}
  if (Result == 0x3E) {Result = 'E';}
  if (Result == 0x3F) {Result = 'F';}
  return(Result);
} 
#endif
  
// =============================================================================
//Set BurnIn Addres TestProg 
// =============================================================================
unsigned char GetBurnInAddress(void)  
{ 
  unsigned char  cv[5]; 
  unsigned char  cv1[5]; 
  unsigned char  stat, stat1, quit = 0;                                                 
  unsigned char  Address;                                                 
  ITEM_INFO *Item;
               
  // geef PIC de kans om geleidbaarheid te meten -------------------------------
  Delay(5000);  //WaitXSec(5);

  for ( quit = 0; quit < 20; quit++ ){
    stat = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)cv );   
  }   
  quit = 0;
  
  do {
    stat = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)cv );   
    stat1 = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)cv1 );   
    
    if ( stat == 0 && stat1 == 0 ){
      if ( cv[3] > cv1[3] ){    
        if ( (cv[3] - cv1[3]) > 5 ) continue;
      } else {
        if ( (cv1[3] - cv[3]) > 5 ) continue;
      }
      if ( cv[4] > cv1[4] ){    
        if ( (cv[4] - cv1[4]) > 5 ) continue;
      } else {
        if ( (cv1[4] - cv[4]) > 5 ) continue;
      }
      quit = 1;
    }
  } while(quit == 0);
                    
  if      (cv[3] <= 42)                  cv[3] = 5; 
  else if (cv[3] >  42  && cv[3] <= 54 ) cv[3] = 4;
  else if (cv[3] >  54  && cv[3] <= 72 ) cv[3] = 3;
  else if (cv[3] >  72  && cv[3] <= 98 ) cv[3] = 2;
  else if (cv[3] >  98  && cv[3] <= 136) cv[3] = 1;
  else                                   cv[3] = 0;

  if      (cv[4] <= 42)                  cv[4] = 5; 
  else if (cv[4] >  42  && cv[4] <= 54 ) cv[4] = 4;
  else if (cv[4] >  54  && cv[4] <= 72 ) cv[4] = 3;
  else if (cv[4] >  72  && cv[4] <= 98 ) cv[4] = 2;
  else if (cv[4] >  98  && cv[4] <= 136) cv[4] = 1;
  else                                   cv[4] = 0;

  Address = AddressArray[cv[4]][cv[3]];                 
   

  if (Address > 50 || Address < 1) {
    Address = 1;
  }
  
  // Programmeer adres ---------------------------------------------------------
  Item = (ITEM_INFO *)&AddressEE;

  WriteEEpromByte(Address, (unsigned char *)Item->PEEAddress);

 
  // for test with emulator ----------------------------------------------------
//  return(1);
  
  return(Address);
}
      
#if ( _HP_TEST_ )
       
// =============================================================================
// Relais en Pe1 en Pe2 input test
// =============================================================================
unsigned char MKTest(void)  
{
  unsigned char  result = 0;

  // met M1, M2, M3 worden K3, K4 en K5 ingelezen ------------------------------
  ExtPortCopy |= ( K3+K4+K5 );
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(2); 
  result = ( PORTA & 0x07 );


  ExtPortCopy &= (0xff - ( K3+K4+K5 ));
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(2); 

  result |= (( PORTA & 0x07 ) ^ 0x07 );
  return result;          
}
 


// =============================================================================
unsigned char IKTest(void)  
{
  unsigned char  result = 0;
  unsigned char  adc;
   
  // Test K1 en K2 -------------------------------------------------------------
  ExtPortCopy &= (0xff - (K1+K2));
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(1); 
  adc = ADConv1Of8( CHANNEL5 );
  if ( adc > 10) result |= 0x01;

  ExtPortCopy |= K2;
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(1); 
  adc = ADConv1Of8( CHANNEL5 );
  if ( adc < 45 || adc > 65 ) result |= 0x02;

  ExtPortCopy &= (0xff - (K1+K2));
  ExtPortCopy |= K1;
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(1); 
  adc = ADConv1Of8( CHANNEL5 );
  if ( adc < 45 || adc > 65 ) result |= 0x04;

  ExtPortCopy |= K1+K2;
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  WaitXSec(1); 
  adc = ADConv1Of8( CHANNEL5 );
  if ( adc < 90 || adc > 115 ) result |= 0x08;

  ExtPortCopy &= (0xff - (K1+K2));
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   
  return result;          
}

// =============================================================================
// M-input and P-output test
// =============================================================================
unsigned char IPTest(void) 
{
  unsigned char result;
  
  // Test I1--I4 en P1/P2/P3/L -------------------------------------------------         
  PORTA |= 0x38;                    // P1/P2/P3 laag          
  PORTD |= 0x20;                    // L        laag
  WaitXSec(2); 
  result = PORTE & 0x0f;

  PORTA &= (0xff - 0x38);           // P1/P2/P3 hoog         
  PORTD &= (0xff - 0x20);           // L        hoog
  WaitXSec(2); 
  result |= (PORTE & 0x0f)^0x0f;

  return result;          
}

// =============================================================================
unsigned char MtrTest(void)  
{
  unsigned char  stat1 = 0, stat2 = 1;
  unsigned short starttime1, stoptime1;
  unsigned short starttime2, stoptime2;
  unsigned char  result = 0;

  // TestMotor met rem, stoppen tussen de 30 en 70 msec ------------------------
  while( stat1 != 1) stat1 = MotorBreakOnOff(1);
  TurnMotorOn(0);
  WaitXSec(3); 
  starttime1 = stoptime1 = Cnt10msec;    
  TurnMotorOff(0);
  do {                                         
    if ( ChkMtrInputLevel( 0)){
      stoptime1 = Cnt10msec;
      break;
    }
  } while( !TimeControl( Cnt10msec, starttime1 + 100)); 

  if ( !( TimeControl( stoptime1, starttime1 + 0 ) && 
         !TimeControl( stoptime1, starttime1 + 5 )) )result++;

  // TestMotor, zonder rem -----------------------------------------------------
  while( stat2 != 0)stat2 = MotorBreakOnOff(0);
  TurnMotorOn(0);
  WaitXSec(3); 
  starttime2 = stoptime2 = Cnt10msec;    
  TurnMotorOff(0);
  do {                                         
    if ( ChkMtrInputLevel( 0)){
      stoptime2 = Cnt10msec;
      break;
    }
  } while( !TimeControl( Cnt10msec, starttime2 + 150)); 

  if ( !( TimeControl( stoptime2, starttime2 + 6 ) && 
         !TimeControl( stoptime2, starttime2 + 11 )) )result++;
  
  if ( result ){                           
    starttime1 = stoptime1 > starttime1 ? ( stoptime1 - starttime1) : (starttime1-stoptime1);
    starttime2 = stoptime2 > starttime2 ? ( stoptime2 - starttime2) : (starttime2-stoptime2);

    // display error values ------------------------------------------------
    // zelfde waarde ander type tbv display --------------------------------
    DispValue((unsigned char *)&(starttime1), _U1, 3, 0, READ_ONLY, 1); 
    DispValue((unsigned char *)&(starttime1), _U2, 3, 0, READ_ONLY, 1); 
    DispValue((unsigned char *)&(starttime2), _U1, 3, 0, READ_ONLY, 5); 
    DispValue((unsigned char *)&(starttime2), _U2, 3, 0, READ_ONLY, 5); 
    WaitXSec(4);
    return result;          
  } else return 0;
}

// =============================================================================
unsigned char CTTest(void)  
{
  unsigned char  stat1, stat2;
  unsigned char  result = 1;
  unsigned char  cv[6], tb[2];
  

  // Test RR/RF en de Temperatuur ----------------------------------------------
  stat1 = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)cv );   
  stat2 = HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, (unsigned char *)tb );   
   
  if ( !stat1 && !stat2 ){ 
    if (cv[0] == IIC_READ_COND && tb[0] == IIC_READ_TEMP){  
      result = 0;
      if( cv[1] > 165 || cv[1] < 150 ) result  = 0x01;         // RV
      if( cv[2] > 165 || cv[2] < 150 ) result |= 0x02;         // RA
      if( tb[1] > 126 || tb[1] < 118 ) result |= 0x04;         // Temp

      if (result){
        // display error values ------------------------------------------------
        // zelfde waarde ander type tbv display --------------------------------
        DispValue((unsigned char *)&(cv[1]), _U2, 3, 0, READ_ONLY, 1); 
        DispValue((unsigned char *)&(cv[1]), _U1, 3, 0, READ_ONLY, 1); 
        DispValue((unsigned char *)&(cv[2]), _U2, 3, 0, READ_ONLY, 4); 
        DispValue((unsigned char *)&(cv[2]), _U1, 3, 0, READ_ONLY, 4); 
        DispValue((unsigned char *)&(tb[1]), _U1, 3, 0, READ_ONLY, 8); 
        WaitXSec(4);
      }  
    }
  }  
  return result;          
}
#endif
 
// =============================================================================
// BURN IN PROGRAM
// =============================================================================
void BurnInProg(void) 
{
#if ( _HP_TEST_ )
  unsigned char NetTest = 0;
  unsigned long LastResultTime;

//  unsigned char Test;
  unsigned char Result;

  TestCommand = FALSE;
  TestResult.Ready        = 0;
  TestResult.RomResult    = 0;
  TestResult.RamResult    = 0;
  TestResult.VC3Result    = 0;
  TestResult.PMResult     = 0;
  TestResult.RelaisResult = 0;
  TestResult.UartResult   = 0;
  TestResult.DummyResult  = 0;
 
  TotalTestResult.NrOfTests    = 0;
  TotalTestResult.RomResult    = 0;
  TotalTestResult.RamResult    = 0;
  TotalTestResult.VC3Result    = 0;
  TotalTestResult.PMResult     = 0;
  TotalTestResult.RelaisResult = 0;
  TotalTestResult.UartResult   = 0;

 
  ExtPortCopy &= (0xff -( K1+K2 ));
  HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );   

  
  while(1){

    if ( TestCommand == 2){
      ShortToDisp(TotalTestResult.NrOfTests, 8, 3);
      memory_cpy((unsigned char *)&(DispTable[2]), (unsigned char *)&((SrvTxtString[TXT_BURNIN_2])+2), 5 );
    } else {
      memory_cpy((unsigned char *)&(DispTable[2]), (unsigned char *)&((SrvTxtString[TXT_BURNIN])+2), 7 );
      Result = NetTest + 0x30;
      AsciiToDisp(Result, 10, 1);
    }
    ShortToDisp(EEAddressCopy, 1, 2);

    Display();
    
    //Indien er vijf minuten lang geen test commando wordt ontvangen,
    //dat wordt het netwerk opnieuw geinitialiseerd
    if (Cnt1sec > (LastResultTime +300)) {
       LastResultTime = Cnt1sec;
       NetClose(CHANNEL);
       InitStartNet();
       NetTest++;
    }   

    if (TestCommand == 1 || TestCommand == 2) { 
     
      //Start testprogramma
      TestResult.Ready = 0;
      if (TestCommand == 2) WaitXSec( EEAddress);   // stand-alone

      // Test ROM  *************************************************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_ROM_TEST])+3), 7 );
//      TestResult.RomResult = FTChecksumRom(0);
      TestResult.RomResult = 0;
      TotalTestResult.RomResult = 0;

      Result = TestResult.RomResult + 0x30;
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);
 
      // Test RAM **************************************************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_RAM_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.RamResult = RamTestAsm(1); 
      if ( TestResult.RamResult )TotalTestResult.RamResult++;

      Result = TestResult.RamResult + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);

    
      // TEST M1,M2, M3 input and K1/K2/K3 outputs *****************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_M_K_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.VC3Result = MKTest();
      if ( TestResult.VC3Result )TotalTestResult.VC3Result++;

      Result = TestResult.VC3Result + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);
     

      // Test I5 en K1 K2  *****************************************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_I_K_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.PMResult = IKTest();
      if ( TestResult.PMResult )TotalTestResult.PMResult++;

      Result = TestResult.PMResult + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);


      // Test I1-I4 dmv P1,P2,P3 en L ******************************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_I_P_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.RelaisResult = IPTest();
      if ( TestResult.RelaisResult )TotalTestResult.RelaisResult++;

      Result = TestResult.RelaisResult + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);

      // Test de Motor Uit en Ingangen *****************************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_MTR_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.UartResult = MtrTest();
      if ( TestResult.UartResult )TotalTestResult.UartResult++;

      Result = TestResult.UartResult + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3);
      
      // Test RR en RF plus de Temperatuur ingang ******************************
      memory_cpy((unsigned char *)&(DispTable[3]), (unsigned char *)&((SrvTxtString[TXT_C_T_TEST])+3), 7 );
      WaitXSec(1);
      TestResult.DummyResult = CTTest();
      if ( TestResult.DummyResult )TotalTestResult.DummyResult++;

      Result = TestResult.DummyResult + 0x30;
      if (Result > 0x39 ) Result = ConvertResult(Result);
      AsciiToDisp(Result, 10, 1);
      WaitXSec(3); 
      
      TestResult.Ready = 1;
      TotalTestResult.NrOfTests += 1;
      if (TestCommand == 1) {
        HndlFTBurnInRes();
        TestCommand = 0;		// mbv computer, dus wachten tot commando weer ongelijk nul wordt  
      }
      LastResultTime = Cnt1sec; 

 
      if ( TestCommand == 2){
        // stand-alone: even pauze
        memory_cpy((unsigned char *)&(DispTable[2]), (unsigned char *)&((SrvTxtString[TXT_BURNIN_2])+2), 5 );
        ShortToDisp(TotalTestResult.NrOfTests, 8, 3);
        ShortToDisp(EEAddressCopy, 1, 2);
        WaitXSec( 37-EEAddress);
      }
    } 
  }
#endif 
}
  

