
// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// =============================================================================

// include files  --------------------------------------------------------------

#include "../key/disp.h"
#include "../key/key.h"
#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/rti_int.h"
#include "../lib/typedef.h"
#include "../lib/applver.h"
#include "../lib/int.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../lib/text.h"
#include "../lib/burnin.h"
#include "../lib/item.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../key/keyint.h"
#include "../key/scr_milk.h"
#include "../key/scr_milk_info.h"
#include "../key/scr_feed_info.h"
#include "../key/scr_cow_info.h"
#include "../key/scr_disease_info.h"
#include "../key/scr_calendar_info.h"
#include "../key/scr_pulse_info.h"
#include "../key/scr_robot.h"
#include "../lib/extport.h"
#include "../include/romeehc1.h"
#include "../include/romstnet.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/cow_herd_vcp_flow.h"
#include "../flowlib/disease_weight_vcp_flow.h"
#include "../flowlib/separation_vcp_flow.h"
#include "../motor/motor.h"
#include "../comm/flowctrl.h"
#include "../net/local_net.h"
#include "../usx/fake_usx.h"
#include "../init/start.h"
#include "../init/main.h"
#include "../appl/niveau.h"
#include "../appl/memolac.h"
#include "../appl/milkmisc.h"
#include "../appl/milk.h"
#include "../appl/take_off_sensor.h"
#include "../appl/takeoff.h"
#include "../appl/conduct.h"
#include "../appl/tandem.h"
#include "../appl/temp.h"


#pragma codeseg(EEPAGE)

unsigned char  ScreenNumberOld;
unsigned char  ScreenNumber;
unsigned char  ScreenMode;
unsigned char  ScreenModeOld;
unsigned short ScreenTime;
unsigned long  OldDispValue;
unsigned short MsgDispTime;

unsigned short OtherDisplayTime;
BOOLEAN        DisplayOtherSide;

ITEM_INFO Item, *ScreenEEPtr;

unsigned char  disease_number;
unsigned char  nr_of_disease_items;
unsigned char  disease_trt_number;
unsigned char  nr_of_disease_trt_items[MAX_NODES];

unsigned char  sub_screen_pntr;
unsigned char  CowSepNrOfItems;


unsigned short CowFedTotal;
unsigned short CowFedBalance;
unsigned short CowFedParlTotalFed;

unsigned short ExtraFeedDpl;
  signed short ExtraFeed[MAX_NODES];

unsigned short CowGenHeatDate[MAX_NODES];
unsigned short CowGenHeatDateDpl;
unsigned short CowGenInsemDate[MAX_NODES];
unsigned short CowGenInsemDateDpl;
unsigned short CowGenDryOffDate[MAX_NODES];
unsigned short CowGenDryOffDateDpl;
unsigned char  CowGenReproState[MAX_NODES];

unsigned char  CommunicationError;

#if ( _EMC_TEST_ )
const unsigned char EmcString[7] = { "START "};
#endif

// =============================================================================
// Display Value
// =============================================================================
void DispValue(unsigned char *Ptr,   unsigned char Type,
               unsigned char Size,   unsigned char NrDec,
               unsigned char Attrib, unsigned char Pos)
{
  static ITEM_INFO OldItem;
  static BOOLEAN DisplayRefresh;

  Item.PEEAddress = Ptr;
  Item.Type       = Type;
  Item.Size       = Size;
  Item.NrDec      = NrDec;
  Item.Attrib     = Attrib;
  Item.Pos        = ( Pos == POS_RIGHT ? 11 - Size : Pos );

  if ( Item.NrDec ) disp_point |= 0x0200 >> (11-(Item.Pos+Size)+Item.NrDec);

  if (Item.Type  != OldItem.Type  || Item.Size       != OldItem.Size       ||
      Item.NrDec != OldItem.NrDec || Item.PEEAddress != OldItem.PEEAddress) {
    DisplayRefresh = TRUE;
    OldDispValue = -1;
    OldItem = Item;
  }

  switch (Item.Type) {
    case _BL:
         if ( (*Ptr != OldDispValue) || DisplayRefresh ) {
           OldDispValue   = *(BOOLEAN *)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
    case _U1:
         if ( (*Ptr != OldDispValue) || DisplayRefresh ) {
           OldDispValue   = *(unsigned char *)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
    case _S1:
         if ( (*Ptr != OldDispValue) || DisplayRefresh || (*(signed char *)Ptr == -1)) {
           OldDispValue   = *(signed char *)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
    case _TM:
    case _U2:
         if ( (*(unsigned short *)Ptr != OldDispValue) || DisplayRefresh ) {
           OldDispValue   = *(unsigned short*)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
    case _S2:
         if ( (*(signed short *)Ptr != OldDispValue) || DisplayRefresh || (*(signed short *)Ptr == -1)) {
           OldDispValue   = *(signed short*)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
    case _U4:
         if ( (*(unsigned long *)Ptr != OldDispValue) || DisplayRefresh || (*(unsigned long *)Ptr == -1L)) {
           OldDispValue   = *(unsigned long *)Ptr;
           DisplayRefresh = FALSE;
           ItemToDisp(&Item, Pos);
         }
      break;
  }
}


// =============================================================================
// display Screen MILKING
// =============================================================================
void DispScreenMilk(void)
{

  // Check Max displaytime
  if (ScreenNumberOld == ScreenNumber ) {
    if (Cnt1sec > (ScreenTime + EEDispTime)) {
      if ( EERobot ) ScreenMode =  SM_ROBOT;
      else           ScreenMode =  SM_MILKING;
      ScreenNumber = SCREEN_MILK_0;
    }
  } else{
    ScreenNumberOld = ScreenNumber;
    ScreenTime = Cnt1sec;
  }

  switch (ScreenMode ){
    case SM_MILKING:
         HandleScreenMilking();
      break;

    case SM_FEED_INFO:
         HandleScreenFeedInfo();
      break;

    case SM_MILK_INFO:
         HandleScreenMilkInfo();
      break;

    case SM_DISEASE_MILK_INFO:
         HandleScreenDiseaseInfo();
      break;

    case SM_COW_INFO:
         HandleScreenCowInfo();
      break;

    case SM_CALENDAR_INFO:
         HandleScreenCalendarInfo();
      break;

    case SM_PULSE_INFO:
         HandleScreenPulseInfo();
      break;

    case SM_ROBOT:
         HandleScreenRobot();
      break;

    default:
      break;

  } // end switch ( ScreenMode )
}


// =============================================================================
// display Screen SERVICE
// =============================================================================
void DispScreenService(void)
{
  unsigned char   stat = 0, dummy = 0;
          short   ctemp = 0, temp = 0;
  unsigned char   cv[5];

  switch ( ScreenMode ){
    case SM_ACT_DISEASE_INFO:
      switch( ScreenNumber ){
        case SCREEN_CONDUCT_RF:
        case SCREEN_CONDUCT_RR:
        case SCREEN_CONDUCT_LR:
        case SCREEN_CONDUCT_LF:
             if ( !(stat = GetConductivityValues( cv ))){
               // data ontvangen
               DispValue((unsigned char *)&(cv[ScreenNumber+1]), _U1, 3, 0, READ_ONLY, POS_RIGHT);
             } else {
               DispValue((unsigned char *)&(stat), _U1, 1, 0, READ_ONLY, POS_RIGHT);
             }
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CV_RF+ScreenNumber]), 6 );
             break;
        case SCREEN_TEMP:
             stat  = GetTemperature(TMP_DISP_SERVICE, &temp, &ctemp );
             if ( !stat ){
               DispValue((unsigned char *)&(  temp), _U2, 3, 1, READ_ONLY, POS_RIGHT);
               DispValue((unsigned char *)&( ctemp), _U2, 3, 1, READ_ONLY, POS_4);
             } else {
               //  fout melding
               DispValue((unsigned char *)&(stat), _U1, 1, 0, READ_ONLY, POS_RIGHT);
             }
             memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_TMP]), 3 );
        default:
          break;
      } // end switch (ScreenNumber, SM_ACT_DECEASE_INFO )
      break;
    case SM_CAL_TEMP_INFO:
         // calibreer temperatuur meet circuit
         stat = HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, cv);

         switch( ScreenNumber ){
           case SCREEN_CAL_TEMP_1:
                if ( !stat && cv[0] == IIC_READ_TEMP ){
                  DispValue((unsigned char *)&(cv[1]), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                } else {
                  DispValue((unsigned char *)&(dummy), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                  memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
                }
                memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_R35]), 6 );
             break;
           case SCREEN_CAL_TEMP_2:
                if ( !stat && cv[0] == IIC_READ_TEMP ){
                  DispValue((unsigned char *)&(cv[1]), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                } else {
                  DispValue((unsigned char *)&(dummy), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                  memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
                }
                memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_R40]), 6 );
             break;
           case SCREEN_CAL_TEMP_3:
                DispValue((unsigned char *)&(EE_U1), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_U35]), 6 );
             break;
           case SCREEN_CAL_TEMP_4:
                DispValue((unsigned char *)&(EE_U2), _U1, 3, 0, READ_ONLY, POS_RIGHT);
                memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_U40]), 6 );
             break;
           default: break;
         } // end switch(ScreenNumber)
      break;
    default:
         // standard service-mode, display service-items
         Item = *ScreenEEPtr;
         if (TestMode == BURN_IN) {
           DispValue(&EEAddressCopy, ScreenEEPtr->Type, ScreenEEPtr->Size,
                ScreenEEPtr->NrDec, ScreenEEPtr->Attrib, POS_RIGHT);
         } else {
           if ( ScreenEEPtr == &KeySoft ){
             DispValue(&SoftwareKey, ScreenEEPtr->Type, ScreenEEPtr->Size,
                  ScreenEEPtr->NrDec, ScreenEEPtr->Attrib, POS_RIGHT);
           } else {
             DispValue(ScreenEEPtr->PEEAddress, ScreenEEPtr->Type, ScreenEEPtr->Size,
                  ScreenEEPtr->NrDec, ScreenEEPtr->Attrib, POS_RIGHT);
           }
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[ScreenEEPtr->TextNr]), 6 );
      break;
  }
}



// =============================================================================
// display Screen Cor factor levelmeter
// =============================================================================
void DispScreenCorFacLvl(void)
{
  Item = *ScreenEEPtr;
  // geef correctie waarde zonder + en - teken weer
  DispValue(ScreenEEPtr->PEEAddress, ScreenEEPtr->Type, ScreenEEPtr->Size,
            0, ScreenEEPtr->Attrib, POS_RIGHT);

  memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CORR]), 6 );
}


// =============================================================================
// display Screen FAIL
// =============================================================================
void DispScreenFail(unsigned char ErrorNr)
{
  Item.NrDec = 0;
  OldDispValue = -1;

  // reset eventueel de communicatie error
  if ((CommunicationError == COMM_ERR_VC4) || (CommunicationError == COMM_INIT_VC4)){
    if ( Net.NetStat.Arbit )CommunicationError = 0;
  }

  switch (ErrorNr){
    case COMM_ERR_IC08: memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_ERR_IC08]), 10 );  break;
    case COMM_ERR_IC09: memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_ERR_IC09]), 10 );  break;
    case COMM_ERR_IC13: memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_ERR_IC13]), 10 );  break;
    case COMM_ERR_VC4:  memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_ERR_VC4]) , 10 );  break;
    case KEY_MODE_ERR:  memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_KEY_MODE_ERR]),  10 );  break;
    case KEY_HOLD_ERR:  memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_KEY_HOLD_ERR]),   7 );  break;
    case COMM_INIT_VC4: memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[TXT_COMM_INIT_VC4]), 10 );  break;
  }
}

// =============================================================================
// Clear Sreen
// =============================================================================
void DispClear(void)
{
  Item.NrDec = 0;
  memory_set( DispTable, ' ', 10 );
  OldDispValue = -1;
}

// =============================================================================
// Disp DHIA text
// =============================================================================
void DispDhia(void)
{
  Item.NrDec = 0;
  AsciiToDisp('d', 1, 1);
  AsciiToDisp('h', 2, 1);
  AsciiToDisp('i', 3, 1);
  AsciiToDisp('a', 4, 1);
  AsciiToDisp(' ', 5, 1);
  AsciiToDisp(' ', 6, 1);
  OldDispValue = -1;
}



// =============================================================================
// display Screen CALIBRATION LEVELMETER
// =============================================================================
void DispScreenCalLevel(void)
{
  Item.NrDec = 0;

  switch (CalState) {
    case CAL_15LTR:
      DispValue((unsigned char *)&PulsWidthCor, _U2, 4, 0, READ_ONLY, POS_RIGHT);
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_KG_15]), 6 );
    break;

    case CAL_5LTR:
      DispValue((unsigned char *)&PulsWidthCor, _U2, 4, 0, READ_ONLY, POS_RIGHT);
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_KG_05]), 6 );
    break;

    case CAL_0LTR:
      DispValue((unsigned char *)&PulsWidthCor, _U2, 4, 0, READ_ONLY, POS_RIGHT);
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_KG_00]), 6 );
    break;

    case CAL_VAL1:
      DispValue((unsigned char *)&EECal1, _U2, 4, 0, READ_ONLY, POS_RIGHT);
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CAL1]), 6 );
    break;

    case CAL_VAL2:
      DispValue((unsigned char *)&EECal2, _U2, 4, 0, READ_ONLY, POS_RIGHT);
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CAL2]), 6 );
    break;

    case CAL_START:
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CALIBRATE]), 10 );
    break;

    case CAL_RDY:
      memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CAL_READY]), 10 );
    break;
  }
  if ( CalState != CAL_START && CalState != CAL_RDY ){
    if ( ExtOutputP1State == P1_OFF_ON || ExtOutputP1State == P1_OFF ) {
      AsciiToDisp('n',6, 1);
    } else {
      AsciiToDisp('u', 6, 1);
    }
  }
}

// =============================================================================
// display Screen DHIA LEVEL
// =============================================================================
void DispScreenDHIALevel(void)
{
  static unsigned char  ScreenNumberOld;
         unsigned char  Value;
         unsigned short Zero = 0, MilkTest;


  // Check Max displaytime
  if (ScreenNumberOld == ScreenNumber) {
    if (Cnt1sec > (ScreenTime + EEDispTime)) {
      ScreenNumber = SCREEN_RESET;
    }
  } else{
    ScreenNumberOld = ScreenNumber;
    ScreenTime = Cnt1sec;
  }

  if (ScreenNumber == SCREEN_RESET) {
    MilkTest = 25;
    if (EEGoatMilkGlass == TRUE) {
      MilkTest = 3;
    }
    if (MlkAmountKg >= MilkTest) {
      DispValue((unsigned char *)&MlkAmountKg, _U2, 3, 1, READ_ONLY, POS_RIGHT);
    } else {
      DispValue((unsigned char *)&Zero, _U2, 3, 1, READ_ONLY, POS_RIGHT);
    }
    AsciiToDisp('t', 1, 1);
    if ( ExtOutputP1State == P1_OFF_ON || ExtOutputP1State == P1_OFF ) {
      AsciiToDisp('n',2, 1);
    } else {
      AsciiToDisp('u', 2, 1);
    }
    AsciiToDisp(' ', 3, 1);
  } else {
    // Cor. waard <= 10 is aftrekken 0-1.0 kg. Cor. waard > 10 optellen 0.1 -1.0 kg.
    Value = EEMmCorNiv1;
    if (Value > 10) Value -= 10;
    ScreenEEPtr = (ITEM_INFO *)&MmCorNiv1EE;
    DispValue(&Value, ScreenEEPtr->Type, ScreenEEPtr->Size, 1, ScreenEEPtr->Attrib, POS_RIGHT);
    if (EEMmCorNiv1 < 11 && EEMmCorNiv1 > 0 ) {
      AsciiToDisp('-', 8, 1);
    }
    memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_CORR]), 6 );
  }
}


// =============================================================================
// display Screen CLEANING
// =============================================================================
void DispScreenCleaningNiveau(void)
{
  if ( CommunicationError ){
    DispScreenFail( CommunicationError );
  } else {

    switch (MlkState) {
      case MLK_CLEANING:
           Item.NrDec = 0;
           memory_set( DispTable, '-', 10 );
        break;
      case MLK_CLEANING_END:
           Item.NrDec = 0;
           memory_set( DispTable, '_', 10 );
       break;
    }
  }
}



// =============================================================================
// display Screen DHIA
// =============================================================================
void DispScreenDHIAMemolac(void)
{
  static unsigned char ScreenNumberOld;

  // Check Max displaytime
  if (ScreenNumberOld == ScreenNumber) {
    if (Cnt1sec > (ScreenTime + EEDispTime)) {
      ScreenNumber = SCREEN_RESET;
    }
  } else {
    ScreenNumberOld = ScreenNumber;
    ScreenTime = Cnt1sec;
  }
  switch (ScreenNumber) {
    case SCREEN_RESET:
      DispValue((unsigned char *)&MlkAmountKg, _U2, 3, 1, READ_ONLY, POS_RIGHT);
      AsciiToDisp('t', 1, 1);
      if ( ExtPortCopy & K1 ) AsciiToDisp('u',3, 1);
      else                    AsciiToDisp('n',3, 1);

      if ( ExtPortCopy & K2 ) AsciiToDisp('u',4, 1);
      else                    AsciiToDisp('n',4, 1);
    break;

    case SCREEN_CORFAC:
      switch (MilkMeterType) {
        case MMT_MEMOLAC:
          DispValue((unsigned char *)&EEMmCorMem, _U1, 2, 0, WRITE, POS_RIGHT);
        break;
      }
    break;
  }
}

// =============================================================================
// display Screen KeyTest
// =============================================================================
void DispScreenKeyTest(void)
{
  Item.NrDec = 0;
  switch (ScreenNumber) {

    case KEY_REL:
         AsciiToDisp('K', 1, 1);
         AsciiToDisp('E', 2, 1);
         AsciiToDisp('Y', 3, 1);
         memory_set( &DispTable[3], ' ', 7 );
         DispBlinkLeds = DispLeds = 0;
         LampControl(LAMP_OFF, 0);
      break;

    case KEY_CLEAR:              memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_CLEAR_KEY]), 6 );  break;
    case KEY_WASH:               memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_WASH_KEY]),  6 );  break;
    case KEY_ENTER:              memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_ENTER_KEY]), 6 );
                                 LampControl(LAMP_ON, 0);
                                 break;
    case KEY_DISEASE_INFO:       memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_DECEAS_KEY]),6 );
                                 DispBlinkLeds &= ~IIC_CONDUCT_ATTENTION;
                                 DispLeds      |=  IIC_CONDUCT_ATTENTION;
                                 break;
    case KEY_COW_INFO:           memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_COW_KEY]), 6 );
                                 DispLeds      &= ~IIC_MILK_ATTENTION;       // bit uit anders knipperd led niet
                                 DispBlinkLeds |=  IIC_MILK_ATTENTION;       // laat led knipperen
                                 break;
    case KEY_FEED_INFO:          memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_FEED_KEY]), 6 );
                                 DispLeds |= IIC_ACTIVITY_ATTENTION;
                                 break;
    case KEY_MILK_INFO:          memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_MILK_KEY]), 6 );
                                 DispLeds      &= ~IIC_MILK_SEP_ATTENTION;       // bit uit anders knipperd led niet
                                 DispBlinkLeds |=  IIC_MILK_SEP_ATTENTION;       // laat led knipperen  -
                                 break;
    case KEY_SHIFT:              memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_SHIFT_KEY]), 6 );    break;
    case KEY_K3_GATE:            memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_CENTRL_KEY]), 6 );   break;
    case KEY_K4_GATE:            memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_ENTRAN_KEY]), 6 );   break;
    case KEY_K5_GATE:            memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_EXIT_KEY]), 6 );     break;
    case KEY_STOP_PULSE:         memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_STOP_KEY]), 6 );     break;
    case KEY_ACT_MILK_VALVE:     memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_ACT_MILK_KEY]), 6 ); break;
    case KEY_MANUAL_REL_CLUSTER: memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_MANUAL_KEY]), 6 );   break;
    case KEY_CLUSTER_REMOVAL:    memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_CLUSTR_KEY]), 6 );   break;
    case KEY_AUTO_RELEASE:       memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_AUTO_KEY]), 6 );     break;
    case KEY_CALENDAR_INFO:      memory_cpy((unsigned char *)&(DispTable[4]), (unsigned char *)&(SrvTxtString[TXT_CALEND_KEY]), 6 );
                                 DispLeds      &= ~IIC_CALENDAR_ATTENTION;       // bit uit anders knipperd led niet
                                 DispBlinkLeds |=  IIC_CALENDAR_ATTENTION;       // laat led knipperen
                                 break;
    case '0':                    AsciiToDisp('0', 10, 1);  break;
    case '1':                    AsciiToDisp('1', 10, 1);  break;
    case '2':                    AsciiToDisp('2', 10, 1);  break;
    case '3':                    AsciiToDisp('3', 10, 1);  break;
    case '4':                    AsciiToDisp('4', 10, 1);  break;
    case '5':                    AsciiToDisp('5', 10, 1);  break;
    case '6':                    AsciiToDisp('6', 10, 1);  break;
    case '7':                    AsciiToDisp('7', 10, 1);  break;
    case '8':                    AsciiToDisp('8', 10, 1);  break;
    case '9':                    AsciiToDisp('9', 10, 1);  break;
  }
}

// =============================================================================
// display Screen Peripheral Test
// =============================================================================
void DispScreenPeripheralTest(void)
{
  memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_PT_I1+ScreenNumber]), 6 );

  switch ( ScreenNumber ){
    case  0: AsciiToDisp( ReadI1Sensor() == SENSOR_STATE_CLOSE ? '0' : '1', 10, 1); break;
    case  1: AsciiToDisp( ReadI2Sensor() == SENSOR_STATE_CLOSE ? '0' : '1', 10, 1); break;
    case  2: AsciiToDisp( ReadI3Sensor() == SENSOR_STATE_CLOSE ? '0' : '1', 10, 1); break;
    case  3: AsciiToDisp( ReadI4Sensor() == SENSOR_STATE_CLOSE ? '0' : '1', 10, 1); break;
    case  4:
    case  5:
    case  6: AsciiToDisp( (PORTA & (P_3<<(ScreenNumber-4))) ? 0x31 : 0x30, 10, 1);  break;
    case  7: AsciiToDisp( (PORTD & P_5) ? 0x31 : 0x30, 10, 1);  break;
    case  8:
    case  9:
    case 10:
    case 11:
    case 12: AsciiToDisp( (ActExtPortCopy &(K1<< (ScreenNumber-8))) ? 0x31 : 0x30, 10, 1);  break;
    case 13:
    case 14:
    case 15: AsciiToDisp( (PORTA & (P_0<<(ScreenNumber-13))) ? 0x31 : 0x30, 10, 1);         break;
    case 16: if ( EERelaisTime ){ AsciiToDisp( (PORTA & P_6) ? 0x30 : 0x31, 10, 1);         break; }
             else               { AsciiToDisp( pt_result+0x30, 10, 1);                      break; }
    default: break;
  }
}

// =============================================================================
// display Screen CLEANING GENERAL
// =============================================================================
void DispScreenCleaningGeneral(void)
{
  Item.NrDec = 0;

  if ( CommunicationError ){
    DispScreenFail( CommunicationError );
  } else {
    switch (CleanPhase) {
      case CLEAN_PHASE_0:
      case CLEAN_PHASE_START:
      case CLEAN_PHASE_I2_CONTROLLED_1:
      case CLEAN_PHASE_I2_CONTROLLED_0:
      case CLEAN_PHASE_I2_CONTROLLED_START:
           memory_set( DispTable, '-', 10 );
        break;
      case CLEAN_PHASE_I2_CONTROLLED:
      case CLEAN_PHASE_1:
           memory_set( DispTable, ' ', 10 );

           AsciiToDisp('P', 4, 1);
           AsciiToDisp('1', 5, 1);

           if (MilkMeterType == MMT_MEMOLAC) {
             switch (CleanState) {
               case FILL_MILK_METER:
                    AsciiToDisp('-', 6, 1);
                    AsciiToDisp('F', 7, 1);
                 break;

               case EMPTY_MILK_METER:
                    AsciiToDisp('-', 6, 1);
                    AsciiToDisp('E', 7, 1);
                 break;
             }
           }
        break;
      case CLEAN_PHASE_2:
           memory_set( DispTable, ' ', 10 );

           AsciiToDisp('P', 5, 1);
           AsciiToDisp('2', 6, 1);
        break;

      case CLEAN_PHASE_3:
           memory_set( DispTable, ' ', 10 );

           AsciiToDisp('P', 5, 1);
           AsciiToDisp('3', 6, 1);
        break;

      case CLEAN_PHASE_PRE_CLEAN:
           memory_set( DispTable, ' ', 10 );

           AsciiToDisp('P', 5, 1);
           AsciiToDisp('4', 6, 1);
        break;

      case CLEAN_PHASE_END:
      case CLEAN_PHASE_I2_CONTROLLED_END:
           memory_set( DispTable, '_', 10 );
      break;
    }
  }
}

// =============================================================================
// display Screen MILKING
// =============================================================================
void DispScreenError(void)
{
  Item.NrDec = 0;
  switch (ScreenNumber) {
    case SCREEN_ERROR_INFO_0:
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_OUT_ERR]), 10 );
      break;
    case SCREEN_ERROR_INFO_1:
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_MTR_ERR]), 10 );
      break;
  }
}

#if ( _EMC_TEST_ )
// =============================================================================
// display Screen EmcTest
// =============================================================================
void DispScreenEmcTest(void )
{
  unsigned char  cv[6];
  unsigned char  stat, dummy = 0;
  unsigned short Temp;

  Item.NrDec = 0;

  switch (ScreenNumber) {
    case SCREEN_EMC_TEST_0:
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(EmcString), 6 );
      break;
    case SCREEN_EMC_TEST_1:
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(EmcString), 6 );

         TakeOffState = TO_RELEASE_MAN;
         MlkState = MLK_INIT;
         NiveauMeasuring();

      break;
    case SCREEN_EMC_TEST_2:
         if ( !(stat = GetConductivityValues( cv ))){
           // data ontvangen
           DispValue((unsigned char *)&(cv[ScreenNumber+1]), _U1, 3, 0, READ_ONLY, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&(stat), _U1, 1, 0, READ_ONLY, POS_RIGHT);
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_CV_RF+ScreenNumber]), 6 );
      break;
    case SCREEN_EMC_TEST_3:
         TurnMotorOn(0);
         if ( !(Temp = GetTemperature()) ){
           DispValue((unsigned char *)&(Temp), _U2, 3, 1, READ_ONLY, POS_RIGHT);
         } else {
           DispValue((unsigned char *)&(dummy), _U1, 3, 0, READ_ONLY, POS_RIGHT);
           memory_set((unsigned char *)&(DispTable[7]), '-', 3 );
         }
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(TextString[EELanguage][TXT_TMP]), 6 );
      break;
    case SCREEN_EMC_TEST_4:
         TurnMotorOff(0);
         DispValue((unsigned char *)&(PulsWidthMeas), _U2, 4, 0, WRITE, POS_7);
         memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_LEVEL_COUNTER]), 6 );
      break;
  }
}
#endif

// =============================================================================
// display Screen
// =============================================================================
void DispScreen(void)
{
  static unsigned char OldMode;

  if (KeyMode != OldMode) {
    OldMode = KeyMode;
    // Redisplay
    OldDispValue = -1;
  }

  // reset knipperen
  DispFlash  = 0;
  disp_point = 0;

  switch (KeyMode) {
    case KM_RBT_MILKING :
    case KM_MILKING :
         DispScreenMilk();
      break;
    case KM_CLEANING:
         switch(MilkMeterType) {
           case MMT_NIVEAU:
                DispScreenCleaningNiveau();
             break;
           case MMT_TAKE_OFF_SENSOR:
           case MMT_MEMOLAC:
           case MMT_VP4001:
           case MMT_VFLOW:
                DispScreenCleaningGeneral();
             break;
           default:
                DispScreenCleaningNiveau();
             break;
         }
      break;
    case KM_ADDRESS :
    case KM_SERVICE :
         DispScreenService();
      break;
    case KM_TEST :
         DispScreenKeyTest();
      break;
    case KM_P_TEST :
         DispScreenPeripheralTest();
      break;

    case KM_CAL    :
         switch(MilkMeterType) {
           case MMT_NIVEAU:
                DispScreenCalLevel();
             break;
           default:
                DispClear();
             break;
         }
      break;
    case KM_DHIA    :
         switch(MilkMeterType) {
           case MMT_NIVEAU:
                DispScreenDHIALevel();
             break;
           case MMT_MEMOLAC:
           case MMT_VFLOW:
           case MMT_VP4001:
           case MMT_TAKE_OFF_SENSOR:
                DispScreenDHIAMemolac();
             break;
           default:
                DispDhia();
             break;
         }
      break;
    case KM_DISP_MSG:
         if ( TimeControl( Cnt10msec, MsgDispTime+50)){
           KeyMode      = KeyModeOld;
           ScreenMode   = ScreenModeOld;
           ScreenNumber = ScreenNumberOld;
         } else {
           memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(MsgStrings[ScreenNumber]), 10 );
         }
      break;
    case KM_ERROR:
         DispScreenError();
      break;
#if ( _EMC_TEST_ )
    case KM_EMC_TEST:
         DispScreenEmcTest();
      break;
#endif
//    case KM_RBT_MILKING :
//         DispScreenRobotMilking();
    default:
      break;
  }
}



