// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND           DEC 1998
//     ALL RIGHTS RESERVED
//
// =============================================================================




// include files ---------------------------------------------------------------

#include "init.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../lib/watchdog.h"
#include "../lib/typedef.h"
#include "../lib/eeprom.h"
#include "../lib/convert.h"
#include "../lib/iic.h"
#include "../lib/extport.h"
#include "../lib/applver.h"
#include "../lib/hptest.h"
#include "../lib/burnin.h"
#include "../lib/intr6811.h"
#include "../key/disp.h"
#include "../key/key.h"
#include "../appl/milk.h"
#include "../appl/temp.h"
#include "../appl/takeoff.h"
#include "../appl/tandem.h"
#include "../appl/pulsation.h"
#include "../include/romstnet.h"
#include "../init/start.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../comm/startnet.h"
#include "../comm/flowctrl.h"
#include "../motor/motor.h"


#pragma codeseg(EEPAGE)

// =============================================================================
// initialisate modules
// =============================================================================
void Init(void)
{

  TMSK1 = 0;
  TMSK2 = 0;
  SCCR2 = 0;
  SPCR  = 0;
  DDRD =  0x3a;


  // Check for HP Processorprint test  -----------------------------------------
#if ( _HP_TEST_ )
  // kijk of de print functioneel getest moet worden ---------------------------
  DDRD  &= (0xff - 0x22);          // PD1/PD5 input

  if(( PORTD & 0x022 ) == 0 ){
    PORTD &= (0xff-0x10);             // toggle PD3
    // functionele test --------------------------------------------------------
    while (( PORTD & 0x22 ) != 0x22)WatchDog();
    DDRD  |= 0x22;                 // PD1/PD5 output
    HpTest();
  } else {
    // in MainMilking (milk.c) wordt hier op getest ----------------------------
    TestMode = BURN_IN;
  }
  DDRD  |= 0x22;                 // PD1/PD5 output
#endif

  InitConfig();                 // initieer configuratie

  InitPulsation();
  InitReg();                    // initieer registers
  InitIic();
  InitDisp();                   // initialize display
  InitMlk();                    // initialize milk
  InitKey();
  InitTemperature();
  InitExtPort();                // initieer external Port
  InitTO();                     // initialize Take Off
  init_motor(20);
  init_flowctrl();				// zet correctie factors geleidbaarheid
  InitStartNet();               // initialize NetWork
  InitOpenTandem();
  SetVersion();                 // set versions
}



// =============================================================================
// initialisate registers
// =============================================================================
void InitReg(void)
{
  TFLG2 = 0xff;                 // reset evt. int. flags
  PACTL |= 0x2 + 0x08;          // RTI every 16.38 msec, PA3 output, output compare

  BAUD = 0x30;                  // Baudrate is 9600
  SCCR1 = 0x18;                 // One start bit, nine databits, one stop bit, wake on nineth bit
  SCCR2 = 0x00;                 // Enable TE
  TFLG1 = 0xff;                 // reset evt. int. flags

  TMSK1 |= OC2I;                // werd gebruikt voor MessageInt, nu Cnt10msec
  TFLG1 = OC2F;

  DDRD  = 0x3a;
  PORTA = 0x40;                 // PA6 hoog, motor uit.

  WatchDog();                   // Reset WatchDog Circuit

  enable_interrupt();
}


// =============================================================================
// initialisate configuratie
// =============================================================================
void InitConfig(void)
{
  ITEM_INFO *Item;

  // Controleer instellingen, indien fout programmeer default waarde -----------
  Item = (ITEM_INFO *)&AddressEE;

  while (Item != &LastEE) {
    switch (Item->Type) {
      case _S1:
              if ((*(signed char *)Item->PEEAddress< (signed char)Item->MinVal) ||
                  (*(signed char *)Item->PEEAddress> (signed char)Item->MaxVal)) {
                WriteEEpromByte((unsigned char)Item->Default, (unsigned char *)Item->PEEAddress);
              }
              break;
      case _U1:
      case _BL:
              if ((unsigned short)(*(Item->PEEAddress))< (unsigned short)Item->MinVal ||
                  (unsigned short)(*(Item->PEEAddress))> (unsigned short)Item->MaxVal) {
                WriteEEpromByte((unsigned char)Item->Default, (unsigned char *)Item->PEEAddress);
              }
              break;

      case _S2:
              if ( ((*(signed short *)(Item->PEEAddress)) < (signed short)Item->MinVal) ||
                  ((*(signed short *)(Item->PEEAddress))> (signed short)Item->MaxVal) ){
                WriteEEpromShort((unsigned short)Item->Default, (unsigned short *)Item->PEEAddress);
              }
              break;
      case _U2:
              if ( ((*(unsigned short *)(Item->PEEAddress)) < Item->MinVal) ||
                  ((*(unsigned short *)(Item->PEEAddress))> Item->MaxVal) ){
                WriteEEpromShort((unsigned short)Item->Default, (unsigned short *)Item->PEEAddress);
              }
              break;
    }
    Item++;
  }

}



// =============================================================================
// initialisate adress
// =============================================================================
void InitExtPort(void)
{
  // moet IIc worden
  ExtPortCopy = 0;
  ActExtPortCopy = 0;
  ExtPortK3State = 0;
  ExtPortK4State = 0;
  ExtPortK5State = 0;

  ExternMessages = 0;				// tbv oversturen berichten onderling
}



