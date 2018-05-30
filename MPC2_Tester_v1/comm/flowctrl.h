#ifndef FLOWCTRL_INCLUDED
#define FLOWCTRL_INCLUDED   1

#define LONG_COW_GEN_NUMBER (EEExtMode == 1)


#include "../include/romstnet.h"
#include "../flowlib/cond_temp_vcp_flow.h"
#include "../flowlib/cow_herd_vcp_flow.h"
#include "../flowlib/activity_vcp_flow.h"
#include "../flowlib/separation_vcp_flow.h"
#include "../flowlib/disease_weight_vcp_flow.h"
#include "../flowlib/feed_vcp_flow.h"
#include "../init/start.h"
#include "../lib/boolean.h"


#include "../comm/send_msg.h"
#include "../flowlib/per_func_test_vcp_flow.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../flowlib/system_vcp_flow.h"
#include "../flowlib/transfer_vcp_flow.h"
#include "../flowlib/separation_vcp_flow.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/activity_vcp_flow.h"
#include "../flowlib/mat_robot_vcp_flow.h"
#include "../flowlib/cond_temp_vcp_flow.h"
#include "../flowlib/disease_weight_vcp_flow.h"
#include "../flowlib/test_mpc1_flow.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/cow_herd_vcp_flow.h"


// MPC instellingen, deze kunnen versie afhankelijk zijn!!
typedef struct _MPCVersInstRef {
    U2K PerGenNumber;
    U2  VersionAppl;
    U1  Address;
    U1  Address2;
    U1  MilkMeterType;
    U1  MmCorMem;
    U1  MmCorNiv1;
    U1  MmCorNiv2;
    U2  ValveTime;
    S1  FillTime;
    U1  Language;
    U2  SendTime;
    U2  NeutralTime1;
    U2  NeutralTime2;
    U2  NeutralThreshold;
    U2  TakeOffThreshold;
    S2  TakeOffTime;
    U1  MlkFlwCalcTime;
    U1  OutputBlockBits;
    U1  SepMsgCntrl;
    U1  SepActFuncNr;
    U1  SepCondFuncNr;
    U1  SepMinYldFuncNr;
    U1  SepTmpFuncNr;
    U1  ClusterDown;
    U2  ClusterDly;
    U2  VacuumDly;
    U1  MilkSweepDlyTime;
    U1  MilkSweepActTime;
    U1  I2Control;
    U1  OutputK2;
    U2  FlowLvl2Threshold;
    U1  TrcArmOnTime;
    U1  TrcArmOffTime;
    U2  TAPulsFreq;
    U1  TAPulsRatio;
    U2  CleanStartTime;
    U2  OutputP1Dly;
    U2  OutputP1Time;
    U1  OutputP1Inv;
    U2  CleanTime;
    U2  GateSwitchTime;
    U2  DispTime;
    U1  MmSensorMem;
    U1  CorFlow0;
    U1  CorFlow1;
    U1  CorFlow2;
    U1  CorFlow3;
    U1  CorFlow4;
    U1  CorFlow5;
    U1  CorFlow6;
    U1  CorFlow7;
    U1  CorFlow8;
    U1  CorFlow9;
    U1  CorFlow10;
    U1  CorFlow11;
    U1  CorFlow12;
    U1  CorFlow13;
    U2  Cal1;
    U2  Cal2;
    U2  Cal3;
    U2  Cal4;
    BL  GoatMilkGlass;
    U2  MmGoatVal1;
    U2  MmGoatVal2;
    BL  Robot;
    U1  PrgVrsIC13;
    U1  PrgVrsIC09;
    U1  PrgVrsIC08;
    U2  VersionRom;
    U1  ClnPhs1;
    U1  ClnPhs2;
    U1  ClnPhs4;
    S1  EmptyTime;
    U1  Pulsation;
    U1  PulsFrequency;
    U1  PulsRatio;
    U2  StimPulsFreq;
    U1  StimPulsRatio;
    U2  StimTresHold;
    U2  StimStartTime;
    U2  StimMaxTime;
    U1  NrOfPulsators;
    U1  Feeding;
    U2  FeedSpeed;
    U2  FeedMtrPortion;
    U2  FeedBalanceDef;
    U2  RelaisTime;
    U2  SB_FeedMtrPortion;
    U2  SB_FeedBalanceDef;
    U2  SB_RelaisTime;
    U1  Test;
    U1  ParlourType;
    U1  GateK3;
    U1  GateK4;
    U1  GateK5;
    U1  OTRowMaster;
    U1  ServiceStands;
    U1  RowSize;
    U1  ConMeasuring;
    U1  ExtConMeasuring;
    BL  TempMeasuring;
    U2  TempRValue;
    U1  KeySoft;
}MPCVersInstRef;



extern BOOLEAN CondMessageRcv;

#define MAX_MAIL_BOXES          10

// MlkRes Type -----------------------------------------------------------------
#define         NORMAL_RES           0
#define         NORMAL_RES_NO_CLEAR  1
#define         EXTEND_RES           2

#define         SEND_ALL_RESULTS               0
#define         SEND_UPDATE_ONLY               1
#define         SEND_UPDATE_CLR_MLK_AMOUNT     2


/* MlkRef Type */
#define         NORMAL_REF      0
#define         MANUAL_REF      1
#define         UPDATE_REF      2
#define         COWNULL_REF     3

/* MlkRefState */
#define         COWS_NONE       0
#define         COWS_ONE        1
#define         COWS_TWO        2

#define SEND_SEP_MSG_ACT      0x01
#define SEND_SEP_MSG_COND     0x02
#define SEND_SEP_MSG_MIN_YLD  0x04
#define SEND_SEP_MSG_TMP      0x08


extern unsigned char  ConCor1;
extern unsigned char  ConCor2;
extern unsigned char  ConCor3;
extern unsigned char  ConCor4;
extern unsigned char  CowDisease[MAX_NODES][14];
extern unsigned char  CowRefSide;

extern NETIO_MAIL *MailBox[MAX_MAIL_BOXES];

extern void        *p_CowActRef[MAX_NODES];
extern void        *p_CowConRef[MAX_NODES];
extern void        *p_CowTmpRef[MAX_NODES];
extern void        *p_CowGenRef[MAX_NODES];
extern HerdConRef  *p_HerdConRef;
extern void        *p_CowFedParlRef[MAX_NODES];
extern void        *p_CowFedParlRes[MAX_NODES];
extern void        *p_CowMlkPlsRef[MAX_NODES];

extern void        *p_CowActMemAttRef[MAX_NODES];

extern NETIO_MAIL    *p_CowSepInfoResMail[MAX_NODES];
extern NETIO_MAIL    *p_CowSepInfoMail[MAX_NODES];
extern NETIO_MAIL    *p_CowFedInfoMail[MAX_NODES];
extern NETIO_MAIL    *p_CowFedParlInfoMail[MAX_NODES];
extern NETIO_MAIL    *p_CowDiseaseMail[MAX_NODES];


extern unsigned char HndlPerPresenceResReq(NETIO_MAIL *MailPtr);
extern void FlowControl(NETIO_MAIL *MailPtr);
extern void GetCowRefs(void);
extern void SetGetCowRefs(unsigned char Side);
extern void SendCleaningBroadcast( unsigned char side, unsigned char command );


extern unsigned short CheckCowRefs(void);


extern void HndlFTBurnInRes(void);
extern void SendDefSet( void );
extern void init_flowctrl(void);
extern void HndlMlkRef(NETIO_MAIL *MailPtr);
extern void HndlCowResults(unsigned char Side);
extern void HndlCowActRef(NETIO_MAIL *MailPtr);
extern void HndlCowDiseaseRef(NETIO_MAIL *MailPtr);
extern void HndlCowTmpRef(NETIO_MAIL *MailPtr);
extern void HndlCowConRef(NETIO_MAIL *MailPtr);
extern unsigned char HndlCowExtFedParlRef(unsigned char Side);
extern void HndlPerFuncTest(NETIO_MAIL *MailPtr);
extern void HndlCowActMemAttRef(NETIO_MAIL *MailPtr) ;
extern void HndlPerMPCInstRef(NETIO_MAIL *MailPtr);
extern void HndlPerMatMlkRef(NETIO_MAIL *MailPtr);

extern void HndlPerGenRes(void);
extern void HndlMlkRes(unsigned char Type, unsigned char update, BOOLEAN);
extern unsigned char HndlCowGenRes( unsigned char Side);
extern unsigned char HndlCowFedParlRes(unsigned char Side);
extern unsigned char HndlCowDiseaseRes( unsigned char Side) ;
extern unsigned char HndlCowSepInfoRes(unsigned char Side);
extern unsigned char HndlMlkStatusUpdateRes( unsigned short amount, unsigned char state);

extern unsigned char HndlCowActRefReq(unsigned char Side);
extern unsigned char HndlCowActMemAttRefReq(unsigned char Side);
extern unsigned char HndlCowConRefReq(unsigned char Side);
extern unsigned char HndlCowDiseaseRefReq(unsigned char Side);
extern unsigned char HndlCowTmpRefReq(unsigned char Side);
extern unsigned char HndlHerdConRefReq( void );
extern unsigned char HndlPerGenNumberRefReq(signed short PerAddress);
extern unsigned char HndlPerGenRefReq(unsigned short PerNumber, signed short PerAddress);
extern unsigned char HndlMlkRefReq(unsigned long num, unsigned char Type, unsigned char idx);
extern unsigned char HndlPerMPCInstResReq( NETIO_MAIL *MailPtr);
extern unsigned char HndlPerMatMlkResReq(NETIO_MAIL *MailPtr);

#endif
