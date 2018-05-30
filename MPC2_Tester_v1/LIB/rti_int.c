/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*                                                              */
/*Description: Real Time Interrupt Module                       */
/*                                                              */
/*                                                              */
/****************************************************************/

/* include files                                                */

#include "io6811.h"
#include "reg_flags.h"
#include "rti_int.h"
#include "convert.h"
#include "intr6811.h"
#include "watchdog.h"

#pragma codeseg(COMMON)


// =============================================================================
// RTI  Program                                                
// =============================================================================
unsigned long Cnt1sec;
unsigned long Cnt500msec;
unsigned long SwitchOffTime;
unsigned long AbsTime = 0;   
unsigned long AbsDate = 0;   
unsigned long PerPresenceResultTime;
 
#pragma function=interrupt

void RtiInt(void)
{
  static unsigned char Cnt16msec;
//  WatchDog();
  Cnt16msec++;                    // increment counter 16.38 msec 

  if (Cnt16msec == 61) {
    Cnt16msec = 0;
    Cnt1sec++;
    AbsTime++;                    // System Clock Time -------------------------
    Cnt500msec++;
  } else {
    if (Cnt16msec == 31) {
      Cnt500msec++;
    }
  }

  TFLG2 = RTIF;
}
#pragma function=default


