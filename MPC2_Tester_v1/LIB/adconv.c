// -----------------------------------------------------------------------------
//                                                                            
// (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   
//     ALL RIGHTS RESERVED                                                    
//                                                                            
// -----------------------------------------------------------------------------

#include "../lib/io6811.h"
#include "../lib/boolean.h"
#include "../lib/adconv.h"


// Global definitions ==========================================================
#define CCF  0x80  // Conversion Completed Flag in ADCTL register

#pragma codeseg(COMMON)
 
// =============================================================================
unsigned char ADConv1Of8( unsigned char Channel)
{
  unsigned int DigitalValue;

  Channel&= 0x0f;  /* SCAN and MULT-bit must be 0 */
  ADCTL= Channel;  /* starts conversion */

  while ( ( ADCTL & CCF) == FALSE);  /* wait until conversion ready */

  DigitalValue= ( ( ( unsigned char) ADR1 + ( unsigned char) ADR2 +
                    ( unsigned char) ADR3 + ( unsigned char) ADR4) / 4);

  return ( unsigned char) DigitalValue;

}


// =============================================================================
unsigned char ADConv( unsigned char Channel)
{
  Channel&= 0x0f;               // SCAN and MULT-bit must be 0  ----------------
  ADCTL= Channel;               // starts conversion  --------------------------

  while ( ( ADCTL & CCF) == FALSE);  // wait until conversion ready ------------

  return ( unsigned char) ADR1;

}  /* ADConv1Of8 */



