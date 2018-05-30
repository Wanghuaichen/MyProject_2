/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*  MODULE Delay                                                */
/*                                                              */
/****************************************************************/




#include "../lib/time.h"
#include "../lib/io6811.h"
#include "../lib/watchdog.h"
#include "../key/keyint.h"


#pragma codeseg(EEPAGE)

/*******************************************************************************************/
/* Delay routine                                                                           */
/*******************************************************************************************/

void Delay(unsigned short Time)
{
unsigned short TimeOut;
unsigned short msecs;

  /* 1 TCNT tik is 500ns ------------------------------------------------------------------*/
  /* 1 Counter tik is 32ms-----------------------------------------------------------------*/
  /* 1 Counter_10ms tik is 10ms------------------------------------------------------------*/

  msecs = (Time%10) * 2000;

  if ( Time >= 10 ) {
    TimeOut = (Cnt10msec+ (Time / 10));
    while ( !TimeControl(Cnt10msec, TimeOut) ) WatchDog();
  }

  if ( msecs ) {
    TimeOut = TCNT + msecs;
    while ( !TimeControl( TCNT, TimeOut) ) WatchDog();
  }
}


