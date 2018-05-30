/* -------------------------------------------------------------------------- */
/*                                                                            */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   */
/*     ALL RIGHTS RESERVED                                                    */
/*                                                                            */
/* file:        eeprom.c                                                      */
/*                                                                            */
/* description: Controlling the eeprom memory part of the processor           */
/*                                                                            */
/*                                                                            */
/* -------------------------------------------------------------------------- */

/* include files ------------------------------------------------------------------------------*/

#include "../lib/io6811.h"                             
#include "../lib/time.h"
#include "../lib/int.h"
#include "../lib/eeprom.h" 
#include "../key/keyint.h"


#pragma codeseg(EEPAGE)

// functions -------------------------------------------------------------------

// =============================================================================
void ProgWait(void)
{
  unsigned int Time;
  // wacht 10 msec voor het programmeren van een eeprom adres ------------------
  Time = TCNT + ProgTime;
  while (!(TimeControl(TCNT, Time)));
  
}


// =============================================================================
unsigned char WriteEEpromByte(unsigned char value, unsigned char *address)
{
  unsigned char OldIntLevel, i=0;

  // controleer of de waarde op het adres verschilt van de te schrijven waarde -
  if (*address != value) {
 
    // controleer of er eerst gewist moet worden -------------------------------
    if ( ( *address & value) != value) {
      // wis het byte op het adres ---------------------------------------------
      Interrupts_Off(&OldIntLevel);
      PPROG = 0x16;
      *address = 0xff;
      PPROG = 0x17;
      ProgWait();
      PPROG = 0x00;
      Interrupts_On(&OldIntLevel);
    }

    // schrijf waarde op het adres ---------------------------------------------
    Interrupts_Off(&OldIntLevel);
    PPROG = 0x02;
    *address = value;
    PPROG = 0x03;
    ProgWait();
    PPROG = 0x00;
    Interrupts_On(&OldIntLevel);
    return 1;
  }
  else {
    return 0;
  }
}
// =============================================================================
unsigned char WriteEEpromShort(unsigned short value, unsigned short *address)
{
  unsigned char r_val = 0;
   
  r_val |= WriteEEpromByte((unsigned char) value, (unsigned char *) address+1);
  r_val |= WriteEEpromByte((unsigned char) (value >> 8), (unsigned char *) address );
  return r_val;

}



// =============================================================================
void EraseEEprom(void)
{
  unsigned char OldIntLevel;

//disable_interrupt();
  Interrupts_Off(&OldIntLevel);
  // wis de gehele eeprom adres ------------------------------------------------
  PPROG = 0x06;
  *((unsigned char *)0xB600) = 0x06;
  PPROG = 0x07;
  ProgWait();
  PPROG = 0x00;
  Interrupts_On(&OldIntLevel);
//enable_interrupt();
}
