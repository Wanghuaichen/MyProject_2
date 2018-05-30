// =============================================================================
//                                                              
// (c) Copyright NEDAP N.V. GROENLO HOLLAND                     
//     ALL RIGHTS RESERVED                                      
//                                                              
// =============================================================================

// include files  --------------------------------------------------------------

#pragma codeseg(COMMON)

#include "main.h"
#include "init.h"
#include "../include/romver.h"
#include "../include/romeehc1.h"
#include "../include/romstnet.h"
#include "../include/romdwnld.h"
#include "../key/disp.h"
#include "../lib/iic.h"
#include "../lib/io6811.h"
#include "../lib/watchdog.h"
#include "../lib/reg_flags.h"
#include "../lib/applver.h"
#include "../lib/eeprom.h"
#include "../lib/schedule.h"
#include "../lib/extport.h"
#include "../lib/boolean.h"
#include "../lib/eehc11.h"
#include "../lib/hptest.h"
#include "../lib/end_test.h"
#include "../usx/fake_usx.h"


// =============================================================================
// Main Program                                                 
// =============================================================================
non_banked void Main(void) 
{
  unsigned char  i = 0, RetVal = 0;
  unsigned char  sbuf[10];

  // Check Rom-versions --------------------------------------------------------
  while (CompRomVersion[i] != 0xffffffff) {
    if (CompRomVersion[i++] == RomProgVersion) {
      RetVal = 1;
      break;
    }
  }

  if (!RetVal) {
    if (EEPROGPresent == PROG_PRESENT) {
      WriteEEpromByte(~PROG_PRESENT, &EEPROGPresent);
    }
    StartRom();
  }

  Init();                       // Initialisatie modules -----------------------

  CommunicationError = 0;
  RequestProgVersionsPIC();     // vraag programma versies van de PIC's op
  
  WatchDog();         
  
  sbuf[0] = ExtPortCopy = 0x20; // schakel uitgangen onder spanning
  if ( HandleWriteIic(PICA_ADDRESS, IIC_WRITE_PORTB, 1, (unsigned char *)&sbuf ) ){
    // give error --------------------------------------------------------------
    if (!CommunicationError) CommunicationError = COMM_ERR_IC13;    
  }

  // geef de knipper frequentie op ---------------------------------------------
  sbuf[0] = sbuf[1] = 10;
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_BLINK_TIMES, 2, sbuf );

//  HandleReadIic(KB_ADDRESS, IIC_GET_PROG_OPTIONS, 2, BlinkTimes );

#if ( !_HP_TEST_ )
  CheckForEndTest();
#endif


  Scheduler();
  while(1);                     // scheduler komt normaal niet terug !!!!!!!!!!!
}


