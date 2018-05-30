/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*                                                              */
/*Description: Key Interrupt Handler                            */
/*                                                              */
/*                                                              */
/****************************************************************/

/* include files                                                */


//#include "../lib/boolean.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"

#include "../lib/watchdog.h"
#include "keyint.h"
#include "key.h"
#include "../lib/intr6811.h"

#pragma codeseg(COMMON)


unsigned short TotADValue2;
unsigned short TotADValue3;

unsigned short Cnt10msec = 0;

//******************************************************************************
interrupt void ScanKey(void)
{
static unsigned char Counter;
static unsigned char Loop10msec;
unsigned char MaskOC4;
static unsigned char KeyIOld = 0;
unsigned char i,KeyIndex;


  TOC3  = TCNT + KEY_SCAN_TIME;
  TMSK1 &= (0xff - OC3I);

  MaskOC4 = TMSK1 & OC4I;
  TMSK1 &= (0xff - OC4I);       /*disable scheduler interrupt */

  enable_interrupt();
  WatchDog();

  // Adjust counter 10 miliseconds
  if (++Loop10msec == 4) {
    ++Cnt10msec;
    Loop10msec = 0;
  }
  if (ADCTL & CC) {
    TotADValue2 = 0;
    TotADValue3 = 0;
    for (i=0; i < 5; i++) {
      TotADValue2 += ADR2;
      TotADValue3 += ADR3;
    }
    TotADValue2 /=5;
    TotADValue3 /=5;

    KeyIndex = 0;
    if (TotADValue2 >  220)                                 {KeyIndex = 1;}
    if (TotADValue3 >  220)                                 {KeyIndex = 2;}
    if (TotADValue2 < (130 + 10) && TotADValue2 > (130-10)) {KeyIndex = 3;}
    if (TotADValue3 < (130 + 10) && TotADValue3 > (130-10)) {KeyIndex = 4;}
    if (TotADValue2 < ( 80 + 10) && TotADValue2 > ( 80-10)) {KeyIndex = 5;}
    if (TotADValue3 < ( 80 + 10) && TotADValue3 > ( 80-10)) {KeyIndex = 6;}
    if (TotADValue2 < ( 60 + 10) && TotADValue2 > ( 60-10)) {KeyIndex = 7;}
    if (TotADValue3 < ( 60 + 10) && TotADValue3 > ( 60-10)) {KeyIndex = 8;}

    if (KeyIndex != KeyIOld) {Counter = 0; KeyIOld = KeyIndex;}
    if (KeyIndex) {Counter++;} else {if (Counter) {Counter--;}}

    if (Counter > 25) {
      Counter = 25;
      Key = KeyIndex;
    } else {
      Key = 0;
    }
  }
  disable_interrupt();
  TMSK1 |=  MaskOC4;
  TFLG1 = OC3F;
  TMSK1 |=  OC3I;

}


