#ifndef OPEN_TANDEM_INCLUDED
#define OPEN_TANDEM_INCLUDED  1

#include "../lib/boolean.h"
#include "../include/romstnet.h"

extern unsigned char OTRowMaster;
extern unsigned char OtherRowMaster;
extern unsigned char OTRowSide;

#define OT_RIGHT_SIDE	1
#define OT_LEFT_SIDE    2

#define OT_OPEN_GATE    1
#define OT_CLOSE_GATE   2

#define OT_MSG_CONFIRM_TIME  150
#define OT_MSG_CONFIRM		   1
#define OT_NO_MSG_CONFIRM      2

#define OT_ENTR_GATE             0x01
#define OT_EXIT_GATE             0x02
 
// bits open tandem slave state
#define OT_EXIT_MANUAL          0x00000001   // exit-gate manueel bediend 
#define OT_COW_PRESENT          0x00000002
#define OT_K3_GATE_STATE        0x00000004   // central-gate open     DEZE NIET WIJZIGEN,
#define OT_K4_GATE_STATE        0x00000008   // entrance-gate open    BITJES KOMEN OVEREEN MET
#define OT_K5_GATE_STATE        0x00000010   // exit-gate open        WERKELIJKE POSITIE EXTPORT
#define OT_K3_GATE_BLOCKED      0x00000020   // central-gate
#define OT_ENTR_GATE_BLOCKED    0x00000040   // entrance-gate
#define OT_EXIT_GATE_BLOCKED    0x00000080   // exit-gate
#define OT_WAIT_COW_MILKING     0x00000100   // wacht met open uitgangshek tot koe gemolken is
#define OT_EMERGENCY_STOP       0x00000200   // ingang open gezet om een koe te stoppen

// bits open tandem station state:Master
#define OT_K3_MANUAL            0x02000000   // central-gate manueel bediend (wel automatic mode)
#define OT_K3_MANUAL_CONTROL    0x04000000   // K3 wordt manueel bediend, cow traffic stopped
#define OTM_COW_TRAFFIC_STOPPED 0x08000000   // koeverkeer aangehouden
#define OTM_MASTER_START        0x10000000   // geef vul richting aan: 0=S->M, 1=S<-M
#define OTM_COW_TRAFFIC_INIT    0x20000000   // initieele start van koe verkeer   
#define OTM_POWER_UP            0x40000000   
#define OTM_STATION_STATE_RCV   0x80000000   // wacht op status station

#define OTSS_SLAVE_MASK         0x00FFFFFF
#define OTMS_MASTER_MASK        0xFF000000
 
// define's tbv koe detectie uitgang 
#define OT_STALL_FREE            1
#define OT_COW_EXPECT            2
#define OT_COW_SEEN              3


typedef struct {
  unsigned long  station_state;      // open-tandem-slave-state
  unsigned long  station_state_old;        
  unsigned char  ot_milk_state;      // is gelijk aan de melk-status
  unsigned char  ot_trf_att;
  unsigned char  otm_prl_info;
  unsigned char  req_send;
  unsigned short req_send_time;
} MPC_OTS_INFO; 

typedef struct {
  unsigned char  gate_to_wait_for;   // geeft aan op welke stand de koe verwacht wordt     
  unsigned char  leave_way_empty;    // geeft aan of de gang leeg is    
} MPC_OTM_PARLOUR_INFO;

typedef struct {
  unsigned long  station_state;      // open-tandem-slave-state
  unsigned char  ot_milk_state;      // is gelijk aan de melk-status
  unsigned char  ot_trf_att;
  unsigned short msg_time;           // binnenkomst tijd bericht
  unsigned char  ot_fill_direction;  // geeft aan van welke kant de stal gevuld wordt
} MPC_OTM_SLAVE_INFO; 

// OTCommand's: berichten die naar de Master of via de Master naar een andere MPC moeten
enum {
  OT_OPEN_EXIT_GATE = 1,
  OT_OPEN_ENTRANCE_GATE,
  OT_COW_TRAFFIC_INIT,
  OT_COW_TRAFFIC_STOP,
  OT_COW_TRAFFIC_START,
  OT_MSG_EXIT_GATE_BLOCKED,
  OT_MSG_ENTR_GATE_BLOCKED,
  OT_MSG_EXIT_OPEN,
  OT_MSG_ENTR_OPEN,
  OT_K3_SET_MANUAL,						// 10
  OT_K3_CLEAR_MANUAL,
  OT_TOGGLE_K3,
  OT_MSG_NO_ENTR_OPEN,
  OT_SET_COW_EXPECTED
//  OT_OPEN_CENTRAL_GATE,
//  OT_CLOSE_CENTRAL_GATE,
//  OT_CENTRAL_GATE_OPENED,
//  OT_CENTRAL_GATE_CLOSED
  };
typedef unsigned char OT_COMMANDS;

enum {
  ATM_CLEANING = 0,
  ATM_INIT,
  ATM_CHECK_MPC_STATES,
  ATM_WAIT_FOR_START,
  ATM_WAIT_LEAVE_WAY_EMPTY,
  ATM_WAIT_ENTRANCE_GATE_CLOSED,
  ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE,
  ATM_WAIT_FOR_EXIT_GATE_CLOSED
  };
typedef unsigned char AT_MASTER_STATE;
extern AT_MASTER_STATE      ATMasterState;


extern MPC_OTS_INFO         MpcOtsInfo;
extern MPC_OTM_SLAVE_INFO   MpcOtmSlaveInfo[10];
extern MPC_OTM_PARLOUR_INFO MpcOtmParlourInfo;
extern unsigned short       OpenTandemStartTime;

//extern non_banked void HndlTandem_Non_Banked(void); 


// function prototypes ---------------------------------------------------------
unsigned char OTGateStateBlocked(void);
unsigned char OTCowInBox( void);
unsigned char OTCowInSystem( void);
unsigned char ParlourTandemTypeAndMilkingMode(void); 

void HandleKeyOpenTandem( unsigned char key, unsigned char key_hold );
void OTSndMsgToSlave(  unsigned char dest, unsigned char message );
void OTSndMsgToMaster( unsigned char command, unsigned char dest, unsigned char source );
void HndlRcvMasterState( NETIO_MAIL *MailPtr );
void HndlRcvOTCommandRef(NETIO_MAIL *MailPtr);
void OT_CheckCentralGateSensor( void );
void HndlRcvOTSStateRes(NETIO_MAIL *MailPtr);
void HndlSendOTSStateRes(void);
void OTStartFromCleaning( void);
void InitOpenTandem( void );
void HndlTandem(void);

#endif 
