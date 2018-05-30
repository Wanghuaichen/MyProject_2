#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/extport.h"
#include "../comm/flowctrl.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/conduct.h"
#include "../appl/temp.h"



/*
** HandleScreenDiseaseInfo
*/
void HandleScreenDiseaseInfo( void )
{
  unsigned char  *p_a;
  void           *CowDiseaseRefPtr;
  void           *CowDiseaseResPtr;
  unsigned short  DummyValue;
  unsigned char   m_averige, mp, i, d_val, p_val;
  unsigned char   idx;
    signed short  Value = 0;

  idx = GetDisplaySide();

  // conductivity-screens
  switch ( ScreenNumber ){
    case SCREEN_DISEASE_0:
         // display ziekte-codes
         if ( p_CowDiseaseMail[idx] != NULL &&  p_MlkRef[idx] != NULL ){
           DispFlash = 0x03fff;
           CowDiseaseRefPtr = (void *)(&p_CowDiseaseMail[idx]->message_start);
           i = LONG_COW_GEN_NUMBER ?
              ((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array[disease_number].CowDiseaseCode:
               ((   CowDiseaseRef *)CowDiseaseRefPtr)->Array[disease_number].CowDiseaseCode;
           switch(i ){
             case 94:
             case 93:
             case 92:
             case 91:
                      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_IC_RF+i-91]), 4 );
               break;
             default: DispValue(LONG_COW_GEN_NUMBER ?
                        (unsigned char *)&(((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array[disease_number].CowDiseaseCode) :
                        (unsigned char *)&(((   CowDiseaseRef *)CowDiseaseRefPtr)->Array[disease_number].CowDiseaseCode), _U1, 2, 0, READ_ONLY, POS_RIGHT);
                      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_IC_ALL]), 8 );
               break;
           }
         }
      break;

    case SCREEN_DISEASE_1:
    case SCREEN_DISEASE_2:
    case SCREEN_DISEASE_3:
    case SCREEN_DISEASE_4:
         if (EEConMeasuring && ( SoftKey & SK_COND_TMP_ENABLE) && ( idx == GetDisplaySide() )) {
           // display the actual conductivity averages, percentage to minimum
           // check for minimum conductivity
           if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
             if ( HVC_Buffer[0].average ){
               DummyValue = ( LONG_COW_GEN_NUMBER ?
                   (((ExtCowConRef **)p_CowConRef)[idx]->CowConRollingAverage1) :
                   (((   CowConRef **)p_CowConRef)[idx]->CowConRollingAverage1));
               if ( HVC_Buffer[0].average > DummyValue ){
                 Value = (((signed int)((int)HVC_Buffer[0].average - (int)DummyValue)*100)/DummyValue);
               } else {
                 Value = (((signed int)((int)DummyValue-(int)HVC_Buffer[0].average)*100)/DummyValue);
                 Value = 0-Value;
               }
             } else {
               Value = 0;
             }
             DispValue((unsigned char *)&(Value), _S2, 3, 0, READ_ONLY, POS_RIGHT);
           } else {
             m_averige = 255; mp = 0;
             for ( i = 0; i < 4; i++ ){
               if ( HVC_Buffer[i].average < m_averige ){
                 m_averige = HVC_Buffer[i].average; mp = i;
               }
             }
             if ( (ScreenNumber-SCREEN_DISEASE_1) == mp || m_averige == 0 ){
               Value = 0;
             } else {
               Value = ((( HVC_Buffer[ScreenNumber-SCREEN_DISEASE_1].average) - (HVC_Buffer[mp].average))*100)/ (HVC_Buffer[mp].average);
             }
             d_val = (unsigned char)Value;

             // display the previous conductivity averages, percentage to minimum
             // check for minimum conductivity
             if ( p_CowConRef[idx] != NULL ){
               m_averige = 255; mp = 0;

               p_a = LONG_COW_GEN_NUMBER ?
                    (unsigned char *)&(((ExtCowConRef **)p_CowConRef)[idx]->CowConDataValue1) :
                    (unsigned char *)&(((   CowConRef **)p_CowConRef)[idx]->CowConDataValue1);
               for ( i = 0; i < 4; i++ ){
                 if ( (p_a[i]) < m_averige ){
                   m_averige = (p_a[i]); mp = i;
                 }
               }
               if ( (ScreenNumber-SCREEN_DISEASE_1) == mp || m_averige == 0 ){
                 Value = 0;
               } else {
                 Value = (((p_a[ScreenNumber-SCREEN_DISEASE_1]) - (p_a[mp]))*100)/ (p_a[mp]);
               }
               p_val = (unsigned char)Value;
             } else {
               // no previous data
               p_val = 0;
             }
             DispValue((unsigned char *)&(d_val), _U1, 2, 0, READ_ONLY, POS_RIGHT);
             DispValue((unsigned char *)&(p_val), _U1, 2, 0, READ_ONLY, POS_6);
           }
         } else {
           Value = 0;
           DispValue((unsigned char *)&(Value), _U1, 2, 0, READ_ONLY, POS_RIGHT);
           memory_set( (unsigned char *)&(DispTable[0]), ' ', 8 );
           memory_set( (unsigned char *)&(DispTable[5]), '-', 2 );
           memory_set( (unsigned char *)&(DispTable[8]), '-', 2 );
         }
         if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CON]), 4 );
         } else {
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CV_RF+ScreenNumber-1]), 4 );
         }

         if ( idx == GetDisplaySide() ){
           if ( ( ConDataAttentions & (0x01 << (ScreenNumber-1))) ||
              ( p_CowConRef[idx]    &&
               ( LONG_COW_GEN_NUMBER ?
               ((ExtCowConRef **)p_CowConRef)[idx]->CowConDataAttentions :
               ((   CowConRef **)p_CowConRef)[idx]->CowConDataAttentions ) & (0x01 << (ScreenNumber-1))) ) DispFlash = 0x3fff;
         }
      break;

    case SCREEN_DISEASE_5:
         // temperature-screens
         if (EETempMeasuring && ( SoftKey & SK_COND_TMP_ENABLE) && (idx == SideState)) {
           DispValue((unsigned char *)&(MaxCowTemperature), _U2, 3, 1, READ_ONLY, POS_RIGHT);
           if ( CowTempAttention ) DispFlash = 0x3fff;
         } else {
           Value = 0;
           DispValue((unsigned char *)&(Value), _U1, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set( (unsigned char *)&(DispTable[0]), ' ', 7 );
           memory_set( (unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_TMP]), 6 );
      break;

    case SCREEN_DISEASE_6:
         // temperature test-value, maximum temperature before generating an attention
         if ( p_CowTmpRef[idx] != NULL && p_MlkRef[idx] != NULL ){
           DispValue(LONG_COW_GEN_NUMBER ?
             (unsigned char *)&(((ExtCowTmpRef **)p_CowTmpRef)[idx]->CowTmpMaximum) :
             (unsigned char *)&(((   CowTmpRef **)p_CowTmpRef)[idx]->CowTmpMaximum), _U2, 3, 1, READ_ONLY, POS_RIGHT);
         } else {
           // geen data aanwezig, zet toch even de pointer ivm atributen
           DispValue((unsigned char *)&DummyValue, _U2, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_TMPT]), 6 );
      break;

    case SCREEN_DISEASE_7:
    case SCREEN_DISEASE_8:
    case SCREEN_DISEASE_9:
    case SCREEN_DISEASE_10:
         if (EEConMeasuring && ( SoftKey & SK_COND_TMP_ENABLE) && (idx == SideState)) {
           // display the actual conductivity averages
           if ( MilkMeterType == MMT_TAKE_OFF_SENSOR ){
             p_a = (unsigned char *)(ConductivityBuffer);
             mp = *(p_a+601);
             DispValue((unsigned char *)&mp, _U1, 3, 0, READ_ONLY, POS_RIGHT);
           } else {
             DispValue((unsigned char *)&(HVC_Buffer[ScreenNumber-7].average), _U1, 3, 0, READ_ONLY, POS_RIGHT);
           }
         } else {
           Value = 0;
           DispValue((unsigned char *)&(Value), _U1, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set( (unsigned char *)&(DispTable[0]), ' ', 7 );
           memory_set( (unsigned char *)&(DispTable[7]), '-', 3 );
         }
         if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ACON]), 5 );
         } else {
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ACV_RF+ScreenNumber-7]), 6 );
         }
      break;

    case SCREEN_DISEASE_11:
    case SCREEN_DISEASE_12:
    case SCREEN_DISEASE_13:
    case SCREEN_DISEASE_14:
         if (EEConMeasuring && ( SoftKey & SK_COND_TMP_ENABLE) && (idx == SideState)) {
           // display the actual conductivity averages
           if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
             Value = ( LONG_COW_GEN_NUMBER ?
                   (((ExtCowConRef **)p_CowConRef)[idx]->CowConRollingAverage1) :
                   (((   CowConRef **)p_CowConRef)[idx]->CowConRollingAverage1));
           } else {
             p_a = LONG_COW_GEN_NUMBER ?
                    (unsigned char *)&(((ExtCowConRef **)p_CowConRef)[idx]->CowConRollingAverage1) :
                    (unsigned char *)&(((   CowConRef **)p_CowConRef)[idx]->CowConRollingAverage1);

             Value = *(p_a+ScreenNumber-11);
           }
           DispValue((unsigned char *)&(Value), _U2, 3, 0, READ_ONLY, POS_RIGHT);
         } else {
           Value = 0;
           DispValue((unsigned char *)&(Value), _U1, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set( (unsigned char *)&(DispTable[0]), ' ', 7 );
           memory_set( (unsigned char *)&(DispTable[7]), '-', 3 );
         }
         if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ARV]), 5 );
         } else {
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_ARV_RF+ScreenNumber-11]), 6 );
         }
      break;

    case SCREEN_DISEASE_15:
         if (p_MlkRef[idx] != NULL) {
           CowDiseaseResPtr = (void *)(&(CowDisease[idx]));
           DispValue( LONG_COW_GEN_NUMBER ?
             (unsigned char *)&(((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode) :
             (unsigned char *)&(((   CowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode), _U1, 2, 0, WRITE, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&(DummyValue), _U1, 2, 0, READ_ONLY, POS_RIGHT);
           memory_set( (unsigned char *)&(DispTable[8]), '-', 2 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_IC_TRT]), 7 );
      break;
  } //  end switch( ScreenNumbers SM_DECEASE_MILK_INFO )
}
