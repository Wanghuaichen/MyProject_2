#ifndef MILKMISC_INCLUDED
#define MILKMISC_INCLUDED 1

// GateState -------------------------------------------------------------------
#define         GATE_CLOSED     0
#define         GATE_OPEN       1

// StartSwitchState ------------------------------------------------------------
#define         SWITCH_CLOSED        0
#define         SWITCH_OPEN          1
#define         SWITCH_CLOSED_HOLD   2
#define         SWITCH_OPEN_HOLD     3

// SideState  ------------------------------------------------------------------
#define         SIDE_A          0
#define         SIDE_B          1

#define  GATE_COUNTER_MAX       15
#define  GATE_TOLERANCE         5

//#define  SWITCH_COUNTER_MAX     6
//#define  SWITCH_TOLERANCE       2

#define  SENSOR_STATE_OPEN        1
#define  SENSOR_STATE_CLOSE       2

extern unsigned char  SensorM1Cntr;                 // NO_MILK_METER
#define  SENSORM1_CNTR_MAX      20                  // 20 * 10 msec
#define  SENSORM1_CNTR_ON       14                  // 14 * 10 msec
#define  SENSORM1_CNTR_OFF       2                  //  2 * 10 msec

extern unsigned char  Sensor1Cntr;
#define  SENSOR1_CNTR_MAX       20                  // 20 * 10 msec
#define  SENSOR1_CNTR_ON        14                  // 14 * 10 msec
#define  SENSOR1_CNTR_OFF        2                  //  2 * 10 msec

extern unsigned char  Sensor2Cntr;
#define  SENSOR2_CNTR_MAX        9                  // 20 * 10 msec
#define  SENSOR2_CNTR_ON         6                  // 14 * 10 msec
#define  SENSOR2_CNTR_OFF        2                  //  2 * 10 msec

// I3 wordt gebruikt voor koedetectie centrale ingang (open-tandem) ------------
extern unsigned char  Sensor3Cntr;
#define  SENSOR3_CNTR_MAX       20                  // 20 * 10 msec
#define  SENSOR3_CNTR_ON        14                  // 14 * 10 msec
#define  SENSOR3_CNTR_OFF        2                  //  2 * 10 msec

// I4 wordt gebruikt voor koedetectie standplaats (open-tandem) ----------------
extern unsigned char  Sensor4Cntr;
#define  SENSOR4_CNTR_MAX       40                  // 40 * 10 msec
#define  SENSOR4_CNTR_ON        14                  // 14 * 10 msec
#define  SENSOR4_CNTR_OFF        2                  //  2 * 10 msec


#define DPL_SEP_ATT      0x0001
#define DPL_COLOST_ATT   0x0002
#define DPL_DRY_ATT      0x0004
#define DPL_ILL_ATT1     0x0008
#define DPL_ILL_ATT2     0x0010
#define DPL_ILL_ATT3     0x0020
#define DPL_ILL_ATT4     0x0040
#define DPL_ILL_ATT5     0x0080
#define DPL_COND_ATT     0x0100
#define DPL_ACT_COND_ATT 0x0200
#define DPL_CM_ERR       0x0400
#define DPL_TM_ERR       0x0800
#define DPL_TMP_ATT      0x1000
#define DPL_COMM_ATT     0x2000

#define DPL_ILL_ATT1_OFFSET 3

//extern unsigned char  SwitchCounter;
extern unsigned char  GateState, SideState, StartSwitchState,EmptyGlassAuto;

unsigned char ReadM1Sensor( void );
unsigned char ReadI1Sensor( void );
unsigned char ReadI2Sensor( void );
unsigned char ReadI3Sensor( void );
unsigned char ReadI4Sensor( void );

extern unsigned char  MlkCowAvailable(unsigned char side);
extern unsigned long GetCowNumber(unsigned char side);
extern unsigned char DifferentCowAvailable( unsigned char idx );
extern void DispMlkRefs(void);
void InitGateState(void);
extern void SetGateState(void);
extern void SetStartSwitchState(void);
extern   signed char  GetNodeIndex(signed char Node);
extern   signed char  GetSideIndex(signed char Side);
extern   signed char  GetOtherSideIndex(signed char Side);
extern   signed char  GetNodeAddress(signed char Side);
extern   signed char GetDisplaySide(void);
extern unsigned short GetCowAtt(unsigned char side);
extern unsigned short GetCowGls(void);
extern unsigned char  GetDhiaAtt(unsigned char Reset) ;
extern unsigned short GetDisplayAtt(signed char side);
extern unsigned char  GetTransferAtt(void);
extern unsigned short GetExtCondAtt( signed char side );
//extern unsigned short GetConDispAtt(signed char side);
//extern unsigned short GetTmpDispAtt(signed char side);

#endif
