#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"

void HandleScreenCowInfo( void )
{
  CowSepInfoRes  *CowSepInfoResMailPtr;
  unsigned short  DummyValue;
  unsigned char   i, idx;

  idx = GetDisplaySide();

  switch ( ScreenNumber ){
    case SCREEN_COW_INFO_0:
         if ( p_CowActMemAttRef[idx] != NULL && (LONG_COW_GEN_NUMBER ?
         	  (((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt) :
         	  (((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt))){
           // TWEE UURS ACTIVITEITS METER
           if ( LONG_COW_GEN_NUMBER ? ((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt :
           	                          ((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt ){
             DispFlash = 0x03ff;
             // laat afwijking plus bijbehorende tijden zien
             if ( sub_screen_pntr == 0 ){
               DispValue(LONG_COW_GEN_NUMBER ?
               	         (unsigned char *)&((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerSrtTime :
               	         (unsigned char *)&((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerSrtTime, _TM, 4, 2, READ_ONLY, POS_2);
               DispValue(LONG_COW_GEN_NUMBER ?
                         (unsigned char *)&((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerEndTime :
                         (unsigned char *)&((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerEndTime, _TM, 4, 2, READ_ONLY, POS_7);
               DispTable[0] = 'T'; DispTable[5] = '-';
             } else {
               // display laaste tien periodes; X = att, ? = verdacht, - = geen attentie
               for ( i = 0; i < 10; i++ ){
                 switch ( LONG_COW_GEN_NUMBER ?
                       (*(signed short *)(&(((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev01)+i)):
                 	     (*(signed short *)(&(((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev01)+i)) ){
                   case 2:  DispTable[i] = '?'; break;       // suspicous
                   case 1:  DispTable[i] = 'X'; break;       // attention
                   default: DispTable[i] = '-'; break;       // no attention
                 }
               }
               // add point
               if ( (LONG_COW_GEN_NUMBER ?
               	   ((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev :
               	   ((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev) <= 10 ){
                 disp_point = 0x0200 >> (10-(LONG_COW_GEN_NUMBER ?
                     (((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev):
                 	   (((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerDev)));
               }
             }
           }
         } else {
           // display activity: last activity / rolling average  STANDAARD ACTIVITEITS METER
           if (p_CowActRef[idx] != NULL && p_MlkRef[idx] != NULL ) {
             DispValue(LONG_COW_GEN_NUMBER ?
                  (unsigned char *)&((ExtCowActRef **)p_CowActRef)[idx]->CowActDeviation :
                  (unsigned char *)&((   CowActRef **)p_CowActRef)[idx]->CowActDeviation, _S2, 3, 0, READ_ONLY, POS_RIGHT);
           } else {
             DispValue((unsigned char *)&DummyValue, _S2, 3, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ACT]), 6 );
         }
      break;

    case SCREEN_COW_INFO_1:
         // separation function-number
         if ( p_CowSepInfoResMail[idx] && CowSepNrOfItems && p_MlkRef[idx] != NULL ){
           CowSepInfoResMailPtr = (void *)(&p_CowSepInfoResMail[idx]->message_start);
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&(((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations :
             (unsigned char *)&(((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations, _U1, 1, 0, WRITE, POS_RIGHT);
           if ( (LONG_COW_GEN_NUMBER ?
           	  (((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations :
           	  (((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations) > 9 ){
             // er is maar 1 digit, dus indien aantal groter dan 9 dan toch 9 op display.
             memory_set((unsigned char *)&(DispTable[0]), ' ', 9 );
             DispTable[9] = '9';
           }
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&(((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction :
           	 (unsigned char *)&(((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction, _U1, 2, 0, WRITE, POS_7);
           if ( (LONG_COW_GEN_NUMBER ?
           	  (((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction :
           	  (((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction) < 10 ){
             DispTable[6] = ' ';
           }
         } else {
           // geen data aanwezig, set even de pointer
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&(((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[0].CowSepNrOfSeparations :
             (unsigned char *)&(((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[0].CowSepNrOfSeparations, _U1, 1, 0, WRITE, POS_RIGHT);
           DispTable[7] = DispTable[9] = '-';
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_SEP_NR]), 6 );
      break;
    case SCREEN_COW_INFO_2:
         // next group-number
         if (p_CowGenRef[idx] != NULL && p_MlkRef[idx] != NULL ) {
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&((ExtCowGenRef **)p_CowGenRef)[idx]->CowGenNextGrpNumber :
             (unsigned char *)&((   CowGenRef **)p_CowGenRef)[idx]->CowGenNextGrpNumber, _U2, 2, 0, WRITE, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&DummyValue, _U2, 2, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[8]), '-', 2 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_GROUP]), 6 );
      break;
    case SCREEN_COW_INFO_3:
         // responder-number
         if ( p_CowGenRef[idx] != NULL && p_MlkRef[idx] != NULL ) {
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&((ExtCowGenRef **)p_CowGenRef)[idx]->CowGenRespNumber :
             (unsigned char *)&((   CowGenRef **)p_CowGenRef)[idx]->CowGenRespNumber, _U2, 5, 0, READ_ONLY, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&DummyValue, _U2, 5, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[5]), '-', 5 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_RESP]), 5 );
      break;
/*
    case SCREEN_COW_INFO_4:
         // responder-number, andere stand (swing-over)
         if (p_MlkRef[idx == 0 ? 1 : 0] != NULL) {
           DispValue((unsigned char *)&p_MlkRef[idx == 0 ? 1 : 0]->CowGenNumber, _U2, 5, 0, WRITE, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&DummyValue, _U2, 5, 0, READ_ONLY, POS_LEFT);
           memory_set((unsigned char *)&(DispTable[5]), '-', 5 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_COW]), 5 );
      break;
*/
  } // end switch (ScreenNumber, SM_COW_INFO )
}