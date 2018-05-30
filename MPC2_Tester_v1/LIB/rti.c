/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND           MAY 1994  */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*                                                              */
/*Description: Real Time Interrupt Initialisation               */
/*                                                              */
/*                                                              */
/****************************************************************/
/* include files                                                */

#include "rti_int.h"
#include "../appl/memolac.h"

#pragma codeseg(COMMON)


/****************************************************************/
/* RTI  Init Program                                            */
/****************************************************************/


 void InitRti(void)
{
  Cnt1sec = 0;
  Cnt500msec = 0;
  SwitchOffTime = 0;
  PerPresenceResultTime = 0;
} 

