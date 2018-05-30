/* -------------------------------------------------------------------------- */
/*                                                                            */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   */
/*     ALL RIGHTS RESERVED                                                    */
/*                                                                            */
/* description: Flow Check                                                    */
/*                                                                            */

/* include files --------------------------------------------------------------*/
#include "../comm/flowrcv.h"

#pragma codeseg(RCODE)

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
non_banked void HandleMess_Non_Banked(void)
{                         
  HandleMess();
}
