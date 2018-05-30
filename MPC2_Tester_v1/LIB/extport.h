#ifndef EXTPORT_INCLUDED
#define EXTPORT_INCLUDED         1

#define PT_HERRING_BONE                1
#define PT_OPEN_TANDEM                 2
#define PT_CARROUSEL                   3
#define PT_SEMI_TANDEM                 4
#define PT_AUTO_TANDEM                 5
#define PT_AUTO_TANDEM_NO_CD_SENSOR    6


#define SK_VC4_ENABLE                 0x01
#define SK_STIM_TRCK_ARM_ENABLE       0x02
#define SK_GATE_CONTROL_ENABLE        0x04
#define SK_FEEDING_ENABLE             0x08
#define SK_COND_TMP_ENABLE            0x10
#define SK_TANDEM_ENABLE              0x20
#define SK_FLW_CNTRL_PLS_ENABLE       0x40


#define SK_CONDUCT_ENABLE       0x01
#define SK_AUTO_TANDEM_ENABLE   0x02
#if _DEBUG_
#define SK_PULS_CONTROL_ENABLE  0x01
#else
#define SK_PULS_CONTROL_ENABLE  0x04
#endif


enum ExtMessage_type {
  EMK3_STATE = 0,
  EMK4_STATE,
  EMK5_STATE,
  EMMS_ADDRESS
};
typedef unsigned char EXT_MESSAGE_TYPE;

enum EPK5State_type {
  EPK5S_IDLE = 0,
  EPK5S_ON,
  EPK5S_OFF,
  EPK5S_TOGGLE,
  EPK5S_ON_OFF,
  EPK5S_OFF_ON
};
typedef unsigned char EPK5STATE_TYPE;

enum EPK4State_type {
  EPK4S_IDLE = 0,
  EPK4S_ON,
  EPK4S_OFF,
  EPK4S_TOGGLE,
  EPK4S_ON_OFF,
  EPK4S_OFF_ON
};
typedef unsigned char EPK4STATE_TYPE;

enum EPK3State_type {
  EPK3S_IDLE = 0,
  EPK3S_ON,
  EPK3S_OFF,
  EPK3S_TOGGLE,
  EPK3S_ON_OFF,
  EPK3S_OFF_ON,
  EPK3S_WAIT_I3_FREE,
  EPK3S_WAIT_IDLE,
  EPK3S_CLOSE_DELAYED
};
typedef unsigned char EPK3STATE_TYPE;

#define OT_EXT_PORT_WRITE_K5  5
#define OT_EXT_PORT_WRITE_K4  4
#define OT_EXT_PORT_WRITE_K3  3

//   External Port  definitions ------------------------------------------------
//#define P1    0x04            // Port output 2
#define K1    0x01            // Port output 0
#define K2    0x02            // Port output 1
#define K3    0x04            // Port output 2
#define K4    0x08            // Port output 3
#define K5    0x10            // Port output 4

#define TP1   0x10            // Port output TP1
#define TP2   0x20            // Port output TP2
#define TP3   0x40            // Port output TP3
#define TP4   0x80            // Port output TP4

#define OPEN_TANDEM_MASTER 100

extern unsigned char ParlourType;
extern unsigned char SoftKey;
extern unsigned char SoftwareKey;

extern unsigned char IC13PrgVrs;
extern unsigned char IC09PrgVrs;
extern unsigned char IC08PrgVrs;
extern unsigned char ExtPortK5State;
extern unsigned char ExtPortK4State;
extern unsigned char ExtPortK3State;
extern unsigned char ExtPortCopy;
extern unsigned char ActExtPortCopy;

extern unsigned char PwmMessage( unsigned char slen, unsigned char *sbuf,
                                 unsigned char rlen, unsigned char *rbuf );

int HndlSndMsgToMasterSlave(unsigned char dest, unsigned short msg_id,
                            unsigned char length, unsigned char *message);

extern void CheckSChannelForData(void);
extern void SetSChannel(void);
extern void RequestProgVersionsPIC(void );
extern void HndlExtPort(void);

#endif
