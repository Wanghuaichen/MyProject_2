#ifndef open_tandem_vcp_flow
#define open_tandem_vcp_flow 1

#include "../flowlib/test_nums.h"
#include "../flowlib/dbmstype.h"

#define DbgMessResId (DEBUG_NMS|LOW|RES+0)
typedef struct _DbgMessRes {
    U1 DbgMessErrorNr;
    U1 DbgMlkState;
} DbgMessRes;

#define DbgMessResLId (DEBUG_NMS|LOW|RES+1)
typedef struct _DbgMessResL {
    U2 DbgMessId;
    struct {
      U2   DbgData;
    } Array[3];  /* sizeof(Array) = 2 */
} DbgMessResL;

#define DbgMessKeyResId (DEBUG_NMS|LOW|RES+2)
typedef struct _DbgMessKeyRes {
    U1 DbgKey;
    U1 DbgMlkState;
    U1 DbgToState;
    U1 DbgCowAtt;
    U1 DbgPlsState;
    U1 DbgKeyPath;
} DbgMessKeyRes;

#define DbgMessStateResId (DEBUG_NMS|LOW|RES+3)
typedef struct _DbgMessStateRes {
    U1 DbgMlkState;
    U1 DbgToState;
} DbgMessStateRes;



// REF *************************************************************************
#define OTExtPortWriteRefId (OPEN_TANDEM_NMS|LOW|REF + 0)	// 9040H

typedef struct _OTExtPortWriteRef {
    U1 OTExtPort;
    U1 OTExtPortState;
} OTExtPortWriteRef;


#define SystKeyExtSwitchRefId (OPEN_TANDEM_NMS|LOW|REF + 2)	// 9042H
typedef struct _SystKeyExtSwitchRef {
    U1 SystInternalCommand;
    U1 SystInternalCommandCheck;
} SystKeyExtSwitchRef;


#define NiveauDefSetUpdateRefId (OPEN_TANDEM_NMS|LOW|REF+3)	// 9043H

typedef struct _NiveauDefSetUpdateRef {
    unsigned char  Language;
    unsigned short SendTime;
    unsigned short NeutralTime1;
    unsigned short NeutralTime2;
    unsigned short NeutralThreshold;
    unsigned short TakeOffThreshold;
      signed short TakeOffTime;
    unsigned char  MlkFlwCalcTime;
    unsigned char  OutputBlockBits;
    unsigned char  SepMsgCntrl;
    unsigned char  SepActFuncNr;
    unsigned char  SepCondFuncNr;
    unsigned char  SepMinYldFuncNr;
    unsigned char  SepTmpFuncNr;
    unsigned char  ClusterDown;
    unsigned short ClusterDly;
    unsigned short VacuumDly;
    unsigned char  MilkSweepDlyTime;
    unsigned char  MilkSweepActTime;
    unsigned char  I2Control;
    unsigned char  OutputK2;
    unsigned short FlowLvl2Threshold;
    unsigned short OutputP1Dly;
    unsigned short OutputP1Time;
    unsigned char  OutputP1Inv;
    unsigned short CleanStartTime;
    unsigned short CleanTime;
    unsigned short GateSwitchTime;
    unsigned short DispTime;
    unsigned char  GoatMilkGlass;
    unsigned short MmGoatVal1;
    unsigned short MmGoatVal2;
    unsigned char  Pulsation;
    unsigned char  PulseFrequency;
    unsigned char  PulseRatio;
    unsigned short StimPulseFreq;
    unsigned char  StimPulseRatio;
    unsigned short StimTresHold;
    unsigned short StimStartTime;
    unsigned short StimMaxTime;
    unsigned char  NrOfPulsators;
    unsigned char  Feeding;
    unsigned short RelaisTime;
    unsigned short SB_RelaisTime;
    unsigned char  MilkMeterType;
    unsigned char  ParlourType;
    unsigned char  CentralGate;
    unsigned char  EntranceGate;
    unsigned char  ExitGate;
    unsigned char  OTRowMaster;
    unsigned char  RowSize;
    unsigned char  ConMeasuring;
    unsigned char  ExtConMeasuring;
    unsigned char  TempMeasuring;
    unsigned char  I1Control;
    }NiveauDefSetUpdateRef;

// memolac default tabel -------------------------------------------------------
#define MemolacDefSetUpdateRefId (OPEN_TANDEM_NMS|LOW|REF+4)	// 9044H

typedef struct _MemolacDefSetUpdateRef {
    unsigned short ValveTime;
      signed char  FillTime;
    unsigned char  Language;
    unsigned short SendTime;
    unsigned short NeutralTime1;
    unsigned short NeutralTime2;
    unsigned short NeutralThreshold;
    unsigned short TakeOffThreshold;
      signed short TakeOffTime;							// request SAC 15/02/02
    unsigned char  MlkFlwCalcTime;
    unsigned char  OutputBlockBits;
    unsigned char  SepMsgCntrl;
    unsigned char  SepActFuncNr;
    unsigned char  SepCondFuncNr;
    unsigned char  SepMinYldFuncNr;
    unsigned char  SepTmpFuncNr;
    unsigned char  ClusterDown;
    unsigned short ClusterDly;
    unsigned short VacuumDly;
    unsigned char  MilkSweepDlyTime;
    unsigned char  MilkSweepActTime;
    unsigned char  I2Control;
    unsigned char  OutputK2;
    unsigned short FlowLvl2Threshold;
    unsigned short CleanStartTime;
    unsigned short GateSwitchTime;
    unsigned short DispTime;
    unsigned char  MmSensorMem;
    unsigned char  ClnPhs1;
    unsigned char  ClnPhs2;
    unsigned char  ClnPhs4;                                   // ClnPhs3EE
      signed char  EmptyTime;
    unsigned char  Pulsation;
    unsigned char  PulseFrequency;
    unsigned char  PulseRatio;
    unsigned short StimPulseFreq;
    unsigned char  StimPulseRatio;
    unsigned short StimTresHold;
    unsigned short StimStartTime;
    unsigned short StimMaxTime;
    unsigned char  NrOfPulsators;
    unsigned char  Feeding;
    unsigned short RelaisTime;
    unsigned short SB_RelaisTime;
    unsigned char  MilkMeterType;
    unsigned char  ParlourType;
    unsigned char  CentralGate;
    unsigned char  EntranceGate;
    unsigned char  ExitGate;
    unsigned char  OTRowMaster;
    unsigned char  RowSize;
    unsigned char  ConMeasuring;
    unsigned char  ExtConMeasuring;
    unsigned char  TempMeasuring;
    unsigned char  I1Control;
    unsigned char  TrcArmOnTime;         // tbv 'nach milch automat'
    unsigned char  TrcArmOffTime;
    unsigned short TAPulseFreq;
    unsigned char  TAPulseRatio;  
} MemolacDefSetUpdateRef;

#define OTMasterSlaveStatusRefId (OPEN_TANDEM_NMS|LOW|REF+5)	// 9045H

typedef struct _OTMasterSlaveStatusRef {
    U4 OTM_station_state;
    U1 OTM_parl_info;
    XX dummy;
} OTMasterSlaveStatusRef;


#define OTCommandRefId (OPEN_TANDEM_NMS|LOW|REF + 6)		// 9046H
typedef struct _OTCommandRef {
    U1 OTCommand;
    U1 OTCommandDest;
    U1 OTCommandSource;
    U1 OTCommandDummy;   				//
} OTCommandRef;


#define OTMessageRefId (OPEN_TANDEM_NMS|LOW|REF + 7)		// 9047H
typedef struct _OTMessageRef {
    U1 OTMessage;
    U1 OTMessageCheck;				//
} OTMessageRef;


// vflow default tabel -------------------------------------------------------
#define VFlowDefSetUpdateRefId (OPEN_TANDEM_NMS|LOW|REF+8)	// 9044H

typedef struct _VFlowDefSetUpdateRef {
    unsigned char  Language;
    unsigned short SendTime;
    unsigned short NeutralTime1;
    unsigned short NeutralTime2;
    unsigned short NeutralThreshold;
    unsigned short TakeOffThreshold;
      signed short TakeOffTime;							// request SAC 15/02/02
    unsigned char  OutputBlockBits;
    unsigned char  SepMsgCntrl;
    unsigned char  SepActFuncNr;
    unsigned char  SepCondFuncNr;
    unsigned char  SepMinYldFuncNr;
    unsigned char  SepTmpFuncNr;
    unsigned char  ClusterDown;
    unsigned short ClusterDly;
    unsigned short VacuumDly;
    unsigned char  MilkSweepDlyTime;
    unsigned char  MilkSweepActTime;
    unsigned char  I1Control;
    unsigned char  I2Control;
    unsigned char  OutputK2;
    unsigned short FlowLvl2Threshold;
    unsigned char  TrcArmOnTime;
    unsigned char  TrcArmOffTime;
    unsigned short TAPulsFreq;
    unsigned char  TAPulsRatio;
    unsigned short CleanStartTime;
    unsigned short GateSwitchTime;
    unsigned short OutputP1Dly;
    unsigned char  OutputP1Inv;
    unsigned short DispTime;
      signed char  CorVFlow;
    unsigned char  ClnPhs1;
    unsigned char  ClnPhs2;
    unsigned char  ClnPhs4;                                   // ClnPhs3EE
    unsigned char  Pulsation;
    unsigned char  PulseFrequency;
    unsigned char  PulseRatio;
    unsigned short StimPulseFreq;
    unsigned char  StimPulseRatio;
    unsigned short StimTresHold;
    unsigned short StimStartTime;
    unsigned short StimMaxTime;
    unsigned char  NrOfPulsators;
    unsigned char  Feeding;
    unsigned short RelaisTime;
    unsigned short SB_RelaisTime;
    unsigned char  MilkMeterType;
    unsigned char  ParlourType;
    unsigned short GateK3Time;
    unsigned short GateK4Time;
    unsigned short GateK5Time;
    unsigned char  GateK3;
    unsigned char  GateK4;
    unsigned char  GateK5;
    unsigned char  OTRowMaster;
    unsigned char  RowSize;
    unsigned char  ConMeasuring;
    unsigned char  TempMeasuring;
  }VFlowDefSetUpdateRef;


// REFREQ **********************************************************************
#define OTExtPortWriteRefReqId (OPEN_TANDEM_NMS|LOW|REFREQ + 0)	// 9000H
typedef struct _OTExtPortWriteRefReq {
    U1 OTExtPort;
    U1 OTExtPortState;
} OTExtPortWriteRefReq;



#define OTStateRefReqId (OPEN_TANDEM_NMS|LOW|REFREQ + 1)	// 9001H
typedef struct _OTStateRefReq {
    S2   Empty;
} OTStateRefReq;




// RES **************************************************************************
#define OTStateResId (OPEN_TANDEM_NMS|LOW|RES + 0)		// 90C0H

typedef struct _OTStateRes {
    U4 station_state;			    // open-tandem-slave-state
    U1 ot_milk_state;	                    // is gelijk aan de melk-status
    U1 ot_trf_att;			    // is gelijk aan de GetTransferAtt
} OTStateRes;


#endif
