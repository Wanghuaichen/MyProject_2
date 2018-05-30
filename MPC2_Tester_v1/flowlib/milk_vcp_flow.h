
#ifndef milk_vcp_flow

#define milk_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define MlkStandRespResId (MILK|LOW|RES + 0)
typedef struct _MlkStandRespRes {
        U2K  PerGenNumber;
        U1   MlkStandNumber;
        XX   Dummy;
        struct {
                U2K  CowGenRespNumber;
        } Array[1];     /* sizeof(Array) = 2 */
} MlkStandRespRes;

/* sizeof(MlkStandRespRes) = 6 */

#define MlkRefReqId (MILK|LOW|REFREQ + 0)
typedef struct _MlkRefReq {
        U2K  PerGenNumber;
        U2K  CowGenNumber;
} MlkRefReq;

/* sizeof(MlkRefReq) = 4 */

#define ExtMlkRefReqId (MILK|LOW|REFREQ + 8)
typedef struct _ExtMlkRefReq {
        U2K  PerGenNumber;
        U4K  CowGenNumber;
} ExtMlkRefReq;

/* sizeof(ExtMlkRefReq) = 6 */

#define MlkRefId (MILK|LOW|REF + 0)
typedef struct _MlkRef {
        U2K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} MlkRef;

/* sizeof(MlkRef) = 22 */

#define ExtMlkRefId (MILK|LOW|REF + 8)
typedef struct _ExtMlkRef {
        U4K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtMlkRef;

/* sizeof(ExtMlkRef) = 24 */

#define MlkManualRefReqId (MILK|LOW|REFREQ + 1)
typedef struct _MlkManualRefReq {
        U2K  PerGenNumber;
        U2K  CowGenNumber;
} MlkManualRefReq;

/* sizeof(MlkManualRefReq) = 4 */

#define ExtMlkManualRefReqId (MILK|LOW|REFREQ + 9)
typedef struct _ExtMlkManualRefReq {
        U2K  PerGenNumber;
        U4K  CowGenNumber;
} ExtMlkManualRefReq;

/* sizeof(ExtMlkManualRefReq) = 4 */

#define MlkManualRefId (MILK|LOW|REF + 1)
typedef struct _MlkManualRef {
        U2K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} MlkManualRef;

/* sizeof(MlkManualRef) = 22 */

#define ExtMlkManualRefId (MILK|LOW|REF + 9)
typedef struct _ExtMlkManualRef {
        U4K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtMlkManualRef;

/* sizeof(ExtMlkManualRef) = 24 */

#define MlkResId (MILK|LOW|RES + 1)
typedef struct _MlkRes {
        U2K  PerGenNumber;
        U2K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U1   CowMlkYldManual;
        BL   CowMlkYldAttention;
        U2   CowMlkYldAmount;
        U2   CowMlkYldMaxSpeed;
        U2   CowMlkYldDuration;
        BL   CowGenManualRelease;
        XX   Dummy;
} MlkRes;

/* sizeof(MlkRes) = 22 */

#define ExtMlkResId (MILK|LOW|RES + 10)
typedef struct _ExtMlkRes {
        U2K  PerGenNumber;
        U4K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U1   CowMlkYldManual;
        BL   CowMlkYldAttention;
        U2   CowMlkYldAmount;
        U2   CowMlkYldMaxSpeed;
        U2   CowMlkYldDuration;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtMlkRes;

/* sizeof(ExtMlkRes) = 22 */

#define MlkStandRespUpdateResId (MILK|LOW|RES + 2)
typedef struct _MlkStandRespUpdateRes {
        U2K  PerGenNumber;
        U1   MlkStandNumber;
        XX   Dummy;
        struct {
                U2K  CowGenRespNumber;
        } Array[1];     /* sizeof(Array) = 2 */
} MlkStandRespUpdateRes;

/* sizeof(MlkStandRespUpdateRes) = 6 */

#define MlkUpdateRefId (MILK|LOW|REF + 2)
typedef struct _MlkUpdateRef {
        U2K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} MlkUpdateRef;

/* sizeof(MlkUpdateRef) = 24 */

#define ExtMlkUpdateRefId (MILK|LOW|REF + 10)
typedef struct _ExtMlkUpdateRef {
        U4K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        DT   CowGenCalvingDate;
        DT   CowGenExpCalvingDate;
        BL   CowGenColost;
        BL   CowGenHeatAttent;
        BL   CowGenInsemAttent;
        BL   CowGenDryOffAttent;
        BL   CowGenIncalfAttent;
        BL   HerdMlkEmptyGlassAuto;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U2   CowMlkExpMinAmount;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtMlkUpdateRef;

/* sizeof(ExtMlkUpdateRef) = 26 */

#define MlkStandRespClearResId (MILK|LOW|RES + 3)
typedef struct _MlkStandRespClearRes {
        U2K  PerGenNumber;
  struct {
    U2   MlkFirstStand;
    U2   MlkNrOfStands;
  } Array[1];  /* sizeof(Array) = 4 */
} MlkStandRespClearRes;

/* sizeof(MlkStandRespClearRes) = 6 */

#define ExtendedMlkResId (MILK|LOW|RES + 4)
typedef struct _ExtendedMlkRes {
        U2K  PerGenNumber;
        U2K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U1   CowMlkYldManual;
        BL   CowMlkYldAttention;
        U2   CowMlkYldAmount;
        U2   CowMlkYldMaxSpeed;
        U2   CowMlkYldDuration;
        U1   CowMlkYldDeadMlkTime1;
        U1   CowMlkYldDeadMlkTime2;
        U1   CowMlkYldDeadMlkTime3;
        U1   CowMlkYldDeadMlkTime4;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtendedMlkRes;

/* sizeof(ExtendedMlkRes) = 26 */

#define ExtExtendedMlkResId (MILK|LOW|RES + 11)
typedef struct _ExtExtendedMlkRes {
        U2K  PerGenNumber;
        U4K  CowGenNumber;
        DT   CowGenHeatDate;
        DT   CowGenInsemDate;
        DT   CowGenDryOffDate;
        U1   CowGenReproState;
        U1   CowMlkNrOfSeps;
        U1   CowMlkYldManual;
        BL   CowMlkYldAttention;
        U2   CowMlkYldAmount;
        U2   CowMlkYldMaxSpeed;
        U2   CowMlkYldDuration;
        U1   CowMlkYldDeadMlkTime1;
        U1   CowMlkYldDeadMlkTime2;
        U1   CowMlkYldDeadMlkTime3;
        U1   CowMlkYldDeadMlkTime4;
        BL   CowGenManualRelease;
        XX   Dummy;
} ExtExtendedMlkRes;

/* sizeof(ExtExtendedMlkRes) = 24 */

#define MlkStandRespActResId (MILK|LOW|RES + 5)
typedef struct _MlkStandRespActRes {
        U2K  PerGenNumber;
        U1   MlkStandNumber;
        XX   Dummy;
        struct {
                U2K  CowGenRespNumber;
                U2   CowActCounterValue;
        } Array[1];     /* sizeof(Array) = 4 */
} MlkStandRespActRes;

/* sizeof(MlkStandRespActRes) = 8 */

#define KbdCowMlkRefReqId (MILK|LOW|REFREQ + 6)
typedef struct _KbdCowMlkRefReq {
        U2   KbdCommand;
        U2K  CowMlkCowId;
} KbdCowMlkRefReq;

/* sizeof(KbdCowMlkRefReq) = 4 */

#define KbdCowMlkRefId (MILK|LOW|REF + 6)
typedef struct _KbdCowMlkRef {
        U2K  CowMlkCowId;
        U2   CowMlkMinExpYld;
      DT   CowMlkSepDate;
        U1   CowMlkNrOfSeps;
        XX   Dummy;
        struct {
                DT   CowMlkYldDate;
                TM   CowMlkYldTime;
                U2   CowMlkYldAmount;
                U2   CowMlkYldMaxSpeed;
                U2   CowMlkYldDuration;
                U1   CowMlkYldManual;
                BL   CowMlkYldAttention;
                U2   CowMlkYldPerNumber;
                U1   CowMlkYldDeadMlkTime1;
                U1   CowMlkYldDeadMlkTime2;
                U1   CowMlkYldDeadMlkTime3;
                U1   CowMlkYldDeadMlkTime4;
        } Array[1];     /* sizeof(Array) = 18 */
} KbdCowMlkRef;

/* sizeof(KbdCowMlkRef) = 26 */

#define KbdCowMlkResId (MILK|LOW|RES + 6)
typedef struct _KbdCowMlkRes {
        U2   KbdCommand;
        U2K  CowMlkCowId;
        DT   CowMlkSepDate;
        U1   CowMlkNrOfSeps;
        XX   Dummy;
} KbdCowMlkRes;

/* sizeof(KbdCowMlkRes) = 8 */

#define CowMlkPlsRefReqId (MILK|LOW|REFREQ + 7)
typedef struct _CowMlkPlsRefReq {
  U2K  CowGenNumber;
} CowMlkPlsRefReq;

/* sizeof(CowMlkPlsRefReq) = 2 */

#define ExtCowMlkPlsRefReqId (MILK|LOW|REFREQ + 10)
typedef struct _ExtCowMlkPlsRefReq {
  U4K  CowGenNumber;
} ExtCowMlkPlsRefReq;

/* sizeof(ExtCowMlkPlsRefReq) = 4 */

#define CowMlkPlsRefId (MILK|LOW|REF + 7)
typedef struct _CowMlkPlsRef {
  U2K  CowGenNumber;
  U2   CowMlkPlsSetting;
  U2   CowMlkPlsTakeOffThresHold;
  U2   CowMlkPlsTakeOffTime;
  U2   CowMlkPlsStmSrtTime;
  U2   CowMlkPlsStmMaxTime;
  U2   CowMlkPlsStmTresHold;
  U2   CowMlkPlsStmFrqFront;
  U2   CowMlkPlsStmFrqBack;
  U1   CowMlkPlsStmRatFront;
  U1   CowMlkPlsStmRatBack;
  U2   CowMlkPlsFrqFront;
  U2   CowMlkPlsFrqBack;
  U1   CowMlkPlsRatFront;
  U1   CowMlkPlsRatBack;
  U2   CowMlkPlsAvgSpeed;
  U1   CowMlkPlsProp01;
  U1   CowMlkPlsProp02;
  U1   CowMlkPlsProp03;
  U1   CowMlkPlsProp04;
  U1   CowMlkPlsProp05;
  U1   CowMlkPlsProp06;
  U1   CowMlkPlsProp07;
  U1   CowMlkPlsProp08;
  U1   CowMlkPlsProp09;
  U1   CowMlkPlsProp10;
  S1   CowMlkPlsRatCorr01;
  S1   CowMlkPlsRatCorr02;
  S1   CowMlkPlsRatCorr03;
  S1   CowMlkPlsRatCorr04;
  S1   CowMlkPlsRatCorr05;
  S1   CowMlkPlsRatCorr06;
  S1   CowMlkPlsRatCorr07;
  S1   CowMlkPlsRatCorr08;
  S1   CowMlkPlsRatCorr09;
  S1   CowMlkPlsRatCorr10;
  S1   CowMlkPlsFrqCorr01;
  S1   CowMlkPlsFrqCorr02;
  S1   CowMlkPlsFrqCorr03;
  S1   CowMlkPlsFrqCorr04;
  S1   CowMlkPlsFrqCorr05;
  S1   CowMlkPlsFrqCorr06;
  S1   CowMlkPlsFrqCorr07;
  S1   CowMlkPlsFrqCorr08;
  S1   CowMlkPlsFrqCorr09;
  S1   CowMlkPlsFrqCorr10;
  U2   CowMlkPlsStripFrqFront;
  U2   CowMlkPlsStripFrqBack;
  U1   CowMlkPlsStripRatFront;
  U1   CowMlkPlsStripRatBack;
  U2   CowMlkPlsStripThreshold;
  U2   CowMlkPlsStripOnTime;
  U2   CowMlkPlsStripOffTime;
} CowMlkPlsRef;

/* sizeof(CowMlkPlsRef) = 68 */

#define ExtCowMlkPlsRefId (MILK|LOW|REF + 11)
typedef struct _ExtCowMlkPlsRef {
  U4K  CowGenNumber;
  U2   CowMlkPlsSetting;
  U2   CowMlkPlsTakeOffThresHold;
  U2   CowMlkPlsTakeOffTime;
  U2   CowMlkPlsStmSrtTime;
  U2   CowMlkPlsStmMaxTime;
  U2   CowMlkPlsStmTresHold;
  U2   CowMlkPlsStmFrqFront;
  U2   CowMlkPlsStmFrqBack;
  U1   CowMlkPlsStmRatFront;
  U1   CowMlkPlsStmRatBack;
  U2   CowMlkPlsFrqFront;
  U2   CowMlkPlsFrqBack;
  U1   CowMlkPlsRatFront;
  U1   CowMlkPlsRatBack;
  U2   CowMlkPlsAvgSpeed;
  U1   CowMlkPlsProp01;
  U1   CowMlkPlsProp02;
  U1   CowMlkPlsProp03;
  U1   CowMlkPlsProp04;
  U1   CowMlkPlsProp05;
  U1   CowMlkPlsProp06;
  U1   CowMlkPlsProp07;
  U1   CowMlkPlsProp08;
  U1   CowMlkPlsProp09;
  U1   CowMlkPlsProp10;
  S1   CowMlkPlsRatCorr01;
  S1   CowMlkPlsRatCorr02;
  S1   CowMlkPlsRatCorr03;
  S1   CowMlkPlsRatCorr04;
  S1   CowMlkPlsRatCorr05;
  S1   CowMlkPlsRatCorr06;
  S1   CowMlkPlsRatCorr07;
  S1   CowMlkPlsRatCorr08;
  S1   CowMlkPlsRatCorr09;
  S1   CowMlkPlsRatCorr10;
  S1   CowMlkPlsFrqCorr01;
  S1   CowMlkPlsFrqCorr02;
  S1   CowMlkPlsFrqCorr03;
  S1   CowMlkPlsFrqCorr04;
  S1   CowMlkPlsFrqCorr05;
  S1   CowMlkPlsFrqCorr06;
  S1   CowMlkPlsFrqCorr07;
  S1   CowMlkPlsFrqCorr08;
  S1   CowMlkPlsFrqCorr09;
  S1   CowMlkPlsFrqCorr10;
  U2   CowMlkPlsStripFrqFront;
  U2   CowMlkPlsStripFrqBack;
  U1   CowMlkPlsStripRatFront;
  U1   CowMlkPlsStripRatBack;
  U2   CowMlkPlsStripThreshold;
  U2   CowMlkPlsStripOnTime;
  U2   CowMlkPlsStripOffTime;
} ExtCowMlkPlsRef;

/* sizeof(ExtCowMlkPlsRef) = 70 */

/* NEW milkmeter VP4001  */
#define MlkAmntRefId (MILK|LOW|REF + 12)
typedef struct _MlkAmntRef {
  U2   MlkAmntMsgCntr;
  U2   MlkTemperature;
  struct {
  	U2   MlkAmnt;
  } Array[5];     /* sizeof(Array) = 10 */
} MlkAmntRef;
/* sizeof(MlkAmntRef) = 18 */

#define CowMlkSampleResId (MILK|LOW|RES + 8)
typedef struct _CowMlkSampleRes {
  U2K  CowGenNumber;
  U1   CowMlkSample001;
  U1   CowMlkSample002;
  U1   CowMlkSample003;
  U1   CowMlkSample004;
  U1   CowMlkSample005;
  U1   CowMlkSample006;
  U1   CowMlkSample007;
  U1   CowMlkSample008;
  U1   CowMlkSample009;
  U1   CowMlkSample010;
  U1   CowMlkSample011;
  U1   CowMlkSample012;
  U1   CowMlkSample013;
  U1   CowMlkSample014;
  U1   CowMlkSample015;
  U1   CowMlkSample016;
  U1   CowMlkSample017;
  U1   CowMlkSample018;
  U1   CowMlkSample019;
  U1   CowMlkSample020;
  U1   CowMlkSample021;
  U1   CowMlkSample022;
  U1   CowMlkSample023;
  U1   CowMlkSample024;
  U1   CowMlkSample025;
  U1   CowMlkSample026;
  U1   CowMlkSample027;
  U1   CowMlkSample028;
  U1   CowMlkSample029;
  U1   CowMlkSample030;
  U1   CowMlkSample031;
  U1   CowMlkSample032;
  U1   CowMlkSample033;
  U1   CowMlkSample034;
  U1   CowMlkSample035;
  U1   CowMlkSample036;
  U1   CowMlkSample037;
  U1   CowMlkSample038;
  U1   CowMlkSample039;
  U1   CowMlkSample040;
  U1   CowMlkSample041;
  U1   CowMlkSample042;
  U1   CowMlkSample043;
  U1   CowMlkSample044;
  U1   CowMlkSample045;
  U1   CowMlkSample046;
  U1   CowMlkSample047;
  U1   CowMlkSample048;
  U1   CowMlkSample049;
  U1   CowMlkSample050;
  U1   CowMlkSample051;
  U1   CowMlkSample052;
  U1   CowMlkSample053;
  U1   CowMlkSample054;
  U1   CowMlkSample055;
  U1   CowMlkSample056;
  U1   CowMlkSample057;
  U1   CowMlkSample058;
  U1   CowMlkSample059;
  U1   CowMlkSample060;
  U1   CowMlkSample061;
  U1   CowMlkSample062;
  U1   CowMlkSample063;
  U1   CowMlkSample064;
  U1   CowMlkSample065;
  U1   CowMlkSample066;
  U1   CowMlkSample067;
  U1   CowMlkSample068;
  U1   CowMlkSample069;
  U1   CowMlkSample070;
  U1   CowMlkSample071;
  U1   CowMlkSample072;
  U1   CowMlkSample073;
  U1   CowMlkSample074;
  U1   CowMlkSample075;
  U1   CowMlkSample076;
  U1   CowMlkSample077;
  U1   CowMlkSample078;
  U1   CowMlkSample079;
  U1   CowMlkSample080;
  U1   CowMlkSample081;
  U1   CowMlkSample082;
  U1   CowMlkSample083;
  U1   CowMlkSample084;
  U1   CowMlkSample085;
  U1   CowMlkSample086;
  U1   CowMlkSample087;
  U1   CowMlkSample088;
  U1   CowMlkSample089;
  U1   CowMlkSample090;
  U1   CowMlkSample091;
  U1   CowMlkSample092;
  U1   CowMlkSample093;
  U1   CowMlkSample094;
  U1   CowMlkSample095;
  U1   CowMlkSample096;
  U1   CowMlkSample097;
  U1   CowMlkSample098;
  U1   CowMlkSample099;
  U1   CowMlkSample100;
  U1   CowMlkSample101;
  U1   CowMlkSample102;
  U1   CowMlkSample103;
  U1   CowMlkSample104;
  U1   CowMlkSample105;
  U1   CowMlkSample106;
  U1   CowMlkSample107;
  U1   CowMlkSample108;
  U1   CowMlkSample109;
  U1   CowMlkSample110;
  U1   CowMlkSample111;
  U1   CowMlkSample112;
  U1   CowMlkSample113;
  U1   CowMlkSample114;
  U1   CowMlkSample115;
  U1   CowMlkSample116;
  U1   CowMlkSample117;
  U1   CowMlkSample118;
  U1   CowMlkSample119;
  U1   CowMlkSample120;
  U1   CowMlkSample121;
  U1   CowMlkSample122;
  U1   CowMlkSample123;
  U1   CowMlkSample124;
  U1   CowMlkSample125;
  U1   CowMlkSample126;
  U1   CowMlkSample127;
  U1   CowMlkSample128;
  U1   CowMlkSample129;
  U1   CowMlkSample130;
  U1   CowMlkSample131;
  U1   CowMlkSample132;
  U1   CowMlkSample133;
  U1   CowMlkSample134;
  U1   CowMlkSample135;
  U1   CowMlkSample136;
  U1   CowMlkSample137;
  U1   CowMlkSample138;
  U1   CowMlkSample139;
  U1   CowMlkSample140;
  U1   CowMlkSample141;
  U1   CowMlkSample142;
  U1   CowMlkSample143;
  U1   CowMlkSample144;
} CowMlkSampleRes;

/* sizeof(CowMlkSampleRes) = 146 */

#define ExtCowMlkSampleResId (MILK|LOW|RES + 12)
typedef struct _ExtCowMlkSampleRes {
  U2K  CowGenNumber;
  U1   CowMlkSample001;
  U1   CowMlkSample002;
  U1   CowMlkSample003;
  U1   CowMlkSample004;
  U1   CowMlkSample005;
  U1   CowMlkSample006;
  U1   CowMlkSample007;
  U1   CowMlkSample008;
  U1   CowMlkSample009;
  U1   CowMlkSample010;
  U1   CowMlkSample011;
  U1   CowMlkSample012;
  U1   CowMlkSample013;
  U1   CowMlkSample014;
  U1   CowMlkSample015;
  U1   CowMlkSample016;
  U1   CowMlkSample017;
  U1   CowMlkSample018;
  U1   CowMlkSample019;
  U1   CowMlkSample020;
  U1   CowMlkSample021;
  U1   CowMlkSample022;
  U1   CowMlkSample023;
  U1   CowMlkSample024;
  U1   CowMlkSample025;
  U1   CowMlkSample026;
  U1   CowMlkSample027;
  U1   CowMlkSample028;
  U1   CowMlkSample029;
  U1   CowMlkSample030;
  U1   CowMlkSample031;
  U1   CowMlkSample032;
  U1   CowMlkSample033;
  U1   CowMlkSample034;
  U1   CowMlkSample035;
  U1   CowMlkSample036;
  U1   CowMlkSample037;
  U1   CowMlkSample038;
  U1   CowMlkSample039;
  U1   CowMlkSample040;
  U1   CowMlkSample041;
  U1   CowMlkSample042;
  U1   CowMlkSample043;
  U1   CowMlkSample044;
  U1   CowMlkSample045;
  U1   CowMlkSample046;
  U1   CowMlkSample047;
  U1   CowMlkSample048;
  U1   CowMlkSample049;
  U1   CowMlkSample050;
  U1   CowMlkSample051;
  U1   CowMlkSample052;
  U1   CowMlkSample053;
  U1   CowMlkSample054;
  U1   CowMlkSample055;
  U1   CowMlkSample056;
  U1   CowMlkSample057;
  U1   CowMlkSample058;
  U1   CowMlkSample059;
  U1   CowMlkSample060;
  U1   CowMlkSample061;
  U1   CowMlkSample062;
  U1   CowMlkSample063;
  U1   CowMlkSample064;
  U1   CowMlkSample065;
  U1   CowMlkSample066;
  U1   CowMlkSample067;
  U1   CowMlkSample068;
  U1   CowMlkSample069;
  U1   CowMlkSample070;
  U1   CowMlkSample071;
  U1   CowMlkSample072;
  U1   CowMlkSample073;
  U1   CowMlkSample074;
  U1   CowMlkSample075;
  U1   CowMlkSample076;
  U1   CowMlkSample077;
  U1   CowMlkSample078;
  U1   CowMlkSample079;
  U1   CowMlkSample080;
  U1   CowMlkSample081;
  U1   CowMlkSample082;
  U1   CowMlkSample083;
  U1   CowMlkSample084;
  U1   CowMlkSample085;
  U1   CowMlkSample086;
  U1   CowMlkSample087;
  U1   CowMlkSample088;
  U1   CowMlkSample089;
  U1   CowMlkSample090;
  U1   CowMlkSample091;
  U1   CowMlkSample092;
  U1   CowMlkSample093;
  U1   CowMlkSample094;
  U1   CowMlkSample095;
  U1   CowMlkSample096;
  U1   CowMlkSample097;
  U1   CowMlkSample098;
  U1   CowMlkSample099;
  U1   CowMlkSample100;
  U1   CowMlkSample101;
  U1   CowMlkSample102;
  U1   CowMlkSample103;
  U1   CowMlkSample104;
  U1   CowMlkSample105;
  U1   CowMlkSample106;
  U1   CowMlkSample107;
  U1   CowMlkSample108;
  U1   CowMlkSample109;
  U1   CowMlkSample110;
  U1   CowMlkSample111;
  U1   CowMlkSample112;
  U1   CowMlkSample113;
  U1   CowMlkSample114;
  U1   CowMlkSample115;
  U1   CowMlkSample116;
  U1   CowMlkSample117;
  U1   CowMlkSample118;
  U1   CowMlkSample119;
  U1   CowMlkSample120;
  U1   CowMlkSample121;
  U1   CowMlkSample122;
  U1   CowMlkSample123;
  U1   CowMlkSample124;
  U1   CowMlkSample125;
  U1   CowMlkSample126;
  U1   CowMlkSample127;
  U1   CowMlkSample128;
  U1   CowMlkSample129;
  U1   CowMlkSample130;
  U1   CowMlkSample131;
  U1   CowMlkSample132;
  U1   CowMlkSample133;
  U1   CowMlkSample134;
  U1   CowMlkSample135;
  U1   CowMlkSample136;
  U1   CowMlkSample137;
  U1   CowMlkSample138;
  U1   CowMlkSample139;
  U1   CowMlkSample140;
  U1   CowMlkSample141;
  U1   CowMlkSample142;
  U1   CowMlkSample143;
  U1   CowMlkSample144;
} ExtCowMlkSampleRes;

/* sizeof(ExtCowMlkSampleRes) = 148 */

#define MlkYldStatusResId (MILK|LOW|RES + 9)
typedef struct _MlkYldStatusRes {
  U2K  CowGenNumber;
  AT   CowMlkYldStartTime;
  U1   CowMlkYldNrOfAttach2min;
  U1   CowMlkYldNrOfAttach;
  BL   CowMlkYldManDetach;
  BL   CowMlkYldManInput;
  U2   CowMlkYldAmount00_15;
  U2   CowMlkYldAmount15_30;
  U2   CowMlkYldAmount30_60;
  U2   CowMlkYldAmount60_120;
} MlkYldStatusRes;

/* sizeof(MlkYldStatusRes) = 10 */

#define ExtMlkYldStatusResId (MILK|LOW|RES + 13)
typedef struct _ExtMlkYldStatusRes {
  U4K  CowGenNumber;
  AT   CowMlkYldStartTime;
  U1   CowMlkYldNrOfAttach2min;
  U1   CowMlkYldNrOfAttach;
  BL   CowMlkYldManDetach;
  BL   CowMlkYldManInput;
  U2   CowMlkYldAmount00_15;
  U2   CowMlkYldAmount15_30;
  U2   CowMlkYldAmount30_60;
  U2   CowMlkYldAmount60_120;
} ExtMlkYldStatusRes;

/* sizeof(ExtMlkYldStatusRes) = 12 */

#define MlkStateUpdateResId (MILK|LOW|RES + 14)
typedef struct _MlkStateUpdateRes {
  U2K  PerGenNumber;
  U2   CowMlkYldAmount;
  U1   MlkState;
  XX   Dummy;
} MlkStateUpdateRes;

/* sizeof(MlkStateUpdateRes) = 6 */

#endif
