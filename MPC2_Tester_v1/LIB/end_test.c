
#include "../lib/io6811.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../lib/text.h"       
#include "../lib/time.h"       
#include "../lib/eehc11.h"       
#include "../lib/watchdog.h"       
#include "../lib/convert.h" 
#include "../key/key.h"
#include "../key/keyint.h"
#include "../key/disp.h"
#include "../usx/fake_usx.h"
#include "end_test.h" 

const unsigned char SendUniqueAddress[11] = {0x00, 0x07, 0x83, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x48};

BOOLEAN EndTest;

#pragma codeseg(EEPAGE)

// =============================================================================
void HandleKeyTest( void)
{
  unsigned char KTArray[61];
  unsigned char KeyCnt = 0;                                    
   
  memory_clear( KTArray, 60); 
  EndTest = TRUE;
  
  while(KeyCnt < 27 ) {
    ReadKey();
    WatchDog();
    ScreenNumber = KeyA;
    DispScreenKeyTest(); 
    Display();
    
    switch (KeyA){
      case KEY_CLEAR:              if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_ENTER:              if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_DISEASE_INFO:       if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_COW_INFO:           if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_FEED_INFO:          if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_MILK_INFO:          if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_SHIFT:              if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_K3_GATE:            if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_K4_GATE:            if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_K5_GATE:            if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;                                                             
      case KEY_STOP_PULSE:         if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_ACT_MILK_VALVE:     if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_MANUAL_REL_CLUSTER: if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_CLUSTER_REMOVAL:    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_AUTO_RELEASE:       if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_CALENDAR_INFO:      if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '0':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '1':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '2':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '3':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '4':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '5':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '6':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '7':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '8':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case '9':                    if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      case KEY_WASH:               if ( KTArray[KeyA] == 0) {KTArray[KeyA] = 1; KeyCnt++; } break;
      default : break;                             
    }                               
  }
}

// =============================================================================
void CheckForEndTest(void)
{
  unsigned char  msg[12];
  unsigned short time;
  OneWireMessage  *p_OneWireMessage = (OneWireMessage *)(msg);
  
  EndTest = FALSE;
 
  // test of de eind test actief is --------------------------------------------
  // zet eerst de 'Uniek Address zoek vlag' ------------------------------------
  
  msg[0] = 0x00; msg[1] = 0x01; msg[2] = 0x01; msg[3] = 0xaa;
  HandleWriteIic(PICA_ADDRESS, IIC_ONE_WIRE_WRITE, 4, (unsigned char *)&msg );
  
  // controleer of het bericht verwerkt is -------------------------------------
  time = Cnt10msec;
  do {
    HandleReadIic(PICA_ADDRESS, IIC_ONE_WIRE_READ, 10, (unsigned char *)&msg );
    // controleer op time-out --------------------------------------------------
    if ( TimeControl( Cnt10msec, time+200 )) return;
  } while( (msg[1] != 0xff ) || (msg[3] == 0x02 ));

  // stuur het unieke address --------------------------------------------------
  msg[0] = 0x00; msg[1] = 0x07; msg[2] = 0x83; msg[3] = 0x77; msg[4] = 0x46; 
  msg[5] = 0xf8; msg[6] = 0xff; msg[7] = 0xff; msg[8] = 0x00; msg[9] = 48;

  HandleWriteIic(PICA_ADDRESS, IIC_ONE_WIRE_WRITE, 10, (unsigned char *)&msg );

  // controleer of het bericht verwerkt is -------------------------------------
  time = Cnt10msec;
  do {
    HandleReadIic(PICA_ADDRESS, IIC_ONE_WIRE_READ, 10, (unsigned char *)&msg );
    // controleer op time-out --------------------------------------------------
    if ( TimeControl( Cnt10msec, time+200 )) return;
  } while( (msg[1] != 0xff ) || (msg[3] == 0x02 ));

  if ( msg[3] == 1 ){

    memory_cpy((unsigned char *)&(DispTable[0]), (unsigned char *)&(SrvTxtString[TXT_END_TEST]), 10 );
    Display();
    
    do {
      // EindTest ----------------------------------------------------------------
      // geef verkort address (2) ------------------------------------------------
      msg[0] = 0x00; msg[1] = 0x07; msg[2] = 0x04; msg[3] = 0x77; msg[4] = 0x46; 
      msg[5] = 0xf8; msg[6] = 0xff; msg[7] = 0xff; msg[8] = 0x00; msg[9] = 2;

      HandleWriteIic(PICA_ADDRESS, IIC_ONE_WIRE_WRITE, 10, (unsigned char *)&msg );
 
      // controleer of het bericht verwerkt is -----------------------------------
      time = Cnt10msec;
      do {
        HandleReadIic(PICA_ADDRESS, IIC_ONE_WIRE_READ, 10, (unsigned char *)&msg );
        WatchDog();
        // controleer op time-out --------------------------------------------------
        if ( TimeControl( Cnt10msec, time+200 )) return;
      } while( msg[3] == 0x02 );
    } while (msg[3] != 0x70 );

    HandleKeyTest();

    do {
      // verkorte address geaccepteerd -------------------------------------------
      // print stickers ----------------------------------------------------------  
      p_OneWireMessage->Address         = 2;
      p_OneWireMessage->MsgLength       = 2;
      p_OneWireMessage->Message         = 0x01;
      (p_OneWireMessage->Array)[0].Data = EE_U1;           // REF 1
      (p_OneWireMessage->Array)[1].Data = EE_U2;           // REF 2

      HandleWriteIic(PICA_ADDRESS, IIC_ONE_WIRE_WRITE, 5, (unsigned char *)&msg );

      time = Cnt10msec;
      do {
        HandleReadIic(PICA_ADDRESS, IIC_ONE_WIRE_READ, 10, (unsigned char *)&msg );
        WatchDog();
        // controleer op time-out --------------------------------------------------
        if ( TimeControl( Cnt10msec, time+200 )) return;
      } while( ((msg[1] != 0xff ) || (msg[3] == 0x02 )));
    } while ( msg[3] != 0x70 );
  }
}
