#ifndef MOTOR_INCLUDED

#define MOTOR_INCLUDED 1

#include "../lib/time.h"

#define OUTPUT_MOTOR            0
#define OUTPUT_RELAY            1

#define NR_OF_MOTORS            1

#define MAX_MOTOR_TURN_TIME    500              // 5 seconden
#define MIN_MOTOR_ON_TIME       70              // ~ 700 msec

#define MAX_MOTOR_CURRENT       0xb0            // ~3 Ampere     SINGLE1
#define MIN_MOTOR_CURRENT       0x05            //
#define MAX_CUR_MTR_TIME        100             // 1 seconde
#define MIN_RELAY_CURRENT       0x02            // ~10 mAmp

#define MAX_LEVEL_CNTR_NORM     178             // ~ 100 msec,  was 100
#define MAX_LEVEL_CNTR_INV        2             // ~ 1,2 msec,  was 5,6 ms  ( 2 pulsen )
//#define NR_OF_CONTACT_READS      45             // ~  7.2 msec
#define NR_OF_CONTACT_READS      4             // ~  7.2 msec

#define MOTOR_INP_LEVEL         0xd2            // 4,12 Volt
#define MIN_CONTACT_LENGTH      40              // 400 msec

#define MOTOR_CURRENT_CHANNEL   CHANNEL7
#define MOTOR_INPUT_CHANNEL     CHANNEL8

#define MOTOR_OUT_FREE            0x20
#define MOTOR_OUT_BLOCKED         (0xff - MOTOR_OUT_FREE)

#define MOTOR_CURRENT_CHANGED_TIME  80  // maximum time within motor-
                                        // current will be stabilized

enum motor_turn {
  TURN,
  FIRST_TURN,
  SECOND_TURN,
  FIRST_TURN_S2
  };
typedef unsigned char MOTOR_TURN;

enum motor_return_state {
  MOTOR_TURNED = 0,
  MOTOR_NUMBER_INVALID,
  MOTOR_CURRENT_ERROR,
  MOTOR_INPUT_CONTACT_ERROR,
  MOTOR_FAILURE,
  MOTOR_OUT_CONF_ERROR,
  MOTOR_CUR_HIGH_ERROR,
  MOTOR_CONF_ERROR,
  MOTOR_UNKNOW_ERROR,
  MOTOR_GROUP_ERROR
};
typedef unsigned char MOTOR_RETURN_STATE;

enum motor_current {
  CURRENT_OFF= 0,
  CURRENT_CHANGING,
  CURRENT_ON
};
typedef unsigned char MOTOR_CURRENT;


enum motor_state {
  MTR_CURRENT_CHECK = 20,
  MTR_INPUT_CHECK_START,
  MTR_INPUT_CHECK_STOP,
  MTR_WAIT_CURRENT_LOW
};
typedef unsigned char MOTOR_STATE;


typedef struct { unsigned char *MtrOutAdres;
                 unsigned char  MtrOutBit;
                 unsigned char *MtrInAdres;
                 unsigned char  MtrInBit;
               } MOTOR_ROM_INFO;


// local variables -------------------------------------------------------------
extern unsigned short       HalfTurnTime[NR_OF_MOTORS];
extern unsigned char        Motor_Fail_Cntr[ NR_OF_MOTORS];
extern const MOTOR_ROM_INFO Motor_Rom_Array[ NR_OF_MOTORS];


// functie prototypes ----------------------------------------------------------
void          init_motor( unsigned char MaxMotorFailCntr );
void          ResetErrorInfo(void);
void          CheckSendCR(void);
void          MotorOutEnable(void);
void          MotorResetErrors(void);
void          MotorError(unsigned char MotorNumber);
void          MotorGroupError(void);
void          TurnMotorOn(unsigned char MotorNumber);
void          TurnMotorOff(unsigned char MotorNumber);
void          OutputConfigInstall(unsigned char outputs_excluded );
void          PowerRelayOff(void);
MOTOR_CURRENT GetMotorCurrent(unsigned char *mtr_current);
MOTOR_CURRENT MotorCurrent( void );
unsigned char MotorBreakOnOff(unsigned char cmnd);
unsigned char ChkMtrInputLevel( unsigned char level);
unsigned char ChkMtrInput( unsigned char level);
unsigned char DetermineHalfMotorTurn ( unsigned char MotorNumber);
unsigned char MotorControl(unsigned char MotorNumber, unsigned char turn);
unsigned char RelaisControl( void );

#endif
