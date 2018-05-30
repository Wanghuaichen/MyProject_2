#include "../key/disp.h"
#include "../key/key.h"
#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/rti_int.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/extport.h"
#include "../include/romeehc1.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/niveau.h"
#include "../appl/milk.h"
#include "../appl/take_off_sensor.h"
#include "../appl/takeoff.h"
#include "../appl/tandem.h"


void HandleScreenMilkInfo(void)
{
  unsigned short  DummyValue = 0;
  unsigned char   idx;

  idx = GetDisplaySide();

  switch (ScreenNumber) {
    case SCREEN_MILK_INFO_0:
         // the number of milkings the milk should be separated
         if (p_MlkRef[idx] != NULL) {
           DispValue( (LONG_COW_GEN_NUMBER ?
                       ((unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps) :
           		         ((unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps)),  _U1, 2, 0, WRITE, POS_RIGHT);
           if ( (LONG_COW_GEN_NUMBER ?
                       ((unsigned char *)((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps) :
           		         ((unsigned char *)((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps)) ) DispFlash = 0x03ff;
         } else {
           if (EEAddressCopy !=0) {
             // geen data aanwezig, zet toch even de pointer ivm read-only
             DispValue((unsigned char *)&DummyValue, _U1, 2, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[8]), '-', 2 );
           } else {
             // StandAlone milkseparation
             DispValue((unsigned char *)&MlkSepStandAlone, _U1, 1, 0, WRITE, POS_RIGHT);
           }
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MILK_SEP]), 6 );
      break;


    case SCREEN_MILK_INFO_1:
         // milk-test-value, the minimum expected milk-yield
         if (p_MlkRef[idx] != NULL) {
           // Omrekenen naar 10de kilogrammen
           LONG_COW_GEN_NUMBER ?
                  (DummyValue = ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount / 10) :
                  (DummyValue = ((   MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount / 10);
           DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_RIGHT);
         } else {
           // geen data aanwezig, zet toch even de pointer ivm atributen
           DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MILK_EXPECT]), 6 );
      break;
    case SCREEN_MILK_INFO_2:
         // colostrum-milk Yes or No
         if (p_MlkRef[idx] != NULL) {
           if ( LONG_COW_GEN_NUMBER ?
           	    (!((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps &&
           	      (DummyValue = ((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost)) :
           	  	(!((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps &&
           	  	  (DummyValue = ((   MlkRef **)p_MlkRef)[idx]->CowGenColost))) DispFlash = 0x03ff;
           DispValue((unsigned char *)(((unsigned char *)(&DummyValue))+1), _BL, 1, 0, READ_ONLY, POS_RIGHT);
         } else {
           // geen data aanwezig, zet toch even de pointer ivm atributen
           DispValue((unsigned char *)&DummyValue, _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[9]), '-', 1 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_COLOST]), 6 );
      break;
    case SCREEN_MILK_INFO_3:
         // total milk-time
         if ( idx == GetDisplaySide() ){
           DummyValue = (unsigned short)((MlkDurationCur * 10)/60);
         } else  {
           DummyValue = 0;
         }
         DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_RIGHT);

         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MILK_TIME]), 6 );
      break;
    case SCREEN_MILK_INFO_4:
         // maximaal gemeten melk-flow
         if ( idx == GetDisplaySide() ){
           DummyValue = (unsigned short)(MlkMaxFlow / 100);
         } else {
           DummyValue = 0;
         }
         DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_RIGHT);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MAX_MILK_SPEED]), 6 );
      break;
    case SCREEN_MILK_INFO_5:
         // gemiddeld gemeten melk-flow
         if ( idx == GetDisplaySide() && MlkAmountKg ){
           DummyValue = (unsigned short)((MlkAmountKg*60) / MlkDurationCur );
         } else {
           DummyValue = 0;
         }
         DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_RIGHT);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_AVE_MILK_SPEED]), 6 );
      break;
    case SCREEN_MILK_INFO_6:
         // actuele gemeten melk-flow
         if ( idx == GetDisplaySide() ){
           DummyValue = (unsigned short)(MlkActFlow / 100);
         } else {
           DummyValue = 0;
         }
         DispValue((unsigned char *)&DummyValue, _U2, 3, 1, READ_ONLY, POS_RIGHT);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ACT_MILK_SPEED]), 6 );
      break;
    case SCREEN_MILK_INFO_7:
         if (p_MlkRef[idx] != NULL) {
           DispValue( (LONG_COW_GEN_NUMBER ?
                       ((unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease) :
           		         ((unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease)),  _BL, 1, 0, WRITE, POS_RIGHT);
         } else {
           // geen data aanwezig, zet toch even de pointer ivm read-only
           DispValue((unsigned char *)&DummyValue, _BL, 1, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[9]), '-', 1 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MILK_MANUAL_SET]), 7 );
      break;
    case SCREEN_MILK_INFO_MAX:
         DispValue((unsigned char *)&PulsWidthMeas, _U2, 4, 0, READ_ONLY, POS_7);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_LEVEL_COUNTER]), 6 );
      break;
  } // end switch( ScreenNumbers, SM_MILK_INFO )
}
