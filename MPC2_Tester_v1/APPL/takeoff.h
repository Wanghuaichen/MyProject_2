#ifndef TAKEOFF_INCLUDED
#define TAKEOFF_INCLUDED        1



// TakeOffStates ---------------------------------------------------------------
#define         TO_INIT              0
#define         TO_IDLE              1
#define         TO_MAN               2
#define         TO_AUTO              3
#define         TO_START             4  
#define         TO_RELEASE_MAN       5
#define         TO_RELEASE_AUTO      6
#define         TO_FIRST_START       7
#define         TO_START_CLEAN       8
#define         TO_CLEANING          9
#define         TO_CLEANING_END     10
#define         TO_START_UNICO      11  
#define         TO_INIT_INIT        20


// ClusterState ----------------------------------------------------------------
#define         CLUSTER_IDLE         0
#define         CLUSTER_HIGH         1
#define         CLUSTER_LOW          2
#define         CLUSTER_HIGH_LOW     3
#define         CLUSTER_LOW_HIGH     4
#define         CLUSTER_DOWN_LOW     5
#define         CLUSTER_DOWN_HIGH    6
#define         CLUSTER_SET_DOWN_HIGH 7
#define         CLUSTER_DELAY_DOWN_HIGH 8
 

// VacuumState  ----------------------------------------------------------------
#define         VACUUM_IDLE                 0
#define         VACUUM_ON                   1
#define         VACUUM_OFF                  2
#define         VACUUM_ON_OFF               3
#define         VACUUM_OFF_ON               4
#define         VACUUM_MLK_SWEEP_DLY_TIME   5
#define         VACUUM_MLK_SWEEP_ACT_TIME   6


// ExtOutputP1State ------------------------------------------------------------
#define         P1_IDLE              0
#define         P1_ON                1
#define         P1_OFF               2
#define         P1_ON_OFF            3
#define         P1_OFF_ON            4
#define         P1_START_MAN         5
#define         P1_CLEAN_END         6

// MilkAutoMatState  -----------------------------------------------------------
#define         MILK_AUTOMAT_IDLE    0
#define         MILK_AUTOMAT_START   1
#define         MILK_AUTOMAT_ON      2
#define         MILK_AUTOMAT_OFF     3
#define         MILK_AUTOMAT_ON_OFF  4
#define         MILK_AUTOMAT_OFF_ON  5


#define         UNICO_IDLE           0
#define         UNICO_OFF            1
#define         UNICO_ON             2
#define         UNICO_ATTACH_AUTO    3
#define         UNICO_ATTACH_MAN     4
#define         UNICO_DETACH         5
#define         UNICO_ON_OFF         6
#define         UNICO_ON_OFF_MAN     7  

// Output K2 control -----------------------------------------------------------
#define         VACUUM_CONTROL        0
#define         MILK_AUTOMAT_CONTROL  1
#define         UNICO_CONTROL         2     
#define         VACUUM_CONTROL_INVERS 3

#define		LAMP_OFF	0	
#define     LAMP_ON		1
#define		LAMP_FLASH	2
#define     LAMP_TOGGLE 3 

extern unsigned char  UnicoState, TakeOffState, VacuumState, ClusterState, ExtOutputP1State,MilkAutomatState;

extern void LampControl( unsigned char command, unsigned short time );
extern void HndlTakeOff(void);
extern void InitTO(void);
extern void PortP1On(void);
extern void PortP1Off(void);
extern void PortP2On(void);
extern void PortP2Off(void);
extern void PortP3On(void);
extern void PortP3Off(void);

#endif
