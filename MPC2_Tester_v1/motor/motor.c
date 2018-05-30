// -----------------------------------------------------------------------------
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// -----------------------------------------------------------------------------


#include "../lib/io6811.h"
#include "../lib/boolean.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/adconv.h"
#include "../lib/iic.h"
#include "../lib/schedule.h"
#include "../lib/watchdog.h"
#include "../usx/fake_usx.h"
#include "../lib/error.h"
//#include "../lib/port.h"
#include "../lib/time.h"
#include "../lib/extport.h"
#include "../key/keyint.h"
#include "motor.h"


// -----------------------------------------------------------------------------
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// -----------------------------------------------------------------------------


#include "../lib/eeprom.h"
#include "../lib/eehc11.h"

#pragma codeseg(COMMON)

// =============================================================================
//                                    b7 b6 b5 b4 b3 b2 b1 b0
//                                    x  x  x  x  M4 M3 M2 M1
// EEMtrType :     0 -> 'normal'
//                 1 -> 'invers'
// EEMtrOutConfig: 0 -> motor
//                 1 -> relay
// EERelayConfig:  0 -> 'niets'
//                 1 -> 'relay'
// -----------------------------------------------------------------------------
// I-contact:
//              --------                             ---------
//  'normal'            -----------------------------
//                     -------------------------------
//  'invers'    -------                               --------
// =============================================================================

#define DISPLAY_OUTPUT_RES_TIME      200     // 2000ms

#define PortE           (unsigned char *)0xB00A
#define PortA           (unsigned char *)0xB000
#define P_6             0x40      
#define P_7             0x80      


// declaratie globale variabelen 
unsigned char  Motor_Fail_Cntr[ NR_OF_MOTORS];               
unsigned short HalfTurnTime[NR_OF_MOTORS];
unsigned char  MotorFailCntrMax;

const MOTOR_ROM_INFO Motor_Rom_Array[ NR_OF_MOTORS]=
{
  {  PortA, P_6 , PortE, P_7 },
};

// =============================================================================
unsigned char MotorBreakOnOff( unsigned char cmnd)
{
  unsigned char msg[3];
  unsigned char stat = 0;

  msg[0] = cmnd;       // rem aan 
  stat |= HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTC, 1, (unsigned char *)msg );

  msg[0] = 0x55; msg[1] = 0xAA; msg[2] = 0x00;    // reset stroom-bewaking  
  stat |= HandleWriteIic( PICA_ADDRESS, IIC_WRITE_MCSG, 3, (unsigned char *)msg );

  ExtPortCopy |= 0x20;                            // schakel relais weer in 
  stat |= HandleWriteIic(PICA_ADDRESS, IIC_WRITE_PORTB, 1, (unsigned char *)&ExtPortCopy );

  stat |= HandleReadIic( PICA_ADDRESS, 0x10, 2, (unsigned char *)msg );

  if (stat) return 0xff;
  else      return msg[1];
}

// =============================================================================
void PowerRelayOff(void)
{
  // schrijf poortb
  ExtPortCopy &= ~0x20;                            // schakel relais uit
  HandleWriteIic(PICA_ADDRESS, IIC_WRITE_PORTB, 1, (unsigned char *)&ExtPortCopy );
}

// =============================================================================
void MotorOutEnable(void)
{
  PORTA |= MOTOR_OUT_FREE;
}


// =============================================================================
void MotorResetErrors( void)
{
  unsigned char i;

  for ( i= 0; i < NR_OF_MOTORS; i++ ) Motor_Fail_Cntr[ i]= 0;
}


// =============================================================================
void init_motor( unsigned char MaxMotorFailCntr )
{
  
  ResetErrorInfo();
  MotorResetErrors();
  MotorFailCntrMax = MaxMotorFailCntr;
 
  // default: Motor, invers 
  WriteEEpromByte( 0xff, &EEMtrType);
  WriteEEpromByte( 0x00, &EEMtrOutConfig);

  // motor rem aan 
  MotorBreakOnOff(1);
  
}


// =============================================================================
unsigned char ChkMtrInputLevel( unsigned char level)
{
  unsigned char i;

  // lees de ingang analoog in 
  for ( i= 0; i < NR_OF_CONTACT_READS; i++) {
    if ( ADConv1Of8( MOTOR_INPUT_CHANNEL ) > MOTOR_INP_LEVEL ){
      // ingang hoog, is dit overeenkomstig gewenste niveau ??? 
      if ( !level ) return 0;
    } else{
      // ingang laag, is dit overeenkomstig gewenste niveau ??? 
      if ( level ) return 0;
    }
  }
  return(1);
}

// =============================================================================
unsigned char ChkMtrInput( unsigned char level)
{

  if ( ADConv1Of8( MOTOR_INPUT_CHANNEL ) > MOTOR_INP_LEVEL ){
    if ( !level ) return 0;
  } else {
    if ( level ) return 0;
  }
  return 1;
}


// ============================================================================
MOTOR_CURRENT GetMotorCurrent( unsigned char *mtr_crrnt )
{
  MOTOR_CURRENT motorcurrent;

  *mtr_crrnt = ADConv1Of8( MOTOR_CURRENT_CHANNEL);

  if ( *mtr_crrnt > 30) motorcurrent = CURRENT_ON;    // veranderd van 25 in 35 voor
  else {                                              // continu aansturen lamp
    if ( *mtr_crrnt < 12) motorcurrent= CURRENT_OFF;  // 35 ~= 8 Watt (300mA)
    else motorcurrent = CURRENT_CHANGING;
  }

  return motorcurrent;
}

// =============================================================================
MOTOR_CURRENT MotorCurrent( void )
{
  unsigned char dum;

  return GetMotorCurrent( &dum );
}


// ============================================================================
void MotorError( unsigned char MotorNumber)
{
  Motor_Fail_Cntr[ MotorNumber]++;
  if ( Motor_Fail_Cntr[ MotorNumber] >= MotorFailCntrMax ) {
    WriteErrorInfo( MotorNumber+MTR1_ERR_NR );
    Motor_Fail_Cntr[ MotorNumber]= 0;
  }
}


// =============================================================================
void MotorGroupError( void )
{
  WriteErrorInfo( MTR_GRP_ERR_NR );
}



// =============================================================================
void TurnMotorOn( unsigned char MotorNumber )
{
  *( Motor_Rom_Array[ MotorNumber].MtrOutAdres) &=
    ( 0xff - Motor_Rom_Array[ MotorNumber].MtrOutBit);
}


// =============================================================================
void TurnMotorOff( unsigned char MotorNumber )
{
  *( Motor_Rom_Array[ MotorNumber].MtrOutAdres) |=
    Motor_Rom_Array[ MotorNumber].MtrOutBit;
}



// =============================================================================
unsigned char DetermineHalfMotorTurn ( unsigned char MotorNumber)
{
  unsigned int StartTime;
  unsigned int HalfTurnTimeOut = Cnt10msec + MAX_MOTOR_TURN_TIME;

  // zoek eerst het overname contact 
  while (MotorControl(MotorNumber, TURN)){
    if ( TimeControl( Cnt10msec, HalfTurnTimeOut ) ) return 1;
  }

  // bepaal halve omwentelingstijd 
  StartTime = Cnt10msec;
  if ( MotorControl(MotorNumber, TURN ) )return(1);
  else HalfTurnTime[MotorNumber] = (Cnt10msec - StartTime) / 2;
  return 0;
}

// =============================================================================
unsigned char RelaisControl( void )
{
  unsigned short x, time;
  
  TurnMotorOn(0); 
  if ( x = (EERelaisTime % 10 )){       
    // milli seconden 
    time = TCNT + (x*2000);
    while( !TimeControl( TCNT, time));
  } 
  if ( x = (EERelaisTime / 10 )){       
    // 10_milli seconden 
    time = Cnt10msec + x;
    while( !TimeControl( Cnt10msec, time)){ WatchDog(); TaskExit(); }
  } 
  TurnMotorOff(0); 
  return 0;  
}

// =============================================================================
// input:  MotorNumber: the number of the motor which has to be turned
// output: returns 0 if motor has turned around, otherwise errorcode
// -----------------------------------------------------------------------------
unsigned char MotorControl( unsigned char MotorNumber, unsigned char Turn)
{
  MOTOR_STATE      MotorState;
  MOTOR_CURRENT    motorcurrent;
  unsigned short   MotorStartTime;
  unsigned short   TimeOut;
  unsigned short   input_start, input_stop;
  unsigned char    motorrtn_value = 0;

  unsigned char    m_type = ( EEMtrType & (0x01 << MotorNumber));
  unsigned char    mtr_crrnt;
  unsigned short   MtrMaxTime;
  unsigned short   cnt_rst_time;
  unsigned short   ContactCntr;
  BOOLEAN          BMaxCurrent = FALSE;

  // controleer of MotorNumber correct is 
  if ( MotorNumber >= NR_OF_MOTORS ){
    motorrtn_value = MOTOR_NUMBER_INVALID;
    goto exit;
  }

   // controleer of de output geconfigureerd is voor een motor 
  if ( EEMtrOutConfig & ( 0x01 << MotorNumber) ){
    motorrtn_value = MOTOR_OUT_CONF_ERROR;
    goto exit;
  }

  // controleer of de motor OK is 
  if ( CheckErrorInfo( MotorNumber) || CheckErrorInfo( MTR_GRP_ERR_NR ) ){
    motorrtn_value = MOTOR_FAILURE;
    goto exit;
  }

  // Moet motor de eerste draai maken ? 
  if ( Turn == FIRST_TURN ) {
    // controleer of de motor in de goede stand staat, invers: input low 
    if ( !(ChkMtrInput( m_type != 0 ? 0 : 1) )) {
      // Zet motor in de ruststand 
      ContactCntr = 0;
      TurnMotorOn( MotorNumber );
      MotorStartTime = Cnt10msec;
      do {
        WatchDog();
        if ( ChkMtrInput( m_type != 0 ? 0 : 1 )) {
          if ( ++ContactCntr >= ( m_type != 0 ? MAX_LEVEL_CNTR_INV : MAX_LEVEL_CNTR_NORM) ) {
            // teruglees ingang is hoog 

            TurnMotorOff( MotorNumber );
            motorrtn_value = MOTOR_TURNED;
            break;
          }
        }
      } while( !TimeControl(Cnt10msec, MotorStartTime +  MAX_MOTOR_TURN_TIME ));
      TurnMotorOff( MotorNumber );

      if ( motorrtn_value != MOTOR_TURNED ) {
        motorrtn_value = MOTOR_INPUT_CONTACT_ERROR;
        goto exit;
      }
    }
  }

  // schakel de motor aan 
  TurnMotorOn( MotorNumber);
  MotorStartTime= Cnt10msec;
  MotorState= MTR_CURRENT_CHECK;


  // motor state machine 
  while (1) {
    switch ( MotorState) {
      case MTR_CURRENT_CHECK:                   // state 20 --------------------

        // check if within a maximum time motorcurrent will be on 
        TimeOut= Cnt10msec + MOTOR_CURRENT_CHANGED_TIME;
        do {
          motorcurrent = GetMotorCurrent(&mtr_crrnt);
          WatchDog();
        } while ( !( motorcurrent == CURRENT_ON) && !( TimeControl( Cnt10msec, TimeOut)));

        if ( motorcurrent == CURRENT_ON) {
          if ( Turn == SECOND_TURN ) MotorState = MTR_INPUT_CHECK_STOP;
          else                       MotorState = MTR_INPUT_CHECK_START;
        } else {
          // no current, so motorfailure 
          TurnMotorOff( MotorNumber);
          MotorError( MotorNumber);
          motorrtn_value = MOTOR_CURRENT_ERROR;
          MotorState = MTR_WAIT_CURRENT_LOW;
        }
        break;

      case MTR_INPUT_CHECK_START:               // state 21 --------------------
        // controleer of de teruglees ingang laag is/wordt 
        while (1) {
          WatchDog();

          // indien invers level wordt != 0, anders 0 
          if (ChkMtrInputLevel( m_type )) {

            // bewaar de tijd tbv configuratie controle 
            input_start = Cnt10msec;

            // I-contact heeft juiste waarde, laat motor minimale tijd lopen 
            if ( Turn == TURN ) {
              while (!TimeControl(Cnt10msec, MotorStartTime + MIN_MOTOR_ON_TIME  ));
            } else {
              while (!TimeControl(Cnt10msec, MotorStartTime + 0                  ));
            }

            MotorState = MTR_INPUT_CHECK_STOP;
            break;
          } else {
            if (TimeControl( Cnt10msec, MotorStartTime + MAX_MOTOR_TURN_TIME )) {

              // motor heeft maximale tijd gedraaid, schakel de motor uit 
              TurnMotorOff( MotorNumber );
              MotorError( MotorNumber);
              motorrtn_value = MOTOR_INPUT_CONTACT_ERROR;
              MotorState = MTR_WAIT_CURRENT_LOW;
              break;
            } else {
              // controleer de motorstroom 
              GetMotorCurrent(&mtr_crrnt);
              if ( mtr_crrnt > MAX_MOTOR_CURRENT ){
                if ( BMaxCurrent == FALSE ){
                  MtrMaxTime = Cnt10msec;
                  BMaxCurrent = TRUE;
                } else {
                  if ( TimeControl(Cnt10msec, MtrMaxTime + MAX_CUR_MTR_TIME )){
                    // er loopt een te grote stroom gedurende een te lange tijd
                    // zet motor uit, genereer error 
                    TurnMotorOff( MotorNumber );
                    MotorError( MotorNumber);
                    motorrtn_value = MOTOR_CUR_HIGH_ERROR;
                    MotorState = MTR_WAIT_CURRENT_LOW;
                    break;
                  }
                }
              } else BMaxCurrent == FALSE;
            }
          }
        }
        break;

      case MTR_INPUT_CHECK_STOP:                // state 22 --------------------
        ContactCntr = 0;

        // controleer of de teruglees ingang laag is/wordt 
        while (1) {
          WatchDog();

          // indien invers level moet laag zijn (0), anders hoog 
          if ( ChkMtrInput( m_type != 0 ? 0 : 1 )) {
            // zet tijd om de ContactCntr te resettten indien 'inverse motor' 
            if( m_type ) cnt_rst_time = Cnt10msec + 30;   // 300 millisec 

            if ( ++ContactCntr >= ( m_type != 0 ? MAX_LEVEL_CNTR_INV : MAX_LEVEL_CNTR_NORM) ) {
              // teruglees ingang is hoog 

              TurnMotorOff( MotorNumber );
              motorrtn_value = MOTOR_TURNED;


              // Eerste helft gedraaid of tweede helft 
              // indien eerste helft, dan had het contact nog niet gezien mogen worden
              if ( Turn == FIRST_TURN ) {
                // Halve draaitijd is te lang, draai nu weer de helft
                TurnMotorOn( MotorNumber );
                while ( !TimeControl( Cnt10msec, MotorStartTime + HalfTurnTime[MotorNumber]) );
                TurnMotorOff( MotorNumber );
              } else {
                input_stop = Cnt10msec;
                if ( Turn == TURN ){
                  // contoleer alleen bij een volledige omwenteling 
                  if ( ( input_stop - input_start ) < MIN_CONTACT_LENGTH ){
                    MotorError( MotorNumber);
                    motorrtn_value = MOTOR_CONF_ERROR;
                  }
                }
              }

              // decrement failurecounter, indien geen error 
              if ( !motorrtn_value ){
                if ( Motor_Fail_Cntr[ MotorNumber] > 0) Motor_Fail_Cntr[ MotorNumber]--;
              }
              MotorState = MTR_WAIT_CURRENT_LOW;
              break;
            }
          } else {
            // indien 'invers' niet aftellen 
            if (!m_type ){
              ContactCntr = ContactCntr <= 0 ? 0 : --ContactCntr;
            } else {
              // 'inverse' motor, tijd om ContactCntr te resetten ( 2 omwentelingen?)
              if ( TimeControl( Cnt10msec, cnt_rst_time ))ContactCntr = 0;
            }
          }

          // controleer de time_out 
          if (TimeControl( Cnt10msec, MotorStartTime +
               ( Turn == FIRST_TURN ? HalfTurnTime[MotorNumber] : MAX_MOTOR_TURN_TIME ))) {

            // motor heeft maximale tijd gedraaid, stop motor 
            TurnMotorOff( MotorNumber );
            if ( Turn != FIRST_TURN ){
              MotorError( MotorNumber);
              motorrtn_value = MOTOR_INPUT_CONTACT_ERROR;
            } else {
              motorrtn_value = MOTOR_TURNED;
            }

            MotorState = MTR_WAIT_CURRENT_LOW;
            break;
          } else {
            // controleer de motorstroom 
            GetMotorCurrent(&mtr_crrnt);
            if ( mtr_crrnt > MAX_MOTOR_CURRENT ){
              if ( BMaxCurrent == FALSE ){
                MtrMaxTime = Cnt10msec;
                BMaxCurrent = TRUE;
              } else {
                if ( TimeControl(Cnt10msec, MtrMaxTime + MAX_CUR_MTR_TIME )){
                  // er loopt een te grote stroom gedurende een te lange tijd
                  // zet motor uit, genereer error 
                  TurnMotorOff( MotorNumber );
                  MotorError( MotorNumber);
                  motorrtn_value = MOTOR_CUR_HIGH_ERROR;
                  MotorState = MTR_WAIT_CURRENT_LOW;
                  break;
                }
              }
            } else BMaxCurrent == FALSE;
          }
        }
        break;

      case MTR_WAIT_CURRENT_LOW:                // state 23 --------------------
        TimeOut= Cnt10msec + MOTOR_CURRENT_CHANGED_TIME;
        do {
          motorcurrent= GetMotorCurrent(&mtr_crrnt);
          WatchDog();
        } while ( !( motorcurrent == CURRENT_OFF) && !( TimeControl( Cnt10msec, TimeOut)));

        if ( motorcurrent != CURRENT_OFF) {
          // wordt er nog iets anders aangestuurd dan alleen deze motor ?
          // nee, toch een stroom, dus een motorfout 
          // schakel de motor uit d.m.v. relais, registreer de fout 
//          PORTA &= MOTOR_OUT_BLOCKED;
          MotorGroupError();
          motorrtn_value = MOTOR_GROUP_ERROR;
        }
        goto exit;

      default:
        motorrtn_value = MOTOR_UNKNOW_ERROR;
        goto exit;
    }
  }

exit:
  return motorrtn_value;
}

