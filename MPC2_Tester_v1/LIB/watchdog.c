/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*  MODULE WatchDog                                             */
/*                                                              */
/****************************************************************/

#include "io6811.h"


#pragma codeseg(COMMON)

non_banked void WatchDog(void)
{
  COPRST = 0x55;
  COPRST = 0xAA;
}
