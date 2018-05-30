#ifndef EEHC11_INCLUDED
#define EEHC11_INCLUDED 1

#include "../init/start.h"


#define EXT_MODE  (*((unsigned char *)(&(PerGenPar.PerGenBlock14))+1))
#define MLK_UPDATE_STATE (*((unsigned char *)(&(PerGenPar.PerGenBlock14))))

//#define DDBPRESENT              0x1234
//#define PROGPRESENT             0x5678
#pragma memory=default

extern unsigned char EEAddressCopy;

#pragma memory=dataseg(EEDATA)
extern unsigned char  EEAddress2;
extern unsigned char  EEDefault;
extern unsigned short EESwitchOff;
extern unsigned short EEDummy1;
extern unsigned short EEMtrRow;
extern unsigned short EEClxTime;
extern unsigned short EEDetectPerNr;
extern unsigned short EECal1;
extern unsigned short EECal2;
extern unsigned short EECal3;
extern unsigned short EECal4;
extern unsigned short EECal5;
extern unsigned short EEConCor1;
extern unsigned short EEConCor2;
extern unsigned short EEConCor3;
extern unsigned short EEConCor4;
extern unsigned char  EEExtMode;
extern unsigned char  EEMlkStateUpdateCntr;

extern unsigned char  EEMmCorNiv1;
extern unsigned char  EEMmCorNiv2;
extern unsigned char  EEMmCorMem;
extern unsigned char  EEMmSensorMem;
extern unsigned short EEDispTime;
extern unsigned short EEGateSwitchTime;
extern unsigned short EENeutralTime1;
extern unsigned short EENeutralTime2;
extern unsigned short EENeutralThreshold;
extern unsigned short EETakeOffThreshold;
extern   signed short EETakeOffTime;
extern unsigned short EEClusterDly;
extern unsigned short EEVacuumDly;
extern unsigned short EEOutputP1Dly;
extern unsigned short EEOutputP1Time;
extern unsigned char  EEOutputP1Inv;
extern unsigned short EEKickOffFlowProc;
extern unsigned short EEKickOffSampleTime;
extern unsigned short EEKickOffThreshold;
extern unsigned short EESendTime;
extern unsigned short EECleanTime;
extern unsigned short EEValveTime;
extern unsigned short EEValveTime1;
extern unsigned short EEValveTime2;
extern unsigned short EEValveTime2;
extern unsigned short EEValveTime3;
extern unsigned short EEValveTime4;
extern unsigned short EEValveTime5;
extern unsigned short EEValveTime6;
extern unsigned short EEValveTime7;
extern unsigned short EEValveTime8;
extern unsigned short EEValveTime9;
extern unsigned short EEValveTime10;
extern unsigned short EEValveTime11;
extern unsigned short EEValveTime12;
extern unsigned short EEValveTime13;
extern unsigned char  EECorFlow0;
extern unsigned char  EECorFlow1;
extern unsigned char  EECorFlow2;
extern unsigned char  EECorFlow3;
extern unsigned char  EECorFlow4;
extern unsigned char  EECorFlow5;
extern unsigned char  EECorFlow6;
extern unsigned char  EECorFlow7;
extern unsigned char  EECorFlow8;
extern unsigned char  EECorFlow9;
extern unsigned char  EECorFlow10;
extern unsigned char  EECorFlow11;
extern unsigned char  EECorFlow12;
extern unsigned char  EECorFlow13;
extern   signed char  EEFillTime;
extern   signed char  EEEmptyTime;
extern unsigned char  EEClnPhs1;
extern unsigned char  EEClnPhs2;
extern unsigned char  EEClnPhs4;
extern unsigned short EESerialNr;
extern unsigned char  EERobot;
extern unsigned char  EETest;
extern unsigned short EECleanStartTime;
extern unsigned char  EETempMeasuring;
extern unsigned char  EEConMeasuring;
extern unsigned char  EEActMeasuring;
extern unsigned char  EEFeeding;
extern unsigned char  EEGoatMilkGlass;
extern unsigned char  EEMlkFlwCalcTime;
extern unsigned char  EEMmOffsetMan;
extern unsigned char  EEMmWaterTestMan;
extern unsigned short EEValveTimeMan;
extern unsigned char  EECorFlow0Man;
extern unsigned char  EECorFlow1Man;
extern unsigned char  EECorFlow2Man;
extern unsigned char  EECorFlow3Man;
extern unsigned char  EECorFlow4Man;
extern unsigned char  EECorFlow5Man;
extern unsigned char  EECorFlow6Man;
extern unsigned char  EECorFlow7Man;
extern unsigned char  EECorFlow8Man;
extern unsigned char  EECorFlow9Man;
extern unsigned char  EECorFlow10Man;
extern unsigned char  EECorFlow11Man;
extern unsigned char  EECorFlow12Man;
extern unsigned char  EECorFlow13Man;
extern unsigned short EEMmGoatVal1;
extern unsigned short EEMmGoatVal2;

extern unsigned char   EELanguage;
extern unsigned short  EETempRValue;
extern unsigned short  EERelaisTime;

extern unsigned char   EEMtrOutConfig;
extern unsigned char   EEMtrType;
extern unsigned char   EERelayConfig;
extern unsigned short  ErrorInfo;


extern unsigned char   EEPulseRatio;
extern unsigned char   EEPulseFrequency;
extern unsigned short  EEStimPulseFreq;
extern unsigned char   EEStimPulseRatio;
extern unsigned short  EEStimTresHold;
extern unsigned short  EEStimStartTime;
extern unsigned short  EEStimMaxTime;
extern unsigned char   EENrOfPulsators;
extern unsigned char   EEPulsation;

// tbv Open Tandem -------------------------------------------------------------
extern unsigned char   EEOTRowMaster;

extern unsigned char   EEI2Control;
extern unsigned short  EEFlowLvl2Threshold;
extern unsigned char   EEOutputK2;
extern unsigned short  EESurgeMilk;

extern unsigned char   EEParlourType;
extern unsigned char   EEOutputBlockBits;
extern unsigned char   EEGateK3;
extern unsigned char   EEGateK4;
extern unsigned char   EEGateK5;

extern unsigned char   EERowSize;
extern unsigned char   EEServiceStands;
extern unsigned char   EEMilkSweepDlyTime;
extern unsigned char   EEMilkSweepActTime;
extern unsigned char   EEExtConMeasuring;
extern unsigned char   EEClusterDown;

extern unsigned short  EEFeedSpeed;
extern unsigned short  EEFeedMtrPortion[MAX_NODES];
extern unsigned short  EEFeedBalanceDef[MAX_NODES];

extern unsigned short  EESB_RelaisTime;

extern unsigned char   EESepMsgCntrl;          // tbv automatisch separatie berichten
extern unsigned char   EESepActFuncNr;
extern unsigned char   EESepCondFuncNr;

extern unsigned char   EETrcArmOnTime;        // tbv 'nach milch automat'
extern unsigned char   EETrcArmOffTime;
extern unsigned short  EETAPulseFreq;
extern unsigned char   EETAPulseRatio;

extern unsigned char   EESepMinYldFuncNr;
extern unsigned char   EESepTmpFuncNr;
extern unsigned short  EEGateK3Time;
extern unsigned char   EEDetectLevel;
extern unsigned char   EEClusterDownDelayHigh;

extern unsigned short  EEGateK4Time;
extern unsigned short  EEGateK5Time;
extern unsigned char   EEI1Control;
#if _HARTMAN_
extern unsigned char   EEHartman;
#endif

extern   signed char  EECorVFlow;

#pragma memory=dataseg(EEDATA2)
extern unsigned char   EE_U1;
extern unsigned char   EE_U2;


#pragma memory = default

#endif

