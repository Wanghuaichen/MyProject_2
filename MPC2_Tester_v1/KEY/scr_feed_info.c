#include "../key/key.h"
#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/extport.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/take_off_sensor.h"



void HandleScreenFeedInfo(void )
{
  unsigned short  DummyValue;
  unsigned short  Value;
  unsigned char   idx;

  idx = GetDisplaySide();

  switch ( ScreenNumber ){
    case SCREEN_FEED_INFO_0:
    	   if ( EEDummy1 == 0){
           if ( p_CowFedParlRef[idx] != NULL && p_MlkRef[idx] != NULL ){
             ExtraFeedDpl = (signed int)(ExtraFeed[idx] + LONG_COW_GEN_NUMBER ?
                            ((((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance) :
                            ((((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance));
             if ( p_CowFedParlRes[idx] != NULL ){
               ExtraFeedDpl -= LONG_COW_GEN_NUMBER ?
                              ( ((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed :
                              ( ((   CowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed;
             }
             ExtraFeedDpl /= 100;
             DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_RIGHT);
           } else {
             if ( p_CowFedParlRes[idx] != NULL ){
               ExtraFeedDpl = (signed int)( ExtraFeed[idx] - LONG_COW_GEN_NUMBER ?
                              ((((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed) :
                              ((((   CowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed))/100;
             } else {
               ExtraFeedDpl = (signed int)( ExtraFeed[idx]/100);
             }
             DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_RIGHT);
           }
         } else {
           // feeding milking parlour done by 'VELOS'
           ExtraFeedDpl = (signed int)( ExtraFeed[idx]/100);
           DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_RIGHT);
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_EXTRA]), 6 );

      break;
    case SCREEN_FEED_INFO_1:
         if ( EEAddressCopy ){
           if ( ParlourType == PT_CARROUSEL ){
             if ( p_CowFedParlInfoMail[idx] != NULL && p_MlkRef[idx] != NULL ){
               DispValue((unsigned char *)&CowFedParlTotalFed, _U2, 3, 1, READ_ONLY, POS_RIGHT);
             } else {
               // geen data aanwezig, zet toch even de pointer ivm atributen
               DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
               memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
             }
           } else {
             if ( p_CowFedParlRef[idx] != NULL && p_MlkRef[idx] != NULL ){
               Value = (LONG_COW_GEN_NUMBER ?
                        (((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance :
                        (((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance)/100;
               DispValue((unsigned char *)&Value,      _U2, 3, 1, READ_ONLY, POS_RIGHT);
             } else{
               // geen data aanwezig, zet toch even de pointer ivm atributen
               DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
               memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
             }
           }
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_MILK]), 6 );
         } else {
           if ( EEFeeding ){
             // stand-alone, set item tbv Min/Max value
             Item = FeedSpeedEE;
             DispValue((unsigned char *)&EEFeedSpeed, _U2, 4, 0, WRITE, POS_RIGHT);
           } else {
             // geen data aanwezig, zet toch even de pointer ivm atributen
             DispValue((unsigned char *)&DummyValue, _U2, 4, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[6]), '-', 4 );
           }
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_FEED_SPEED]), 6 );
         }
      break;
    case SCREEN_FEED_INFO_2:
         if ( EEAddressCopy ){
           // voer totaal alle 5 voersoorten, exclusief melkstal
           if ( p_CowFedInfoMail[idx] != NULL && p_MlkRef[idx] != NULL ){
             DispValue((unsigned char *)&CowFedTotal, _U2, 3, 1, READ_ONLY, POS_RIGHT);
           } else {
             // geen data aanwezig, zet toch even de pointer ivm atributen
             DispValue((unsigned char *)&DummyValue,  _U2, 3, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_TOTAL]), 6 );
         } else {
           if ( EEFeeding ){
             // stand-alone, set item tbv Min/Max value
             if ( SwingOver() && (idx == SIDE_B) ){
               // stand-alone, swing-over: SIDE_B
               Item = SB_FeedMtrPortionEE;
               DispValue((unsigned char *)&(EEFeedMtrPortion[SIDE_B]), _U2, 4, 0, WRITE, POS_RIGHT);
             } else {
               Item = FeedMtrPortionEE;
               DispValue((unsigned char *)&(EEFeedMtrPortion[SIDE_A]), _U2, 4, 0, WRITE, POS_RIGHT);
             }
           } else {
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }

           if ( SwingOver() && (idx == SIDE_B) ){
             // stand-alone, swing-over: SIDE_B
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_SB_FEED_PORTION]), 6 );
           } else {
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_FEED_PORTION]), 6 );
           }
         }
      break;
    case SCREEN_FEED_INFO_3:
         if ( EEAddressCopy ){
           // voertegoed alle 5 voersoorten op dit moment, exclusief melkstal
           if ( p_CowFedInfoMail[idx] != NULL && p_MlkRef[idx] != NULL ){
             DispValue((unsigned char *)&CowFedBalance, _U2, 3, 1, READ_ONLY, POS_RIGHT);
           } else{
             // geen data aanwezig, zet toch even de pointer ivm atributen
             DispValue((unsigned char *)&DummyValue,    _U2, 3, 0, READ_ONLY, POS_RIGHT);
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_BALANCE]), 6 );
         } else {
           if ( EEFeeding ){
             // stand-alone, set item tbv Min/Max value
             if ( SwingOver() && (idx == SIDE_B) ){
               // stand-alone, swing-over: SIDE_B
               Item = SB_FeedBalanceDefEE;
               DispValue((unsigned char *)&(EEFeedBalanceDef[SIDE_B]), _U2, 4, 0, WRITE, POS_RIGHT);
             } else {
               Item = FeedBalanceDefEE;
               DispValue((unsigned char *)&(EEFeedBalanceDef[SIDE_A]), _U2, 4, 0, WRITE, POS_RIGHT);
             }
           } else {
             memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
           }
           if ( SwingOver() && (idx == SIDE_B) ){
               // stand-alone, swing-over: SIDE_B
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_SB_FEED_BALANCE_DEF]), 6 );
           } else {
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_FEED_BALANCE_DEF]), 6 );
           }
         }
      break;
    case SCREEN_FEED_INFO_4:
         // display aantal calibratie porties
         DispValue((unsigned char *)&(nr_of_cal_feed_portions), _U1, 2, 0, READ_ONLY, POS_RIGHT);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_FEED_CAL]), 6 );
      break;
    default:
      break;
  } // end switch ( ScreenNumbers, SM_FEED_INFO )
}