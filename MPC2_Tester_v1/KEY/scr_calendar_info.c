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


void HandleScreenCalendarInfo( void )
{
  unsigned short  DummyValue;
  unsigned short  Value;
  unsigned char   idx;

  idx = GetDisplaySide();

  // calendar-screens
  switch ( ScreenNumber ){
    case SCREEN_CALENDAR_INFO_0:
         // lactation
         if (p_MlkRef[idx] != NULL ){
           if( (LONG_COW_GEN_NUMBER ?
           	    ((ExtMlkRef **)p_MlkRef)[idx]->CowGenCalvingDate :
           	    ((   MlkRef **)p_MlkRef)[idx]->CowGenCalvingDate)) {
             Value = AbsDate - (LONG_COW_GEN_NUMBER ?
                                ((ExtMlkRef **)p_MlkRef)[idx]->CowGenCalvingDate :
                                ((   MlkRef **)p_MlkRef)[idx]->CowGenCalvingDate);
             DispValue((unsigned char *)&Value, _U2, 3, 0, READ_ONLY, POS_RIGHT);
           } else {
             DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }
         } else {
           // geen data, zet pointer ivm item-type
           DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_LACT]), 6 );
      break;
    case SCREEN_CALENDAR_INFO_1:
         // heat, CowGenHeatDate = AbsDate - p_MlkRef[idx]->CowGenHeatDate
         if (p_MlkRef[idx] != NULL ){
           if ( LONG_COW_GEN_NUMBER ?
           	    ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatAttent:
           	    ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatAttent )DispFlash = 0x03ff;

           if ( CowGenHeatDate[idx] == AbsDate ){
             // datum is met de hand ingevuld
             CowGenHeatDateDpl = 0;
             DispValue((unsigned char *)&CowGenHeatDateDpl, _U2, 3, 0, WRITE, POS_RIGHT);
           } else {
             if ( LONG_COW_GEN_NUMBER ?
             	    ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate :
             	    ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate ){
               CowGenHeatDateDpl = AbsDate - (LONG_COW_GEN_NUMBER ?
                                             ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate :
               	                             ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate);
               DispValue((unsigned char *)&CowGenHeatDateDpl, _U2, 3, 0, WRITE, POS_RIGHT);
             } else {
               // geen tocht data
               CowGenHeatDateDpl = 0;
               DispValue((unsigned char *)&CowGenHeatDateDpl, _BL, 1, 0, WRITE, POS_RIGHT);
             }
           }
         } else {
           // geen data aanwezig, set even de pointer
           DispValue((unsigned char *)&CowGenHeatDateDpl, _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_HEAT]), 6 );
      break;
    case SCREEN_CALENDAR_INFO_2:
         // Insemination
         if (p_MlkRef[idx] != NULL ){
           if ( LONG_COW_GEN_NUMBER ?
           	   ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemAttent :
           	   ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemAttent  ) DispFlash = 0x03ff;
           if ( CowGenInsemDate[idx] == AbsDate ){
             // datum is met de hand ingevuld
             CowGenInsemDateDpl = 0;
             DispValue((unsigned char *)&CowGenInsemDateDpl, _U2, 3, 0, WRITE, POS_RIGHT);
           } else {
             if ( LONG_COW_GEN_NUMBER ?
             	    ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate :
             	    ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate ){
               CowGenInsemDateDpl = AbsDate - ( LONG_COW_GEN_NUMBER ?
                                              ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate :
                                              ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate);
               DispValue((unsigned char *)&CowGenInsemDateDpl, _U2, 3, 0, WRITE, POS_RIGHT);
             } else {
               // geen inseminatie data
               CowGenInsemDateDpl = 0;
               DispValue((unsigned char *)&CowGenInsemDateDpl, _BL, 1, 0, WRITE, POS_RIGHT);
             }
           }
         } else {
           // geen data aanwezig, set even de pointer
           DispValue((unsigned char *)&CowGenInsemDateDpl, _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_INSEM]), 6 );
      break;
    case SCREEN_CALENDAR_INFO_3:
         // Incalf
         if (p_MlkRef[idx] != NULL ){
           if ( LONG_COW_GEN_NUMBER ?
           	      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent :
           	      ((   MlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent ) DispFlash = 0x03fff;
           // initieer CowGenReproState
           if ( CowGenReproState[idx] == 0xff ) CowGenReproState[idx] =
           	                  (LONG_COW_GEN_NUMBER ?
           	                  ((ExtMlkRef **)p_MlkRef)[idx]->CowGenReproState  :
           	                  ((   MlkRef **)p_MlkRef)[idx]->CowGenReproState) == 3 ? 1 : 0;
           DispValue((unsigned char *)&(CowGenReproState[idx]), _BL, 1, 0, WRITE, POS_RIGHT);
         } else {
           // geen data aanwezig, set even de pointer
           DispValue((unsigned char *)&(CowGenReproState[idx]), _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[9]), '-', 1 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_INCALF]), 6 );
      break;
    case SCREEN_CALENDAR_INFO_4:
         // Dry, weergeven als een BOOLEAN
         if (p_MlkRef[idx] != NULL ){
           if ( LONG_COW_GEN_NUMBER ?
           	       ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent :
           	       ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent ) DispFlash = 0x03fff;

           if ( CowGenDryOffDate[idx] == AbsDate ){
             // datum is met de hand ingevuld (256 om MSB != 0 te maken )
             CowGenDryOffDateDpl = 256;
             DispValue((unsigned char *)&CowGenDryOffDateDpl, _BL, 1, 0, WRITE, POS_RIGHT);
           } else {
             if ( LONG_COW_GEN_NUMBER ? ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate :
             	                          ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate ){
               CowGenDryOffDateDpl = AbsDate - ( LONG_COW_GEN_NUMBER ?
                                         ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate :
                                         ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate);
               DispValue((unsigned char *)&CowGenDryOffDateDpl, _U2, 3, 0, WRITE, POS_RIGHT);
             } else {
               // geen inseminatie data
               CowGenDryOffDateDpl = 0;
               DispValue((unsigned char *)&CowGenDryOffDateDpl, _BL, 1, 0, WRITE, POS_RIGHT);
             }
           }
         } else {
           // geen data aanwezig, set even de pointer
           DispValue((unsigned char *)&CowGenDryOffDateDpl, _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_DRY]), 6 );
      break;
    default:
      break;
  }
}