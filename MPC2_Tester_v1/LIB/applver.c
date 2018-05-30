/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*  MODULE Application Versions                                 */
/*                                                              */
/****************************************************************/



#include "../lib/applver.h"
#include "../include/romver.h"

unsigned short ApplVersion;
unsigned short RomVersion;

#pragma codeseg(COMMON)

/****************************************************************/
/* APPL VERSION                                                  */
/****************************************************************/

const unsigned long ApplProgVersion = VERSION(0,109);

/****************************************************************/
/* SET  VERSIONS                                                 */
/****************************************************************/

void SetVersion(void) {
   ApplVersion = ((ApplProgVersion / 0x10000)* 100)+ (ApplProgVersion % 0x100);
   RomVersion  = ((RomProgVersion  / 0x10000)* 100)+ (RomProgVersion  % 0x100);
}


#pragma memory=constseg(ROMVER)
/****************************************************************/
/* APPL COMPATIBLE VERSIONS                                      */
/****************************************************************/

const unsigned long CompRomVersion[] = {
  0x23242520, 0x524f4d20, 0x25242300,     /* string #$% ROM %$#  */
  0x434E4D34, 0x4D414520,                 /* string 'CNM4MAE ' */
  VERSION(00,04),
  VERSION(00,05),
  0xffffffff
  };



const unsigned char MemoryMap[] = {
  0x28, /*Test string (<[ INF ]>) */
  0x3C,
  0x5B,
  0x20,
  0x49,
  0x4E,
  0x46,
  0x20,
  0x5D,
  0x3E,
  0x29,
  0x00,

  0x40, /*EEBankSize*/
  0x00,
  0x00, /*EEBankStart*/
  0x00,
  0x07, /*EENrOffEEBanks, PAGES to be erased, one page is four banks*/
  0x00, /*StartPROGBank*/
  0x00, /*StartDBMSBank*/
  0x00, /*dummy*/
  0xff,
  0xff,
  0xff,
  0xff
  };



