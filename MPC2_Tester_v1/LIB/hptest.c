// =============================================================================
//                                                             
// (c) Copyright NEDAP N.V. GROENLO HOLLAND             
//     ALL RIGHTS RESERVED                                     
//                                                             
// Description: Test Programma Modules for MPC                 
//                                                             
// =============================================================================

// include files  --------------------------------------------------------------
#include "io6811.h"
#include "intr6811.h"
#include "reg_flags.h"
#include "watchdog.h"
#include "iic.h"
#include "time.h"
#include "int.h"
#include "bnksel.h"                
#include "eeprom.h"                
#include "ramtest.h"     
#include "../lib/extport.h"
#include "../key/keyint.h"
#include "../include/cstartup.h"
#include "eehc11.h"
#include "hptest.h"

#include "../appl/takeoff.h"
#include "../lib/convert.h"
#include "../lib/mem.h"
#include "../lib/adconv.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../motor/motor.h"
#include "../key/keyint.h"
#include "../key/key.h"
#include "burnin.c"
 
#pragma codeseg(SPECIALMODE)


#define FALSE   0
#define TRUE    1  
#define NR_OF_COMM_TESTS  10 
#define TOL     20
                                                                                                            
                                                                                                             
extern unsigned char BankRegCopy; 

//extern non_banked void ChecksumCurrentBank(void); 
//extern non_banked void RRomBankSel(unsigned char RomBank);


#if ( _HP_TEST_ )

// RomBank selectie ------------------------------------------------------------
non_banked void RRomBankSel(unsigned char RomBank)
{
  unsigned char value;

  value = BankRegCopy;
  value &= 0xe0;
  value |= RomBank;
  BankRegCopy = value;
  *(unsigned char *)BANK = value;
}


/***************************************************************/
/*Bepaal Checksum appl.                                        */
/***************************************************************/

non_banked unsigned char FTChecksumRom(unsigned char ProgChange) 
{
  unsigned long CheckSum;
  unsigned char i, *j, EEPromBanks, BankOld, Int;
  unsigned short EEPromStartAddress, EEPromEndAddress;

  EEPromStartAddress = *(unsigned short *)(Start_EE_Bank_Appl);
  EEPromEndAddress = *(unsigned short *)EE_Bank_Size_Appl + EEPromStartAddress;
  EEPromBanks = *(unsigned char *)Nr_EE_Banks_Appl;

  CheckSum = 0; 

  Interrupts_Off(&Int);

//  BankOld = BankACopy;

  //Bepaal checksum van alle banken waarin het applicatie programma zit
  for ( i = *(unsigned char *)(Start_PROG_Bank_Appl); i < EEPromBanks; i++) {
    Interrupts_On(&Int);
    WatchDog(); 
    Interrupts_Off(&Int);
    RRomBankSel(i);
    for (j = (unsigned char *)EEPromStartAddress; j < (unsigned char *)EEPromEndAddress; j++) {
      if ( ((unsigned short)j % 0x1000) == 0) {
        WatchDog();
//        ChecksumCurrentBank();
      }  
      CheckSum += *j;
    }
  }

  i = *(unsigned char *)BANK;
  i &= 0x1f;
  i |= BankOld;
  *(unsigned char *)BANK = i;
 
  if (CheckSum == (*(unsigned long *)(CheckSum_Appl)) ){ 
    Interrupts_On(&Int);
 
    //Mag waarde in EEProm al gewijzigd worden
    if(ProgChange == 1) {
      return(0);
    } else {
      return(0);
    }
  }
  Interrupts_On(&Int);
 
  //ProgPresent(FALSE);
  return(1);

}


// =============================================================================
non_banked unsigned short GetHpTestCommand(void)
{
  // wacht op byte, eerste byte moet F1 zijn ----------------------------------
  unsigned short r_val;
  unsigned char  x;

rcv_start:
  r_val = 0;
  while ( !(SCSR & 0x20 )) WatchDog();
  if ( SCSR & 0x0e ){
    // receive error -----------------------------------------------------------
    SCSR; SCDR; goto rcv_start; 
  }
  x = SCDR;
  if (( x == 0x1F ) || ( x == 0x21 ) ){
    r_val = ( SCDR << 8);
    while ( !(SCSR & 0x20 )) WatchDog();
    if ( (x=SCSR) & 0x0e ){
      // receive error -----------------------------------------------------------
       SCSR; SCDR; goto rcv_start; 
    }
    x = SCDR;
    return (r_val | x);
  } else  goto rcv_start;  
}


// =============================================================================
// Print "TESTPROG'                                   
// =============================================================================
non_banked void PrintTestProg(void) 
{
  unsigned short PointInfo;
  unsigned char  Text[11];

  Text[0] = 'T';
  Text[1] = 'E';
  Text[2] = 'S';
  Text[3] = 'T';
  Text[4] = 'P';
  Text[5] = 'R';
  Text[6] = 'O';
  Text[7] = 'G';
  Text[8] = ' ';
  Text[9] = ' ';
//  Text[8] = 0x30 + (EE_U1 / 10);
//  Text[9] = 0x30 + (EE_U1 % 10);
  Text[10] = 0;

  PointInfo = 0x0000;
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&PointInfo ); WatchDog();
  HandleWriteIic(KB_ADDRESS, IIC_DISPLAY_TEXT, 10, Text); WatchDog();
}

non_banked void DisplayAll(void)
{
	disp_point = 0x3fff;
	memory_set( DispTable, ' ', 10 );
	HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&disp_point );
	HandleWriteIic(KB_ADDRESS, IIC_DISPLAY_TEXT, 10, DispTable);
}

non_banked void DisplayClear(void)
{
	disp_point = 0;
	memory_clear( DispTable, 10 ) ;
	HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&disp_point );
	HandleWriteIic(KB_ADDRESS, IIC_DISPLAY_TEXT, 10, DispTable);
}

non_banked unsigned short MeasureBrakeTime(void)
{
	unsigned short starttime, stoptime;
	unsigned short braketime = 0;

	starttime = stoptime = Cnt10msec;
	TurnMotorOff(0);
	do {
		if ( ChkMtrInputLevel(0) ){
			stoptime = Cnt10msec;
			break;
		}
	} while( !TimeControl(Cnt10msec, starttime + 150) );

	braketime = stoptime > starttime ? (stoptime - starttime) : (0xFFFF - starttime + stoptime);
	return braketime;
}

// =============================================================================
non_banked unsigned char UartTest( void )
{              
  unsigned char  r_val;
  unsigned char  i;
   
  r_val = 0;  
  
  for ( i = 0; i < NR_OF_COMM_TESTS && r_val == 0 ; i++ ){ 
    // nog doen ???? -----------------------------------------------------------
    WatchDog();
  }
  return r_val;
}

// =============================================================================
non_banked void Send( unsigned short result)
{
  unsigned short time;
  unsigned char  x;

  while( !(x = ( SCSR & 0x80 )) );
  // disable receiver ----------------------------------------------------------
  SCCR2 &= ( 0xff- 0x24 );
  SCDR = (unsigned char)(result >> 8);
  while( !(x = ( SCSR & 0x80 )) );

  time = TCNT;
  while(!TimeControl(TCNT, time + 20000))WatchDog();

  SCDR = (unsigned char)(result & 0x00ff);
  while( !(x = ( SCSR & 0x80 )) );
  SCCR2 |= 0x04;

  time = TCNT;
  while(!TimeControl(TCNT, time + 20000))WatchDog();

}


// =============================================================================
non_banked void HpTest (void) 
{
  unsigned short HpTestCommand;
  unsigned short Result, time; 
  unsigned char cv[6];
  unsigned char stat;
 
  WatchDog();
  InitIic();
  enable_interrupt();
  PrintTestProg(); 

  RequestProgVersionsPIC();

  BAUD =  0x30;                 // Baudrate is 9600 
  SCCR1 = 0x00;                 // One start bit, eight databits, one stop bit,  
  SCCR2 |= 0x08;                                        // transmit enable

  WatchDog();              
  
  WriteEEpromByte( 0, &EEExtConMeasuring);
//  if (IC08PrgVrs == 0)IC08PrgVrs = 4;
  
  time = TCNT;
  while ( !TimeControl( TCNT, time +20000))WatchDog();
  Send(0xFF50);

  while(1) {
    WatchDog();              
    
    PrintTestProg();              
    HpTestCommand = 0;                             

    HpTestCommand = GetHpTestCommand(); 
  
    switch( HpTestCommand )
    {
      case HP_ROM_TEST:
//           Result = FTChecksumRom(0);
           Result = 0;
           Send ( Result | 0xff50 );
        break;
      case HP_EEPROM_TEST:
           Send ( 0xff50 );                     // altijd ok.
        break;
      case HP_RAM_TEST:
           Result = RamTestAsm(0); 
           Send ( Result | 0xff50 );
        break;
      case HP_COND__LV_TEST:
      case HP_COND__LA_TEST:
      case HP_COND__RA_TEST:
      case HP_COND__RV_TEST:
           stat = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)cv );   
           if ( stat ) Send( 0xFF51 );
           else {
             if (cv[0] == IIC_READ_COND ) Send( (unsigned short)cv[ HpTestCommand - HP_COND__LV_TEST + 1] );
             else Send( 0xFF52 );
           }
        break;
      case HP_TEMP_TEST:
           stat = HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, (unsigned char *)cv );   
           if ( stat ) Send( 0xFF51 );
           else {
             if (cv[0] == IIC_READ_TEMP ) Send( (unsigned short)cv[1] );
             else Send( 0xFF52 );
           } 
        break;     
      case HP_TEMP_REF1:   
           stat = HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, (unsigned char *)cv );   
           if ( stat ) Send( 0xFF51 );
           else {
             if (cv[0] == IIC_READ_TEMP ){
               if ( (cv[1] >= 83) && (cv[1] <= 95 )) {
                 // ref '35' ok, store -----------------------------------------
                 WriteEEpromByte( cv[1], &EE_U1 ); 
                 if ( EE_U1 == cv[1] ) Send( 0xff50 );
                 else                  Send( 0xff52 );
               } else                  Send( 0xFF52 ); 
             } else                    Send( 0xFF52 );
           } 
        break;
      case HP_TEMP_REF2: 
           stat = HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, (unsigned char *)cv );   
           if ( stat ) Send( 0xFF51 );
           else {
             if (cv[0] == IIC_READ_TEMP ){
               if ( (cv[1] >= 148) && (cv[1] <= 164 )) {
                 // ref '40' ok, store -----------------------------------------
                 WriteEEpromByte( cv[1], &EE_U2 ); 
                 if ( EE_U2 == cv[1] ) Send( 0xff50 ); 
                 else                  Send( 0xff52 );
               } else                  Send( 0xFF52 ); 
             } else                    Send( 0xFF52 );
           } 
        break;
      case HP_RTN_REF1: 
           Send( EE_U1 ); 
        break;
      case HP_RTN_REF2: 
           Send( EE_U2 ); 
         break;
      case HP_WD_TEST: 
           if ( CONFIG & NOCOP ) Send( 0xFF51 );
           else                  Send( 0xFF50 ); 
        break;
//      default:
//           if ( HpTestCommand > 0x2100 && HpTestCommand < 0x212f ){
//             if ( HandleWriteIic(PICA_ADDRESS, IIC_WRITE_PORTB,   1, (unsigned char *)((unsigned char *)(&HpTestCommand)+1)) ){
//               Send ( 0xff51 );
//             } else {
//               Send ( 0xff50 );
//             }
//           }
//        break;
      case HP_REQUEST_PICA_VERSION:
    	  Result = 0;
    	  Result = (unsigned short) IC08PrgVrs;
    	  Send(Result);
    	  break;
      case HP_REQUEST_PICB_VERSION:
    	  Result = 0;
    	  Result = (unsigned short) IC13PrgVrs;
    	  Send(Result);
    	  break;
      case HP_REQUEST_CONDUCTIVITY_VERSION:
    	  Result = 0;
    	  Result = (unsigned short) IC09PrgVrs;
    	  Send(Result);
    	  break;
      case HP_DISPLAY_TEST_OFF:
    	  DisplayClear();
          Send(TEST_PASSED);
    	  break;
      case HP_DISPLAY_TEST_ON:
    	  DisplayAll();
          Send(TEST_PASSED);
    	  break;
      case HP_LED_TEST_OFF:
    	  DispLeds = 0x00;
    	  stat = HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_INFO, 1, (unsigned char *)&DispLeds );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_LED_TEST_ON:
    	  DispLeds = 0xFF;
    	  stat = HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_INFO, 1, (unsigned char *)&DispLeds );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_LAMP_TEST_OFF:
          LampControl(LAMP_OFF, 0);
//    	  PORTD &= (0xFF - 0x20);		// PD5 (68HC11) ---> LAMP
          Send(TEST_PASSED);
    	  break;
      case HP_LAMP_TEST_ON:
    	  LampControl(LAMP_ON, 0);
//    	  PORTD |= 0x20;		// PD5 (68HC11) ---> LAMP
          Send(TEST_PASSED);
    	  break;
//      case HP_KEY_COW_TEST:
//      case HP_KEY_FEED_TEST:
//      case HP_KEY_MILK_TEST:
//      case HP_KEY_DECEAS_TEST:
//      case HP_KEY_CALEND_TEST:
//      case HP_KEY_SHIFT_TEST:
//      case HP_KEY_1_TEST:
//      case HP_KEY_2_TEST:
//      case HP_KEY_3_TEST:
//      case HP_KEY_4_TEST:
//      case HP_KEY_5_TEST:
//      case HP_KEY_6_TEST:
//      case HP_KEY_7_TEST:
//      case HP_KEY_8_TEST:
//      case HP_KEY_9_TEST:
//      case HP_KEY_0_TEST:
//      case HP_KEY_ENTER_TEST:
//      case HP_KEY_ENTRAN_TEST:
//      case HP_KEY_CENTRL_TEST:
//      case HP_KEY_WATERTAP_TEST:
//      case HP_KEY_EXIT_TEST:
//      case HP_KEY_STOP_TEST:
//      case HP_KEY_MILK_V_TEST:
//      case HP_KEY_CLUSTR_TEST:
//      case HP_KEY_AUTO_TEST:
//      case HP_KEY_MANUAL_TEST:
      case HP_READ_KEY:
    	  /* The returned key code is defined as PIC-Offset + shift in KEY/key.h */
    	  Result = (unsigned short)ReadKey();
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_M1:	// M1 ---> PA0 (68HC11)
    	  Result = ( (PORTA & K3_M1) == K3_M1 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_M2:	// M2 ---> PA1 (68HC11)
    	  Result = ( (PORTA & K3_M2) == K3_M2 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_M3:	// M3 ---> PA2 (68HC11)
    	  Result = ( (PORTA & K3_M3) == K3_M3 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_I1:	// I1 ---> PE3 (68HC11)
    	  Result = ( (PORTE & K5_I1) == K5_I1 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_I2:	// I2 ---> PE2 (68HC11)
    	  Result = ( (PORTE & K5_I2) == K5_I2 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_I3:	// I3 ---> PE1 (68HC11)
    	  Result = ( (PORTE & K5_I3) == K5_I3 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_I4:	// I4 ---> PE0 (68HC11)
    	  Result = ( (PORTE & K5_I4) == K5_I4 )? STATE_HIGH: STATE_LOW;
    	  Send(Result);
    	  break;
      case HP_READ_INPUT_I5:	// I5 ---> RXD2 ---> RA4 (PIC-A)
    	  // Need further investigation over how to read I5
          Send(TEST_PASSED);
    	  break;
      case HP_READ_ANALOG_INPUT_VSS:	// VSS ---> Current Sensing Circuit ---> PE4/AN4 (68HC11)
    	  Result = (unsigned short)ADConv1Of8( CHANNEL5 );
    	  Send(Result);
    	  break;
      case HP_READ_ANALOG_INPUT_IMOTOR:		// Imotor -----------------------------> PE6/AN6 (68HC11)
    	  Result = (unsigned short)ADConv1Of8( MOTOR_CURRENT_CHANNEL );
    	  Send(Result);
    	  break;
      case HP_SET_OUTPUT_P1:	// PA3 (68HC11) ---> P1
    	  PortP1On();
//    	  PORTA |= K6_P1;
          Send(TEST_PASSED);
    	  break;
      case HP_SET_OUTPUT_P2:	// PA4 (68HC11) ---> P2
    	  PortP2On();
//    	  PORTA |= K6_P2;
          Send(TEST_PASSED);
    	  break;
      case HP_SET_OUTPUT_P3:	// PA5 (68HC11) ---> P3
    	  PortP3On();
//    	  PORTA |= K6_P3;
          Send(TEST_PASSED);
    	  break;
      case HP_RESET_OUTPUT_P1:		// PA3 (68HC11) ---> P1
    	  PortP1Off();
//    	  PORTA &= (0xFF - K6_P1);
          Send(TEST_PASSED);
    	  break;
      case HP_RESET_OUTPUT_P2:		// PA4 (68HC11) ---> P2
    	  PortP2Off();
//    	  PORTA &= (0xFF - K6_P2);
          Send(TEST_PASSED);
    	  break;
      case HP_RESET_OUTPUT_P3:		// PA5 (68HC11) ---> P3
    	  PortP3Off();
//    	  PORTA &= (0xFF - K6_P3);
          Send(TEST_PASSED);
    	  break;
      case HP_SET_RELAY_3:		// PIC-A RB0 ---> Relay 3
    	  ExtPortCopy |= RY3;
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_SET_RELAY_4:		// PIC-A RB1 ---> Relay 4
    	  ExtPortCopy |= RY4;
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_SET_RELAY_5:		// PIC-A RB2 ---> Relay 5
    	  ExtPortCopy |= RY5;
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_SET_RELAY_6:		// PIC-A RB3 ---> Relay 6
    	  ExtPortCopy |= RY6;
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_SET_RELAY_7:		// PIC-A RB4 ---> Relay 7
    	  ExtPortCopy |= RY7;
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_RESET_RELAY_3:	// PIC-A RB0 ---> Relay 3
    	  ExtPortCopy &= (0xFF - RY3);
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_RESET_RELAY_4:	// PIC-A RB1 ---> Relay 4
    	  ExtPortCopy &= (0xFF - RY4);
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_RESET_RELAY_5:	// PIC-A RB2 ---> Relay 5
    	  ExtPortCopy &= (0xFF - RY5);
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_RESET_RELAY_6:	// PIC-A RB3 ---> Relay 6
    	  ExtPortCopy &= (0xFF - RY6);
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_RESET_RELAY_7:	// PIC-A RB4 ---> Relay 7
    	  ExtPortCopy &= (0xFF - RY7);
    	  stat = HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, &ExtPortCopy );
          if ( stat ) Send( TEST_FAILED );
          else 		  Send( TEST_PASSED );
    	  break;
      case HP_MOTOR_START:
    	  TurnMotorOn(0);
          Send(TEST_PASSED);
    	  break;
      case HP_MOTOR_STOP:
    	  TurnMotorOff(0);
          Send(TEST_PASSED);
    	  break;
      case HP_BRAKE_ENABLE:
    	  stat = MotorBreakOnOff(1);
          Send(TEST_PASSED);
    	  break;
      case HP_BRAKE_DISABLE:
    	  stat = MotorBreakOnOff(0);
          Send(TEST_PASSED);
    	  break;
      case HP_READ_MOTOR_LEVEL:
    	  Result = (unsigned short)ADConv1Of8( MOTOR_INPUT_CHANNEL );
    	  Send(Result);
    	  break;
      case HP_MEASURE_MOTOR_BRAKE_TIME:
    	  Result = MeasureBrakeTime();
    	  Send(Result);
    	  break;
      default:
    	  Send(COMMAND_INVALID);
    	  break;
    } // switch( HpTestCommand )
  } // while(1)
} // void HpTest (void)

  
#endif 
