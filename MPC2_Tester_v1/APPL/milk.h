#ifndef MILK_INCLUDED
#define MILK_INCLUDED 1

#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/mat_robot_vcp_flow.h"
#include "../lib/boolean.h"
#include "../init/start.h"

#define         MLK_AMOUNT_PROFILE_TABLE_INDEX_MAX      144   // 12 minuten

#define         MLK_TBL_MAX       60
#define         MLK_TBL_MAX_GOAT  15

typedef enum {
	DBG_VFLOW_WEIGHT_RES_ID = 1
} FNC_MESSAGE_ID;

//#define FNC_ERROR_NMS unsigned short
typedef enum {
	FNC_INIT = 1,
	FNC_SWING_OVER,
	FNC_MLK_INIT,
	FNC_MLK_DONE,
	FNC_COWS_ONE_TO_ONE,
	FNC_COWS_NONE_TO_ONE,
	FNC_COWS_ONE_TO_NONE,
  FNC_COWS_ONO_TO_TWO,
	FNC_COWS_TWO_TO_NONE,
	FNC_COWS_TWO_TO_ONE,
	FNC_COWS_TWO_TO_TWO,
	FNC_NORMAL_RES,
	K3_GATE,
	K4_GATE,
	K5_GATE,
  // not used anymore
	RPT_KEY_MANUAL_REL_CLUSTER,
  RPT_KEY_SHIFT_MANUAL_REL_CLUSTER,
  RPT_KEY_SHIFT_2_MANUAL_REL_CLUSTER,
  RPT_KEY_CLUSTER_REMOVAL,
  RPT_KEY_SHIFT_CLUSTER_REMOVAL,
  RPT_KEY_AUTO_RELEASE,
  RPT_KEY_AUTO_RELEASE_FIRST_ATTACH,
  RPT_KEY_SHIFT_AUTO_RELEASE,
  RPT_KEY_SHIFT_2_AUTO_RELEASE,
  RPT_KEY_MANUAL_REL_CLUSTER_HOLD,
  RPT_KEY_AUTO_RELEASE_HOLD,
  RPT_KEY_AUTO_RELEASE_FIRST_ATTACH_HOLD,
  FNC_COMM_LOSS,
  // *************
	FNC_SWING_GATE                 // actual swing of gate
} FNC_ERROR_NMS;

/* Milkmeter Types              */
#define         MMT_NIVEAU      	      40
#define         MMT_SURGE       	      41
#define         MMT_MEMOLAC     	      42
#define         MMT_AFIKIM      	      43
#define         MMT_UNIVERSAL   	      44
#define         MMT_MANUFLOW          	45
#define         MMT_ORION       	      46
#define         MMT_TAKE_OFF_SENSOR     47
#define         MMT_VFLOW               48
#define         MMT_VP4001              49
#define         MMT_NO_MILK_METER      100

/* MlkStates */
#define         MLK_INIT                 0
#define         MLK_IDLE                 1
#define         MLK_NEUTRAL              2
#define         MLK_MEASURING            3
#define         MLK_TAKE_OFF_WAIT        4
#define         MLK_TAKE_OFF             5
#define         MLK_SEND_WAIT            6
#define         MLK_SEND                 7
#define         MLK_DONE                 8
#define         MLK_CONTINUE             9

#define         MLK_TAKE_OFF_MAN        10
#define         MLK_WAIT_FOR_START      11
#define         MLK_KICKED_OFF          12
#define         MLK_WAIT_FOR_SEND       13
#define         MLK_SEND_MAN            14
#define         MLK_WAIT_FOR_MLKREF     15

#define         MLK_START_CLEANING      20
#define         MLK_CLEANING            21
#define         MLK_RESTART_CLEANING    22
#define         MLK_CLEANING_END        23
#define         MLK_CLEANING_END_I2     24
#define         MLK_SERVICE            101
#define         MLK_CAL                102
#define         MLK_DHIA               103
#define         MLK_TEST               104

//Milk Mode
#define         MM_INIT            0
#define         MM_MEAS            1
#define         MM_CAL             2
#define         MM_DHIA            3
#define         MM_CLEANING        4
#define         MM_SERVICE         5

//Cleaning Phases
#define         CLEAN_PHASE_START								 0
#define         CLEAN_PHASE_START_1							 1
#define         CLEAN_PHASE_START_2   					 2
#define         CLEAN_PHASE_0       						 3
#define         CLEAN_PHASE_1         					 4
#define         CLEAN_PHASE_2         					 5
#define         CLEAN_PHASE_3         					 6
#define         CLEAN_PHASE_END       					 7
#define         CLEAN_PHASE_PRE_CLEAN						 8
#define					CLEAN_PHASE_I2_CONTROLLED_START	 9
#define					CLEAN_PHASE_I2_CONTROLLED_0			10
#define					CLEAN_PHASE_I2_CONTROLLED_1			11
#define					CLEAN_PHASE_I2_CONTROLLED				12
#define					CLEAN_PHASE_I2_CONTROLLED_END		13

// Robot status
#define         RBT_MLK_START      0
#define         RBT_MLK_END        1
#define         RBT_MLK_FAIL       2
#define         RBT_MLK_IDLE       3
#define         RBT_MLK_CLEANING   4

/*Milkmeter ValveState*/
#define         VALVE_OFF          0
#define         VALVE_ON           1
#define         VALVE_START        2
#define         VALVE_RELEASE      3
#define         VALVE_CONT_ON      4
#define         VALVE_SHORT_REL    5
#define         VALVE_DELAY        6

#define         VALVE_ON_EXTRA_DUMP           7
#define         VALVE_START_EXTRA_DUMP        8
#define         VALVE_SHORT_REL_EXTRA_DUMP    9
#define         VALVE_DELAY_EXTRA_DUMP       10


// Transfer Attenties ----------------------------------------------------------
#define         TA_YIELD        0x01
#define         TA_MINYIELD     0x01
#define         TA_SEP          0x02
#define         TA_COLOST       0x02
#define         TA_NOCOW        0x04
#define         TA_ACT_COND     0x08
#define         TA_COND         0x00    // not used
#define         TA_DRY          0x00    // not used
#define         TA_HEAT         0x00    // not used
#define         TA_TEMP         0x00    // not used


// CowAttState -----------------------------------------------------------------
#define         CA_SEP          0x0001
#define         CA_COLOST       0x0002
#define         CA_DRY          0x0004
#define         CA_HEAT         0x0008
#define         CA_COND         0x0010
#define         CA_ACT_COND     0x0020
#define         CA_TEMP         0x0040
#define         CA_NOCOW        0x0080
#define         CA_DRY_BLCK     0x0100

// CowGlsState -----------------------------------------------------------------
#define         CG_SEP          0x0001
#define         CG_COLOST       0x0002
#define         CG_YIELD        0x0004
#define         CG_MINYIELD     0x0008
#define         CG_EMPTY        0x0010
#define         CG_EXPYIELD     0x0020
#define         CG_NOCOW        0x0080

// MeasState -------------------------------------------------------------------
#define         MEAS_IDLE       0
#define         MEAS_MEAS       1
#define         MEAS_DONE       2

// StartMilkAutomat
#define MA_MANUAL_STARTED       3
#define MA_MANUAL_START         2
#define MA_AUTO_START           1

typedef struct _MlkTbl {
 unsigned long  MlkTblYield;
 unsigned short MlkTblTime;
} MlkTbl;

extern unsigned char  FlowState;
extern unsigned char  MlkState, MlkSepStandAlone, MlkMode;
extern unsigned char  RobotState;
extern unsigned long  MlkMaxFlow, MlkAmount, SendMlkAmount, StoredMlkAmount, MlkAmountRobot, MlkActFlow;
extern unsigned long  Cnt1secLast, ul_CleanStartTime;
extern unsigned short MlkDurationCur, MlkAmountKg, MlkResOtherSide;
extern unsigned short MlkDuration, MlkSendTime, SwingTime;
extern unsigned char  MilkMeterType, ValveState;
extern unsigned short ValveStartTime, GateK3ActiveTime, GateK4ActiveTime, GateK5ActiveTime;
extern unsigned char  MilkManualInput;
extern unsigned char  CleanState, CleanPhase;
extern unsigned char  MlkAttention;
extern unsigned char  MlkUpdateCntr,StartMilkAutomat;
extern unsigned char  swing_init,FirstAttach;

extern unsigned char  CowMlkYldNrOfAttach2min;     // aantal keren opnieuw onderhangen gedurende de eerste 2 minuten
extern unsigned char  CowMlkYldNrOfAttach;         // totaal aantal keren opnieuw onderhangen
extern unsigned char  CowMlkYldManDetach;          // manual
extern unsigned long  CowMlkYldStartTime[MAX_NODES];
extern unsigned char  CowMlkYldManInput[MAX_NODES];
extern unsigned long  CowMlkYldStartTime_B[MAX_NODES];
extern unsigned char  CowMlkYldManInput_B[MAX_NODES];

extern unsigned char  MlkAmountProfileTable[MLK_AMOUNT_PROFILE_TABLE_INDEX_MAX];
extern unsigned char  MlkAmountProfileTableIndex;

extern void *p_MlkRef[MAX_NODES];
extern void *p_MlkRef_B[MAX_NODES];
extern unsigned char  MlkRefState[MAX_NODES];
extern unsigned char  MeasState[MAX_NODES];
extern PerMatMlkRef   PerRbtMlkRef;
extern unsigned short CowMlkYldAmount00_15, CowMlkYldAmount15_30, CowMlkYldAmount30_60, CowMlkYldAmount60_120;
extern unsigned long  CowMilked[MAX_NODES];
extern BOOLEAN        MlkSessionAvailable[MAX_NODES];

extern void FreeMlkRef(signed char side, unsigned char call_id);
extern void FreeMlkRefB(signed char side);
extern void UpdateAndSendMlkState(unsigned long MlkAmount, unsigned char MlkState);
extern void PutMlkAmountInTable(unsigned char yield );
extern void PutMlkTbl(unsigned long Yield, unsigned short Time);
extern void AddMlkTbl(unsigned long Yield, unsigned short Time);
extern void MainMilking(void);
extern void InitMilk(void);
extern void InitMlk(void);
extern non_banked void MainMilking_Non_Banked(void);
extern interrupt void MilkInputM1(void);
extern interrupt void MilkInputM2(void);
extern interrupt void MilkInputM3(void);
#endif
