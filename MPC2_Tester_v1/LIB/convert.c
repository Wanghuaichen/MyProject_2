/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*  MODULE Convert                                              */
/*                                                              */
/****************************************************************/


#include "../lib/typedef.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/eehc11.h"
#include "../lib/time.h"
#include "../lib/rti_int.h"
#include "../lib/end_test.h"
#include "../usx/fake_usx.h"
#include "../comm/flowctrl.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../appl/conduct.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/temp.h"
#include "../appl/tandem.h"
#include "../appl/pulsation.h"
#include "../appl/takeoff.h"
#include "../key/key.h"
#include "../key/keyint.h"
#include "convert.h"

#pragma codeseg(EEPAGE)

unsigned char  DispTable[11];
unsigned char  iic_disp[11];
unsigned short DispFlash;
unsigned short disp_point;
unsigned char  DispLeds;
unsigned char  DispBlinkLeds;
unsigned char  DispBusy;

//extern unsigned short suErrorCount;
//extern unsigned char  suMsgType;

unsigned short timeon, timeoff;

// =============================================================================
unsigned char CheckLedAttentions( void )
{
//  NETIO_MAIL    *MailPtr;
//  CowDiseaseRef *CowDiseaseRefPtr;
//  unsigned char  nr_of_items;
  unsigned char  idx = GetDisplaySide();

  if ( KeyMode == KM_MILKING ){

    // melk-separatie attenties (D36)
    if ((p_MlkRef[idx] != NULL) && (LONG_COW_GEN_NUMBER ?
                    ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps :
                    ((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )){
      if ( (LONG_COW_GEN_NUMBER ?
             (((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost &&  (((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps == 1)):
             (((   MlkRef **)p_MlkRef)[idx]->CowGenColost &&  (((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps == 1)))){
        // colost (biest), led laten knipperen
        DispLeds      &= ~IIC_MILK_SEP_ATTENTION;       // bit uit anders knipperd led niet
        DispBlinkLeds |=  IIC_MILK_SEP_ATTENTION;       // laat led knipperen
      } else {
        DispLeds |= IIC_MILK_SEP_ATTENTION;             // led continue aan
      }
    } else {
      if ( (p_MlkRef[idx] != NULL) && (LONG_COW_GEN_NUMBER ?
                   ((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost :
                   ((   MlkRef **)p_MlkRef)[idx]->CowGenColost )){
        // colost (biest), led laten knipperen
        DispLeds      &= ~IIC_MILK_SEP_ATTENTION;       // bit uit anders knipperd led niet
        DispBlinkLeds |=  IIC_MILK_SEP_ATTENTION;       // laat led knipperen
      } else {
        // geen koe-data, of geen attentie, zet melk-led uit
        DispLeds      &= ~IIC_MILK_SEP_ATTENTION;
        DispBlinkLeds &= ~IIC_MILK_SEP_ATTENTION;
      }
    }

    // conductivity / temperature en ziekte code attenties (D35)
/*
    if ( p_CowDiseaseMail != NULL ){
      MailPtr = (NETIO_MAIL *)p_CowDiseaseMail;
      nr_of_items = ((MailPtr->message_length-(LONG_COW_GEN_NUMBER ?4:2))/sizeof((((CowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
    }
*/
    // check previous data
    if ( idx == GetDisplaySide() &&
        ((p_CowConRef[idx] != NULL &&
                 (LONG_COW_GEN_NUMBER ?
                 ((ExtCowConRef **)p_CowConRef)[idx]->CowConDataAttentions :
                 ((   CowConRef **)p_CowConRef)[idx]->CowConDataAttentions) &&
          MeasState[idx] == MEAS_IDLE ) ||
          CowTempAttention )){
      DispLeds      &= ~IIC_CONDUCT_ATTENTION;       // bit uit anders knipperd led niet
      DispBlinkLeds |=  IIC_CONDUCT_ATTENTION;       // laat led knipperen
    } else {
      // geen koe-data, of geen attentie, zet conductivity-led uit
      DispLeds      &= ~IIC_CONDUCT_ATTENTION;
      DispBlinkLeds &= ~IIC_CONDUCT_ATTENTION;
    }

    // bij actuele attentie led continue aan, 'previous' wordt overruled
    if ( idx == GetDisplaySide() && ConDataAttentions & 0x0f ){
      // generate attention, led continue aan
      DispBlinkLeds &= ~IIC_CONDUCT_ATTENTION;
      DispLeds      |=  IIC_CONDUCT_ATTENTION;
    }

    // kalender attenties (D34)
    if ( p_MlkRef[idx] != 0 ){
      if ( (LONG_COW_GEN_NUMBER ?
          (((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatAttent   ||
           ((ExtMlkRef **)p_MlkRef)[idx]->CowGenInsemAttent  ||
           ((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent ||
           ((ExtMlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent) :
          (((   MlkRef **)p_MlkRef)[idx]->CowGenHeatAttent   ||
           ((   MlkRef **)p_MlkRef)[idx]->CowGenInsemAttent  ||
           ((   MlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent ||
           ((   MlkRef **)p_MlkRef)[idx]->CowGenIncalfAttent)) ){
        DispLeds      &= ~IIC_CALENDAR_ATTENTION;       // bit uit anders knipperd led niet
        DispBlinkLeds |=  IIC_CALENDAR_ATTENTION;       // laat led knipperen
      } else {
        DispLeds      &= ~IIC_CALENDAR_ATTENTION;
        DispBlinkLeds &= ~IIC_CALENDAR_ATTENTION;
      }
    } else {
      DispLeds      &= ~IIC_CALENDAR_ATTENTION;
      DispBlinkLeds &= ~IIC_CALENDAR_ATTENTION;
    }

    // activiteits attentie (D33)
    if ((p_CowActRef[idx] != NULL  &&
          (LONG_COW_GEN_NUMBER ?
          ((ExtCowActRef **)p_CowActRef)[idx]->CowActAttention :
          ((   CowActRef **)p_CowActRef)[idx]->CowActAttention )) ||
        (p_CowActMemAttRef[idx] != NULL  &&
          (LONG_COW_GEN_NUMBER ?
          ((ExtCowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt :
          ((   CowActMemAttRef **)p_CowActMemAttRef)[idx]->CowActMemPerMaxAtt))){
      DispLeds |= IIC_ACTIVITY_ATTENTION;
    } else {
      DispLeds &= ~IIC_ACTIVITY_ATTENTION;
    }

    // melk attenties (D28)
    if ( (MilkMeterType < MMT_NO_MILK_METER )                        &&
        ((MlkState == MLK_SEND_WAIT && MeasState[idx] == MEAS_MEAS)  ||
         (MlkState == MLK_DONE      && MeasState[idx] == MEAS_DONE)) ){
      if ( (p_MlkRef[idx] != NULL) &&
          ((LONG_COW_GEN_NUMBER ?
           (((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10) :
           (((   MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10)) > ((MlkAmount+50) / 100)) ){
          // hoeveelheid melk onder de verwachte hoeveelheid
        DispLeds      &= ~IIC_MILK_ATTENTION ;       // bit uit anders knipperd led niet
        DispBlinkLeds |=  IIC_MILK_ATTENTION;        // laat led knipperen
      } else {
        // geen koe-data, of geen attentie, zet melk-led uit
        DispLeds      &= ~IIC_MILK_ATTENTION;
        DispBlinkLeds &= ~IIC_MILK_ATTENTION;
      }
    } else {
      if ( (EEPulsation & 0x02) && PLSCntrl.ManualPulseToggle){
        // handmatige pulsatie
        DispBlinkLeds &= ~IIC_MILK_ATTENTION;

        switch( PlsState ){

          case PLS_START_PULSATION:
          case PLS_STANDARD_PULSATION:
               if ( DispLeds & IIC_MILK_ATTENTION ){
                 if ( TimeControl( Cnt10msec, timeon + 100 )){
                   DispLeds &= ~IIC_MILK_ATTENTION;
                   timeoff = Cnt10msec;
                 }
               } else {
                 if ( TimeControl( Cnt10msec, timeoff + 100 )){
                   DispLeds |= IIC_MILK_ATTENTION;
                   timeon = Cnt10msec;
                 }
               }
            break;

          case PLS_START_STIM_PULSATION:
          case PLS_STIMULATION_PULSATION:
               if ( DispLeds & IIC_MILK_ATTENTION ){
                 if ( TimeControl( Cnt10msec, timeon + 50 )){
                   DispLeds &= ~IIC_MILK_ATTENTION;
                   timeoff = Cnt10msec;
                 }
               } else {
                 if ( TimeControl( Cnt10msec, timeoff + 50 )){
                   DispLeds |= IIC_MILK_ATTENTION;
                   timeon = Cnt10msec;
                 }
               }
            break;
          default:
               // geen koe-data, of geen attentie, zet melk-led uit
               DispLeds      &= ~IIC_MILK_ATTENTION;
               DispBlinkLeds &= ~IIC_MILK_ATTENTION;
               timeoff = Cnt10msec;
            break;
        } // end switch(PlsState)
      } else {
        // geen koe-data, of geen attentie, zet melk-led uit
        DispLeds      &= ~IIC_MILK_ATTENTION;
        DispBlinkLeds &= ~IIC_MILK_ATTENTION;
      }
    }

    if ( DispLeds != 0 || DispBlinkLeds != 0 ) return 1;
    else                                       return 0;
  } else {
    if ( EndTest == FALSE){
      if ( DispLeds      != 0 )DispLeds = 0;
      if ( DispBlinkLeds != 0 )DispBlinkLeds = 0;
    }
    return 1;
  }
}

// =============================================================================
// Display
// =============================================================================
void InitDisp(void)
{
  memory_set( DispTable, ' ', 10 );
  memory_clear(iic_disp, 10) ;

  DisplayOtherSide = 0;
  DispBlinkLeds = DispLeds = 0;
  DispFlash = 0;
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_INFO, 1, (unsigned char *)&DispLeds );
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_BLINK_INFO, 1, (unsigned char *)&DispBlinkLeds );
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_FLASH_INFO, 2, (unsigned char *)&DispFlash );
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&DispLeds );
  HandleWriteIic(KB_ADDRESS, IIC_DISPLAY_TEXT, 10, DispTable);

//  for(;;)ReadKey();
  DispBusy  = 0;
}


// =============================================================================
// Display
// =============================================================================
void Display(void)
{
  static unsigned short dc = 0xffff;
  static unsigned char  dbl = 0x00;
  static unsigned char  dl  = 0x00;
  static unsigned short next_disp_time = 0;
  static unsigned short point_data = 0;
//  unsigned short x,i;

  if (DispBusy == 0) {
    if ((DispFlash != dc) || (Cnt1sec > next_disp_time )) {
      if(!HandleWriteIic(KB_ADDRESS, IIC_WRITE_FLASH_INFO, 2, (unsigned char *)&DispFlash) ) dc = DispFlash;
    }

    // kijk welke leds aan of uit moeten, en of ze eventueel moeten knipperen
    CheckLedAttentions();
    if ( (DispBlinkLeds != dbl) || (Cnt1sec > next_disp_time )){
      if ( !HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_BLINK_INFO, 1, (unsigned char *)&DispBlinkLeds ) ) dbl = DispBlinkLeds;
    }
    if ( (DispLeds != dl) || (Cnt1sec > next_disp_time ) ){
      if( !HandleWriteIic(KB_ADDRESS, IIC_WRITE_LED_INFO, 1, (unsigned char *)&DispLeds ) ) dl = DispLeds;  // 1
    }

    // indien 'shift' key wordt ingedrukt display allemaal puntjes op display!!
    if ( KeyMode == KM_MILKING && (KeyState == KS_CONTINUE || KeyState == KS_CONTINUE_2)) disp_point |= 0x03fff;
    else if ( (disp_point & 0x3fff) == 0x3fff ) { disp_point = 0;  next_disp_time -= 5; }
/*
    if ( (KeyMode == KM_MILKING) && (MilkMeterType == MMT_VFLOW) && (suErrorCount)){
      if ( suMsgType == 'D' ){
        i =((suErrorCount-1)%3);
        x = (0x0008 >> i);
        x |= 0x0010;
      } else {
        i =((suErrorCount-1)%3);
        x = (0x0002 << i);
        x |= 0x0001;
      }
      disp_point |= x;
    }
*/
    if ( (memory_cmp( DispTable, iic_disp, 10 )) || (Cnt1sec > next_disp_time )){

      // bepaal waar de punt terecht moet komen
      // 10 digits, start van af rechter digit (waarde is gespiegeld)
//      if ( disp_point != 0x3fff ){
//        if (Item.NrDec == 0) disp_point = 0;
//        else                 disp_point = 0x0200 >> Item.NrDec;
//      }

      if (!HandleWriteIic(KB_ADDRESS, IIC_DISPLAY_TEXT, 10, DispTable) ){
        // copieer van DispTable( source) naar cc (destination)
        memory_cpy( iic_disp, DispTable, 10 );

        if (!HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&disp_point ))point_data = disp_point;
        next_disp_time = Cnt1sec+5;
      }
    }

    // 'exit-gate' locked wordt aangegeven door aansturen van de punten
    // op de eerste vijf plekken (alleen open-tandem)
    if ( ParlourTandemTypeAndMilkingMode() && (disp_point != 0x3fff)) {
      if ( OTCowInBox() ) disp_point |=  0x0010;          // zet punt achter resp-nr
      else                disp_point &= ~0x0010;

      if ( OTCowInSystem() ) disp_point |=  0x0020;      // zet punt op digit 6
      else                   disp_point &= ~0x0020;
    }


    if (disp_point != point_data ) {
      if (!HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&disp_point ))point_data = disp_point;
    }
  }
}


// =============================================================================
// Display Message
// =============================================================================
void DisplayMessage(unsigned char msg_nr)
{
  if ( KeyMode != KM_DISP_MSG ){
    MsgDispTime     = Cnt10msec;
    KeyModeOld      = KeyMode;
    ScreenModeOld   = ScreenMode;
    ScreenNumberOld = ScreenNumber;
    ScreenNumber    = msg_nr;
    KeyMode         = KM_DISP_MSG;
  }
}

// =============================================================================
// Display Item
// =============================================================================
void ItemToDisp(ITEM_INFO *p, unsigned char Pos)
{
  signed short Dummy;
  DispBusy = 1;

  if (Pos == 0) {
    // komt overeen met POS_RIGHT ----------------------------------------------
    Pos = 11 - p->Size;
    AsciiToDisp(' ', 1, 10);
  }
  switch (p->Type) {
    case _BL :
         BoolToDisp(*(p->PEEAddress), Pos);
      break;
    case _U1 :
         CharToDisp(*(p->PEEAddress), Pos, p->Size);
      break;
    case _S1 :
         Dummy = *(signed char *)p->PEEAddress;
         if (Dummy < 0) {
           Dummy ^= 0xff;
           Dummy++;
           AsciiToDisp('-', Pos-1, 1);
         }
         CharToDisp(Dummy, Pos, p->Size);
      break;
    case _U2 :
         ShortToDisp(*(unsigned short *)(p->PEEAddress), Pos, p->Size);
      break;
    case _S2 :
         Dummy = *(signed short *)p->PEEAddress;
         if (Dummy < 0) {
           Dummy ^= 0xffff;
           Dummy++;
           AsciiToDisp('-', Pos-1, 1);
         }
         ShortToDisp(Dummy, Pos, p->Size);
      break;
    case _U4 :
         LongToDisp(*(unsigned long *)(p->PEEAddress), Pos, p->Size);
      break;
    case _TM:
         Dummy  = (*(unsigned short *)p->PEEAddress/60)*100;
         Dummy += (*(unsigned short *)p->PEEAddress%60);
         ShortToDisp(Dummy, Pos, p->Size);
      break;
    default :
      break;
  }
  DispBusy = 0;
}


// =============================================================================
// Display Ascii
// =============================================================================
void AsciiToDisp(unsigned char n, unsigned char start, unsigned char nmbr)
{
  unsigned short i = 0;

  start--;
  switch (nmbr) {
    case 10 :
      DispTable[start+i] = n;
      i++;
    case 9 :
      DispTable[start+i] = n;
      i++;
    case 8 :
      DispTable[start+i] = n;
      i++;
    case 7 :
      DispTable[start+i] = n;
      i++;
    case 6 :
      DispTable[start+i] = n;
      i++;
    case 5 :
      DispTable[start+i] = n;
      i++;
    case 4 :
      DispTable[start+i] = n;
      i++;
    case 3 :
      DispTable[start+i] = n;
      i++;
    case 2 :
      DispTable[start+i] = n;
      i++;
    case 1 :
      DispTable[start+i] = n;
      i++;
      break;
    default :
      break;
  }
}

// =============================================================================
// Display unsigned char
// =============================================================================
void BoolToDisp(BOOLEAN n, unsigned char start)
{
  if ( n ){
    memory_cpy((unsigned char *)&(DispTable[start-1]), (unsigned char *)&(TextString[EELanguage][TXT_YES]), 1 );
  }
  else{
    memory_cpy((unsigned char *)&(DispTable[start-1]), (unsigned char *)&(TextString[EELanguage][TXT_NO]), 1 );
  }
}


// =============================================================================
// Display unsigned char
// =============================================================================
void CharToDisp(unsigned char n, unsigned char start, unsigned char nmbr)
{
  unsigned short i = 0;

  start--;
  switch (nmbr) {
    case 3 :
      DispTable[start+i] = (n / 100) + 0x30;
      i++;
    case 2 :
      n %= 100;
      DispTable[start+i] = (n / 10) + 0x30;
      i++;
    case 1 :
      n %= 10;
      DispTable[start+i] = (n / 1) + 0x30;
      i++;
      break;
    default :
      break;
  }
}


// =============================================================================
// Display unsigned short
// =============================================================================
void ShortToDisp(unsigned short n, unsigned char start, unsigned char nmbr)
{
  unsigned short i = 0;

  start--;
  switch (nmbr) {
    case 5 :
      DispTable[start+i] = (n / 10000) + 0x30;
      i++;
    case 4 :
      n %= 10000;
      DispTable[start+i] = (n / 1000) + 0x30;
      i++;
    case 3 :
      n %= 1000;
      DispTable[start+i] = (n / 100) + 0x30;
      i++;
    case 2 :
      n %= 100;
      DispTable[start+i] = (n / 10) + 0x30;
      i++;
    case 1 :
      n %= 10;
      DispTable[start+i] = (n / 1) + 0x30;
      i++;
      break;
    default :
      break;
  }
}


// =============================================================================
// Display unsigned long
// =============================================================================
void LongToDisp(unsigned long n, unsigned char start, unsigned char nmbr)
{
  unsigned short i = 0;

  start--;
  switch (nmbr) {
    case 6 :
      DispTable[start+i] = (n / 100000) + 0x30;
      i++;
    case 5 :
      n %= 100000;
      DispTable[start+i] = (n / 10000) + 0x30;
      i++;
    case 4 :
      n %= 10000;
      DispTable[start+i] = (n / 1000) + 0x30;
      i++;
    case 3 :
      n %= 1000;
      DispTable[start+i] = (n / 100) + 0x30;
      i++;
    case 2 :
      n %= 100;
      DispTable[start+i] = (n / 10) + 0x30;
      i++;
    case 1 :
      n %= 10;
      DispTable[start+i] = (n / 1) + 0x30;
      i++;
      break;
    default :
      break;
  }
}


// =============================================================================
// Convert integer to string
// =============================================================================
void IntToStr(unsigned long n, char *s, unsigned char start, unsigned char nmbr)
{
  unsigned short i = 0;

  start--;
  s[6] = '\0';

  switch (nmbr) {
    case 6 :
      s[start+i] = (n / 100000) + 0x30;
      i++;
    case 5 :
      n %= 100000;
      s[start+i] = (n / 10000) + 0x30;
      i++;
    case 4 :
      n %= 10000;
      s[start+i] = (n / 1000) + 0x30;
      i++;
    case 3 :
      n %= 1000;
      s[start+i] = (n / 100) + 0x30;
      i++;
    case 2 :
      n %= 100;
      s[start+i] = (n / 10) + 0x30;
      i++;
    case 1 :
      n %= 10;
      s[start+i] = (n / 1) + 0x30;
      i++;
      break;
    default :
      break;
  }
}


