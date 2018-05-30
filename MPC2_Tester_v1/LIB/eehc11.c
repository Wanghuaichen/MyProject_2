// =============================================================================
//
// Module Name  :eehc11.c
//
// Description  :  Declaration EEProm Variables
//
// =============================================================================

#include "eehc11.h"

#pragma memory=default

unsigned char EEAddressCopy;



#pragma memory=dataseg(EEDATA)


 unsigned char  EEAddress2;
 unsigned char  EEDefault;
 unsigned short EESwitchOff;
//Pergen blok
 unsigned short EEDummy1;
 unsigned short EEMtrRow;
 unsigned short EEClxTime;
 unsigned short EEDetectPerNr;
 unsigned short EECal1;
 unsigned short EECal2;
 unsigned short EECal3;
 unsigned short EECal4;
 unsigned short EECal5;
 unsigned short EEConCor1;
 unsigned short EEConCor2;
 unsigned short EEConCor3;
 unsigned short EEConCor4;
 unsigned char  EEExtMode;
 unsigned char  EEMlkStateUpdateCntr;
//Einde pergen blok
 unsigned char  EEMmCorNiv1;
 unsigned char  EEMmCorNiv2;
 unsigned char  EEMmCorMem;
 unsigned char  EEMmSensorMem;
 unsigned short EEDispTime;
 unsigned short EEGateSwitchTime;
 unsigned short EENeutralTime1;
 unsigned short EENeutralTime2;
 unsigned short EENeutralThreshold;
 unsigned short EETakeOffThreshold;
   signed short EETakeOffTime;
 unsigned short EEClusterDly;
 unsigned short EEVacuumDly;
 unsigned short EEOutputP1Dly;
 unsigned short EEOutputP1Time;
 unsigned char  EEOutputP1Inv;
 unsigned short EEKickOffFlowProc;
 unsigned short EEKickOffSampleTime;
 unsigned short EEKickOffThreshold;
 unsigned short EESendTime;
 unsigned short EECleanTime;
 unsigned short EEValveTime;
 unsigned char  EECorFlow0;
 unsigned char  EECorFlow1;
 unsigned char  EECorFlow2;
 unsigned char  EECorFlow3;
 unsigned char  EECorFlow4;
 unsigned char  EECorFlow5;
 unsigned char  EECorFlow6;
 unsigned char  EECorFlow7;
 unsigned char  EECorFlow8;
 unsigned char  EECorFlow9;
 unsigned char  EECorFlow10;
 unsigned char  EECorFlow11;
 unsigned char  EECorFlow12;
 unsigned char  EECorFlow13;
   signed char  EEFillTime;
   signed char  EEEmptyTime;
 unsigned char  EEClnPhs1;
 unsigned char  EEClnPhs2;
 unsigned char  EEClnPhs4;
 unsigned short EESerialNr;
 unsigned char  EERobot;
 unsigned char  EETest;
 unsigned short EECleanStartTime;
 unsigned char  EEActMeasuring;
 unsigned char  EEGoatMilkGlass;
 unsigned char  EEMlkFlwCalcTime;
 unsigned char  EEMmOffsetMan;
 unsigned char  EEMmWaterTestMan;
 unsigned short EEValveTimeMan;
 unsigned char  EECorFlow0Man;
 unsigned char  EECorFlow1Man;
 unsigned char  EECorFlow2Man;
 unsigned char  EECorFlow3Man;
 unsigned char  EECorFlow4Man;
 unsigned char  EECorFlow5Man;
 unsigned char  EECorFlow6Man;
 unsigned char  EECorFlow7Man;
 unsigned char  EECorFlow8Man;
 unsigned char  EECorFlow9Man;
 unsigned char  EECorFlow10Man;
 unsigned char  EECorFlow11Man;
 unsigned char  EECorFlow12Man;
 unsigned char  EECorFlow13Man;
 unsigned short EEMmGoatVal1;
 unsigned short EEMmGoatVal2;

unsigned char   EELanguage;
unsigned short  EETempRValue;

// tbv motor/solenoid ----------------------------------------------------------
unsigned short  EERelaisTime;
unsigned char   EEMtrOutConfig;
unsigned char   EEMtrType;
unsigned char   EERelayConfig;
unsigned short  ErrorInfo;


// tbv pulsatie ----------------------------------------------------------------
unsigned char   EEPulseRatio;
unsigned char   EEPulseFrequency;
unsigned short  EEStimPulseFreq;
unsigned char   EEStimPulseRatio;
unsigned short  EEStimTresHold;
unsigned short  EEStimStartTime;
unsigned short  EEStimMaxTime;
unsigned char   EENrOfPulsators;
unsigned char   EEPulsation;

// tbv Open Tandem -------------------------------------------------------------
unsigned char   EEOTRowMaster;

unsigned char   EEI2Control;

unsigned short  EEFlowLvl2Threshold;
unsigned char   EEOutputK2;
unsigned short  EESurgeMilk;

unsigned char   EEParlourType;
unsigned char   EEOutputBlockBits;
unsigned char   EEGateK3;
unsigned char   EEGateK4;
unsigned char   EEGateK5;

unsigned char   EERowSize;
unsigned char   EEServiceStands;
unsigned char   EEMilkSweepDlyTime;
unsigned char   EEMilkSweepActTime;
unsigned char   EEExtConMeasuring;
unsigned char   EEClusterDown;

// tbv voeren
unsigned short  EEFeedSpeed;
unsigned short  EEFeedMtrPortion[MAX_NODES];
unsigned short  EEFeedBalanceDef[MAX_NODES];

unsigned short  EESB_RelaisTime;

unsigned char   EESepMsgCntrl;          // tbv automatisch separatie berichten
unsigned char   EESepActFuncNr;
unsigned char   EESepCondFuncNr;

unsigned char   EETrcArmOnTime;        // tbv 'nach milch automat'
unsigned char   EETrcArmOffTime;
unsigned short  EETAPulseFreq;
unsigned char   EETAPulseRatio;

unsigned char   EESepMinYldFuncNr;                               // tbv automatisch separatie berichten
unsigned char   EESepTmpFuncNr;
unsigned short  EEGateK3Time;

unsigned char   EEDetectLevel;
unsigned char   EEClusterDownDelayHigh;
unsigned short  EEGateK4Time;
unsigned short  EEGateK5Time;
unsigned char   EEI1Control;
#if _HARTMAN_
unsigned char   EEHartman;
#endif

  signed char  EECorVFlow;

// tbv temperatuur -------------------------------------------------------------
#pragma memory=dataseg(EEDATA2)
unsigned char   EE_U1;
unsigned char   EE_U2;


#pragma memory=default
