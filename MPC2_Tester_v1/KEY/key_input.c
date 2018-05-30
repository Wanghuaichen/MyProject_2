#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/input.h"
#include "../lib/eehc11.h"
#include "../lib/rti_int.h"
#include "../lib/intr6811.h"
#include "../lib/mem.h"
#include "../lib/convert.h"
#include "../lib/typedef.h"
#include "../lib/text.h"
#include "../lib/eeprom.h"
#include "../lib/extport.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../lib/stdlib.h"
#include "../include/romeehc1.h"
#include "../include/romver.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/temp.h"
#include "../motor/motor.h"
#include "../comm/send_msg.h"
#include "../comm/flowctrl.h"

// =============================================================================
// Controleer ingevoerde address
// =============================================================================
void CheckAddress(unsigned long Value)
{
  //Main address ?
  if (Item.PEEAddress == &EEAddress ) {
    if ((unsigned char)Value != EEAddress) {
      if ((unsigned char)Value <= Item.MaxVal && (unsigned char)Value >= Item.MinVal) {
        // indien het address dezelfde is als address2 dan maak address2 nul!
        if ((unsigned char)Value == EEAddress2) {
          WriteEEpromByte(0, &EEAddress2);
        }
        WriteEEpromByte((unsigned char)Value, &EEAddress);

        // schakel relays af, anders wordt motor-uitgang aangestuurd tijdens checksum test
        PowerRelayOff();

        //Reset op waakhond, maar lees eerst key-buffer leeg
        while ( ReadKey() == KEY_ENTER );
        disable_interrupt();
        while(1);
      }
    }
  } else {
    //Address 2
    if ((unsigned char)Value != EEAddress2) {
      if ((unsigned char)Value <= Item.MaxVal && (unsigned char)Value >= Item.MinVal) {
        if ((unsigned char)Value != EEAddress || Value == 0) {
          WriteEEpromByte((unsigned char)Value, &EEAddress2);

          // schakel relays af, anders wordt motor-uitgang aangestuurd tijdens checksum test
          PowerRelayOff();

          // lees key-buffer leeg
          while ( ReadKey() == KEY_ENTER );
          disable_interrupt();
          while(1);
        }
      }
    }
  }
}


// =============================================================================
// Handle KeySave
// =============================================================================
void HandleKeySave(long Value, unsigned char SMode, unsigned char SNumber,
                   unsigned char EItem, unsigned char idx, unsigned char val2 )
{
  NETIO_MAIL     *MailPtr;
  void           *CowSepInfoResMailPtr;       // Result is een kopie van de Ref, andere structure
  void           *CowDiseaseResPtr;
  unsigned char  *Ptr, check_send = 0;
  unsigned char  i, nr_of_res_items;
  unsigned short amount;

  Ptr = Item.PEEAddress;
  OldDispValue = -1;

  // Address gewijzigd ?
  if (Ptr == &EEAddress || Ptr == &EEAddress2) {
    CheckAddress(Value);
    return;
  }

  // Default invullen  ?
  if (Ptr == &EEDefault) {
    switch( Value ){
      case  1: SetFactorySettings();
               RomReset();
        break;
      case  2: if ( EEServiceStands ){
                 SendDefSet();
                 RomReset();
               }
        break;
      default: return;
    }
    return;
  }

  // controleer of het een EEprom adres is
  if ((Ptr >= (unsigned char *)0xb602)  && (Ptr < (unsigned char *)0xb800)) {
    switch (Item.Type ) {
      case _U1:
      case _BL:
           if ((unsigned char)Value >= Item.MinVal && (unsigned char)Value <= Item.MaxVal ) {
             if ( WriteEEpromByte((unsigned char) Value, Ptr) ){
               if ( Ptr == &EEFeeding) {
                 if ( EEFeeding && ( SoftKey & SK_FEEDING_ENABLE) ){
                   TaskStart(FEED_A_CONTROL_TASK);
                   if ( EEAddress2 ) TaskStart(FEED_B_CONTROL_TASK);  // swing-over
                 } else {
                   TaskStop(FEED_A_CONTROL_TASK);
                   if ( EEAddress2 )TaskStop(FEED_B_CONTROL_TASK);    // swing-over
                 }
               } else {
                 if ((Ptr == &EEParlourType) && (SoftKey & SK_TANDEM_ENABLE )){
                   if ((EEParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) && (EEParlourType >= PT_SEMI_TANDEM)) TaskStart(TANDEM_CONTROL_TASK);
                   else                                                                                     TaskStop(TANDEM_CONTROL_TASK);
                 } else {
                   if ((Ptr == &EEConMeasuring) && ((EEMilkMeterType == MMT_VFLOW) || (EEMilkMeterType == MMT_VP4001))) {
                      // ExtConMeasuring not visible when VFLOW
                     if ( Value != 0) WriteEEpromByte((unsigned char) 4, &EEExtConMeasuring);
                     else             WriteEEpromByte((unsigned char) 0, &EEExtConMeasuring);
                   }
                 }
               }
             }
           }
        break;
      case _S1:
           if ((signed char)Value >= (signed char)Item.MinVal && (signed char)Value <= (signed char)Item.MaxVal ) {
             WriteEEpromByte((signed char) Value, Ptr);
           }
        break;
      case _S2:
           if ((signed short)Value >= (signed short)Item.MinVal && (signed short)Value <= (signed short)Item.MaxVal ) {
             WriteEEpromShort((unsigned short) Value, (unsigned short *)Ptr);
           }
        break;
      case _U2:
           if ((unsigned short)Value >= Item.MinVal && (unsigned short)Value <= Item.MaxVal ) {
             WriteEEpromShort((unsigned short) Value, (unsigned short *)Ptr);
           }
        break;
    }
  } else {
    if  ( Value < 0 ) return;

    switch( SMode ){
      case SM_MILKING:
      case SM_ROBOT:
           if ( SNumber == SCREEN_MILK_0 ){ // COW_NUMBER
             if ( EItem == 1){
               if ( StandAloneFeeding() ){
                 // actuele voer info
                 if ( (unsigned short)Ptr == (unsigned short )(&ExtraFeedDpl) ){
                   // Extra voertegoed
                   if ( Value <= 100 ){
                   	 // check if extra feed must be fed by MPC or external feeder
                     if ( p_CowFedParlRef[idx] != NULL ){
                       amount = (signed short)Value*100 -
                              (LONG_COW_GEN_NUMBER ?
                              (((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance :
                              (((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance );
                     } else {
                       amount = (signed short)Value*100;
                     }
                   } else {
                     amount = 0;
                   }

                   // in geval van stand-alone maak de gevoerde hoeveelheid weer nul
                   if ((EEAddressCopy == 0) && Value &&  (!EEFeedBalanceDef[idx]) ){
                     if ( p_CowFedParlRes[idx] != NULL ){
                       if (LONG_COW_GEN_NUMBER ){
                         (((ExtCowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed = 0;
                       } else {
                     	   (((   CowFedParlRes **)p_CowFedParlRes)[idx]->Array)[0].CowFedParlAmountFed = 0;
                       }
                     }
                   }

                   ExtraFeed[idx] = amount;
                 }
               } else {
                 // CowNumber modified
                 while(HndlMlkRefReq((unsigned short) Value, MANUAL_REF, idx) == 0)TaskExit();
               }
             } else {                            // MILK YIELD
               if ( EItem == 2){
                 // Milkyield manual input
                 if ( Value == 0 ) {
                   MilkManualInput = FALSE;
                 } else {
                   MilkManualInput = TRUE;
                   MlkAmount   = Value * 100;
                   MlkAmountKg = Value;

                   // stuur melkgift op, indien MlkState == MLK_SEND_WAIT gebeurt dit vanzelf
                   if (MlkState != MLK_SEND_WAIT ){
                     HndlMlkRes(NORMAL_RES_NO_CLEAR, SEND_UPDATE_ONLY, TRUE );
                   }
                 }
               }
             }
           }
        break;
      case SM_COW_INFO:
           switch ( SNumber ){
             case SCREEN_COW_INFO_1:             // SEPARATION INFO
                  if (p_CowGenRef[idx] != NULL) {
                    if (p_CowSepInfoResMail[idx] == NULL) {
                      // reserveer ruimte voor resultaat
                      while (p_CowSepInfoResMail[idx] == NULL) {
                          p_CowSepInfoResMail[idx] = calloc(1, (sizeof(ExtCowSepInfoRef)+
                                        ((MAX_NR_OF_SEP_RES-1)*sizeof(((CowSepInfoRes *)CowSepInfoResMailPtr)->Array[0])) + sizeof(NETIO_MAIL)));
                      }
                    }
                    MailPtr = (NETIO_MAIL *)p_CowSepInfoResMail[idx];
                    nr_of_res_items = ((MailPtr->message_length-(LONG_COW_GEN_NUMBER ? 4:2))/sizeof((((CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[0]));
                    CowSepInfoResMailPtr = (void *)(&p_CowSepInfoResMail[idx]->message_start);

                    if ( EItem == 1 ){
                      // separation function, kijk of die in de lijst staat zoniet plaats hem
                      for( i = 0; i < nr_of_res_items; i++ ){
                        if ( (LONG_COW_GEN_NUMBER ?
                        	    ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepFunction :
                        	    ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepFunction ) == (unsigned char)Value ) {
                          // functie bestaat al, zet aantal separaties op het display
                          sub_screen_pntr = i;
                          if ( val2 != 0xff ){
                            // extra variabele meegestuurd welke het aantal separaties weergeeft
                            if (LONG_COW_GEN_NUMBER ){
                              ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations = val2;
                            } else {
                              ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations = val2;
                            }
                            check_send = 1;
                          }
                          break;
                        }
                      }
                      if ( i == nr_of_res_items ){
                        // functie niet aanwezig, plaats hem achter aan
                        if ( i < MAX_NR_OF_SEP_RES ){
                          if (LONG_COW_GEN_NUMBER ){
                            ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepFunction = (unsigned char)Value;
                            ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepNrOfSeparations = 1;
                          } else {
                            ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepFunction = (unsigned char)Value;
                            ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[i].CowSepNrOfSeparations = 1;
                          }
                          MailPtr->message_length += 2;
                          // verhoog het aantal items, en zet de pointer op het toegevoegde item
                          CowSepNrOfItems++;
                          sub_screen_pntr = CowSepNrOfItems-1;
                          check_send = 1;
                        } else {
                          // buffer te klein, eerste overschrijven
                          if (LONG_COW_GEN_NUMBER ){
                            ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[0].CowSepFunction = (unsigned char)Value;
                            ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[0].CowSepNrOfSeparations = 1;
                          } else {
                            ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[0].CowSepFunction = (unsigned char)Value;
                            ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[0].CowSepNrOfSeparations = 1;
                          }
                          check_send = 1;
                        }
                      }
                    } else {
                      if ( EItem == 2 ){
                        // aantal separaties
                        if ( (LONG_COW_GEN_NUMBER ?
                        	 ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations :
                        	 ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations ) != (unsigned char)Value ) {
                          if (LONG_COW_GEN_NUMBER ){
                            ((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations = (unsigned char)Value;
                          } else {
                            ((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array[sub_screen_pntr].CowSepNrOfSeparations = (unsigned char)Value;
                          }
                          check_send = 1;
                        }
                      }
                    }

                    if ( check_send == 1 ){
                      // stuur de data opnieuw zolang er nog geen nieuwe koe herkend is
                      while(HndlCowSepInfoRes(idx) == 0)TaskExit();
                    }
                  }
               break;
             case SCREEN_COW_INFO_2:             // GROUP NUMBER
                  if ( Value == 0 ){
                    // restore old value
                    if ( *(unsigned short *)Ptr !=
                    	 (LONG_COW_GEN_NUMBER ?
                    	 ((ExtCowGenRef **)p_CowGenRef)[idx]->CowGenGrpNumber :
                    	 ((   CowGenRef **)p_CowGenRef)[idx]->CowGenGrpNumber )){
                      *(unsigned short *)Ptr =
                                  	 (LONG_COW_GEN_NUMBER ?
                                      ((ExtCowGenRef **)p_CowGenRef)[idx]->CowGenGrpNumber :
                                      ((   CowGenRef **)p_CowGenRef)[idx]->CowGenGrpNumber);
                      check_send = 1;
                    }
                  } else {
                    if ( *(unsigned short *)Ptr != (unsigned short) Value ){
                      *(unsigned short *)Ptr = (unsigned short) Value;
                      check_send = 1;
                    }
                  }
                  if ( check_send ){
                    // stuur de data opnieuw zolang er nog geen nieuwe koe herkend is
                    HndlCowGenRes(idx);
                  }
               break;
             case SCREEN_COW_INFO_4:
                  // swing over, koe ander zijde
                  idx = GetOtherSideIndex(SideState);
                  while (HndlMlkRefReq((unsigned short) Value, MANUAL_REF, idx)== 0)TaskExit();
               break;
           } // end switch ( SNumber )
        break;
      case SM_DISEASE_MILK_INFO:
           // Ziekte codes/ behandeling
           if ( *(unsigned char *)Ptr != (unsigned char) Value ){
             *(unsigned char *)Ptr = (unsigned char) Value;

             // pas opsturen als beide items zijn ingevuld; fill treatment with same value
             CowDiseaseResPtr = (void *)(&(CowDisease[idx]));
             (( LONG_COW_GEN_NUMBER ?
                 (((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseTreatment = Value ) :
                 (((   CowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseTreatment = Value)));

             if (( LONG_COW_GEN_NUMBER ?
                 (((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode ) :
                 (((   CowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode ))){
               // stuur de data opnieuw zolang er nog geen nieuwe koe herkend is
               while (HndlCowDiseaseRes(idx) == 0)TaskExit();
             }
           }

        break;
      case SM_FEED_INFO:                         // EXTRA FEED
           // Do not save in case of No Cow
           if ( (unsigned short)Ptr == (unsigned short )(&ExtraFeedDpl) ){
             // Extra voertegoed
             if ( Value <= 100 ){
               if ( (p_CowFedParlRef[idx] != NULL) && (EEDummy1 == 0)){
                 ExtraFeed[idx] = (signed short)Value*100 -
                      (LONG_COW_GEN_NUMBER ?
                      ( ((ExtCowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance :
                      ( ((   CowFedParlRef **)p_CowFedParlRef)[idx]->Array)[0].CowFedParlBalance);
               } else {
                 ExtraFeed[idx] = (signed short)Value*100;
               }
             } else {
               ExtraFeed[idx] = 0;
             }
             if ( EEDummy1 == 1 ){
               // external feeder; send REF
               while (HndlCowExtFedParlRef(idx) == 0)TaskExit();
             }
           }
        break;
      case SM_MILK_INFO:                         // MELK GIFT
           switch ( SNumber ){
             case SCREEN_MILK_INFO_0:
                  // Do not save in case of No Cow
                  // separatie melk moet in stand-alone kunnen worden ingegeven
                  if (p_MlkRef[idx] != NULL || Ptr == &MlkSepStandAlone) {
                    if ( *(unsigned char *)Ptr != (unsigned char) Value ){
                      *(unsigned char *)Ptr = (unsigned char) Value;
                      check_send = 1;
                    }

                    if (( p_MlkRef[idx] != NULL) && ( check_send == 1 ) ){
                      // stuur de data opnieuw zolang er nog geen nieuwe koe herkend is
                      // milk-amount 0 wordt overgestuurd, zonder deze aan te passen
                      // komt hier niet zonder MlkRef!!
                      HndlMlkRes(NORMAL_RES_NO_CLEAR, SEND_UPDATE_CLR_MLK_AMOUNT, FALSE);
                    }
                  }
               break;
             case SCREEN_MILK_INFO_7:   // MANUAL RELEASE
                  if ((p_MlkRef[idx] != NULL) && (Ptr == (LONG_COW_GEN_NUMBER ?
                              ((unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowGenManualRelease) :
           		                ((unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowGenManualRelease))) ){
           		      *(unsigned char *)Ptr = Value;
           		    }
               break;
           }
        break;
      case SM_CALENDAR_INFO:
           switch ( SNumber ){
             case SCREEN_CALENDAR_INFO_1:        // HEAT
                  if ( Value == 1 ){
                    // schrijf huidige datum naar result
                    if ( CowGenHeatDate[idx] != AbsDate ){
                      CowGenHeatDate[idx] = AbsDate;
                      check_send =1;
                    }
                  } else {
                    if ( Value == 0 ){
                      // haal oude waarde weer terug
                      if ( CowGenHeatDate[idx] !=
                           (LONG_COW_GEN_NUMBER ?
                           ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate)) :
                         	 ( ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate)) )){
                        CowGenHeatDate[idx]     =
                               (LONG_COW_GEN_NUMBER ?
                                ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatDate):
                                ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenHeatDate));
                        check_send =1;
                      }
                    }
                  }
               break;
             case SCREEN_CALENDAR_INFO_2:          // INSEMINATION
                  if ( Value == 1 ){
                    // schrijf huidige datum naar result
                    if ( CowGenInsemDate[idx] != AbsDate ){
                      CowGenInsemDate[idx] = AbsDate;
                      check_send =1;
                    }
                  } else {
                    if ( Value == 0 ){
                      // haal oude waarde weer terug
                      if ( CowGenInsemDate[idx] !=
                           (LONG_COW_GEN_NUMBER ?
                           ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate)) :
                         	 ( ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate)) )){
                        CowGenInsemDate[idx]     =
                               (LONG_COW_GEN_NUMBER ?
                                ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemDate):
                                ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemDate));

                        check_send =1;
                      }
                    }
                  }
               break;
             case SCREEN_CALENDAR_INFO_3:          // INCALF
                  if ( Value == 1 ){
                    // schrijf huidige datum naar result
                    if ( CowGenReproState[idx] != 3 ){
                      CowGenReproState[idx] = 3;
                      check_send = 1;
                    }
                  } else {
                    if ( Value == 0 ){
                      // haal oude waarde weer terug
                      if ( CowGenReproState[idx] !=
                           (LONG_COW_GEN_NUMBER ?
                           ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenReproState == 3 ? 1 : 0 ) :
                         	 ( ((   MlkRef **)p_MlkRef)[idx]->CowGenReproState == 3 ? 1 : 0 ))){
                        CowGenReproState[idx]     =
                               (LONG_COW_GEN_NUMBER ?
                                (((ExtMlkRef **)p_MlkRef)[idx]->CowGenReproState == 3 ? 1 : 0) :
                                (((   MlkRef **)p_MlkRef)[idx]->CowGenReproState == 3 ? 1 : 0));
                        check_send = 1;
                      }
                    }
                  }
               break;
             case SCREEN_CALENDAR_INFO_4:          // DRY
                  if ( Value == 1 ){
                    // schrijf huidige datum naar result
                    if ( CowGenDryOffDate[idx] != AbsDate ){
                      CowGenDryOffDate[idx] = AbsDate;
                      check_send = 1;
                    }
                  } else {
                    if ( Value == 0 ){
                      // haal oude waarde weer terug
                      if ( CowGenDryOffDate[idx] !=
                           (LONG_COW_GEN_NUMBER ?
                           ( ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate)) :
                         	 ( ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate)) )){
                        CowGenDryOffDate[idx]     =
                               (LONG_COW_GEN_NUMBER ?
                                ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate == 0 ? 0 :(AbsDate - ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate):
                                ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate == 0 ? 0 :(AbsDate - ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate));
                        check_send =1;
                      }
                    }
                  }
               break;
           } // end switch (SNumber )
           if ( check_send ){
             // stuur de data opnieuw zolang er nog geen nieuwe koe herkend is
             HndlMlkRes(NORMAL_RES_NO_CLEAR, SEND_UPDATE_CLR_MLK_AMOUNT, FALSE);
           }
        break;
      default:
        break;
    } // end switch(SMode)
  }
}

void HandleKeyInput( void )
{
  void           *CowDiseaseResPtr;
  void           *CowSepInfoResMailPtr;
           long   value;
  unsigned short  DummyValue = 0;
  unsigned char   idx;

  idx = GetDisplaySide();


  // in geval van 2 items op het display moet eerst de pointer naar
  // dat item gezet worden dmv de functie DisplayValue ......
  idx = GetDisplaySide();
  if ( ScreenMode == SM_MILKING && ScreenNumber == SCREEN_MILK_0 ){
    if ( EditItem != 2 ){
      if ( StandAloneFeeding() ){
        DispValue((unsigned char *)&ExtraFeedDpl, _S2, 3, 1, WRITE, POS_2);
      } else {
        if (p_MlkRef[idx] != NULL) {
          DispValue( LONG_COW_GEN_NUMBER ?
                 (unsigned char *)&((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber :
                 (unsigned char *)&((   MlkRef **)p_MlkRef)[idx]->CowGenNumber, _U2, 5, 0, WRITE, POS_LEFT);
        } else {
          DispValue((unsigned char *)&DummyValue, _U2, 5, 0, WRITE, POS_LEFT);
        }
      }
    } else {
      if ( MilkMeterType >= MMT_NO_MILK_METER ){
        Item.Attrib = READ_ONLY;
      } else {
        DispValue((unsigned char *)&MlkAmountKg, _U2, 3, 1, WRITE, POS_8);
      }
    }
  } else {
    if ( ScreenMode == SM_DISEASE_MILK_INFO && ScreenNumber == SCREEN_DISEASE_15 ){
      CowDiseaseResPtr = (void *)(&(CowDisease[idx]));
//      if ( EditItem != 2 ){
        DispValue(LONG_COW_GEN_NUMBER ?
              (unsigned char *)&(((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode) :
              (unsigned char *)&(((   CowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseCode), _U1, 2, 0, WRITE, POS_RIGHT);
//      } else {
//        DispValue(LONG_COW_GEN_NUMBER ?
//              (unsigned char *)&(((ExtCowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseTreatment) :
//              (unsigned char *)&(((   CowDiseaseRes *)CowDiseaseResPtr)->Array[disease_trt_number].CowDiseaseTreatment), _U1, 2, 0, WRITE, POS_RIGHT);
//      }
    } else {
      if ( ScreenMode == SM_COW_INFO && ScreenNumber == SCREEN_COW_INFO_1 ){
        CowSepInfoResMailPtr = (void *)(&p_CowSepInfoResMail[idx]->message_start);
        if ( EditItem != 2 ){
          DispValue(LONG_COW_GEN_NUMBER ?
                (unsigned char *)&(((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction :
                (unsigned char *)&(((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepFunction, _U1, 2, 0, WRITE, POS_7);
        } else {
          DispValue(LONG_COW_GEN_NUMBER ?
                (unsigned char *)&(((ExtCowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations :
                (unsigned char *)&(((   CowSepInfoRes *)CowSepInfoResMailPtr)->Array)[sub_screen_pntr].CowSepNrOfSeparations, _U1, 1, 0, WRITE, POS_RIGHT);
        }
      }
    }
  }

  if (   Item.Attrib == WRITE ){
    // negatieve items hebben eventueel een positie meer nodig voor het 'min-teken'
    if ( GetNValue((Item.Type == _S1 || Item.Type == _S2 ) ? (Item.Size+1) : Item.Size, Item.NrDec,
                   (Item.Type == _S1 || Item.Type == _S2 ) ? (Item.Pos-2): Item.Pos-1,  &value)){
      HandleKeySave( value, ScreenMode, ScreenNumber, EditItem, idx, 0xff );
      // maak ruimte voor een extra ziekte behandeling, max 5
      if ( ScreenMode == SM_DISEASE_MILK_INFO && ScreenNumber == SCREEN_DISEASE_15 && EditItem != 2 ){
        if ( nr_of_disease_trt_items[idx] < 5 ){
            nr_of_disease_trt_items[idx]++;
        }
      }
    }
  }

  // clear table zodat deze opniew wordt geschreven
  memory_clear( (unsigned char *)iic_disp, 10 );
  KeyMode     = KeyModeOld;
  KeyOld      = KEY_ENTER;        // anders wordt enter-key opnieuw behandeld
  KeyHoldTime = Cnt10msec;
  ScreenTime  = Cnt1sec;
}