#include "../key/key.h"
#include "../lib/eehc11.h"
#include "../lib/convert.h"
#include "../lib/extport.h"
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../motor/motor.h"
#include "../appl/milk.h"
#include "../appl/takeoff.h"


void HandleKeyPeripheralTest(void)
{   
  switch (KeyA) {
    case KEY_WASH:
         if (KeyHold == FALSE) {
           switch (MilkMeterType) {
             case MMT_NIVEAU:
                  KeyMode      = KM_CAL;
                  MlkState     = MLK_CAL;
                  ScreenNumber = SCREEN_RESET;
               break;
             case MMT_MEMOLAC:
             case MMT_SURGE:
             default:
                  KeyMode      = KM_CLEANING;
                  MlkState     = MLK_CLEANING;
                  ScreenNumber = SCREEN_RESET;
               break;
           } // end switch ( MilkMeterType) 
         }
      break;
    case '0':
         switch ( ScreenNumber ){
           case  4:
           case  5:
           case  6: PORTA &= ~(P_3<<(ScreenNumber-4));       break;   // P1 t/m P3
           case  7: PORTD &= ~P_5;                           break;   // lamp
           case  8:
           case  9:
           case 10:
           case 11:
           case 12: ExtPortCopy &= ~(K1<<(ScreenNumber-8));  break;   // K1 t/m K5
         }
      break;
    case '1':
         switch ( ScreenNumber ){
           case  4:
           case  5:
           case  6: PORTA |= (P_3<<(ScreenNumber-4));        break;   // P1 t/m P3
           case  7: PORTD |=  P_5;                           break;   // lamp
           case  8:
           case  9:
           case 10:
           case 11:
           case 12: ExtPortCopy |= (K1<<(ScreenNumber-8));   break;   // K1 t/m K5
           case 16: if ( EERelaisTime ) pt_result = RelaisControl();
                    else                pt_result = MotorControl(TURN, 0); 
                    // doordat het lopen van de motor lang duurt en de toets nog
                    // een keer wordt terug gegeven (ReadKey) wordt KeyHold TRUE en
                    // daarmee KeyOld 0, gevolg motor wordt nog een keer aangestuurd 
                    while(ReadKey() == '1');
                    break; 
         }
      break;
    case KEY_ENTER:
        ScreenNumber++;
        if ( ScreenNumber != 7 ) LampControl(LAMP_OFF, 0);
        if (ScreenNumber == 17) {
          ScreenNumber = 0;
        }
      break;
    case KEY_SHIFT:
        if (ScreenNumber == 0) {
          ScreenNumber = 17;
        }
        ScreenNumber--;
        if ( ScreenNumber != 7 ) LampControl(LAMP_OFF, 0);
      break;
    default: 
      break;
  } // end switch(KeyA) 
}