#include "../key/disp.h"
#include "../key/key.h"
#include "../lib/eehc11.h"
#include "chkerr.h"

// =============================================================================
unsigned char CheckErrors( void )
{
  unsigned char rval = 0;
  
  if ( ErrorInfo & 0x03 ){
    rval = 1;
    // motor error -------------------------------------------------------------
    if ( KeyMode != KM_ERROR && ScreenMode != SM_ERROR ){
      KeyModeOld      = KeyMode;
      ScreenModeOld   = ScreenMode;
      ScreenNumberOld = ScreenNumber;
      KeyMode      = KM_ERROR;
      ScreenMode   = SM_ERROR; 
      ModeChange   = TRUE;
      if ( ErrorInfo & 0x0001 )ScreenNumber = 0;
      else                     ScreenNumber = 1;
    }
  }
  return rval;
}
