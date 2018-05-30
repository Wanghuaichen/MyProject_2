#include "../lib/eehc11.h"
#include "../lib/convert.h"               
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/rti_int.h"
#include "../lib/extport.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"

void HandleScreenPulseInfo(void)
{
  unsigned char   val;

  switch ( ScreenNumber ){
    case SCREEN_PULSE_INFO_0:
         val = 100 - EEPulseRatio;
         DispValue((unsigned char *)&val, _U1, 2, 0, READ_ONLY, POS_RIGHT);  
         DispValue((unsigned char *)&EEPulseRatio, _U1, 2, 0, READ_ONLY, POS_6);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_PULS_RATIO]), 5 );
      break;
    case SCREEN_PULSE_INFO_1:
         DispValue((unsigned char *)&EEPulseFrequency, _U1, 3, 0, READ_ONLY, POS_RIGHT);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_PULS_FREQ]), 5 );
    default:
      break;
  } 
} 
