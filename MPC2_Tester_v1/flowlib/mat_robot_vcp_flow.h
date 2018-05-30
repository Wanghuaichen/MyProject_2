
#ifndef mat_robot_vcp_flow

#define mat_robot_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define CowMatRefReqId (MAT_ROBOT|LOW|REFREQ + 0)
typedef struct _CowMatRefReq {
    U2K  CowMatCowId;
} CowMatRefReq;

/* sizeof(CowMatRefReq) = 2 */

#define CowMatRefId (MAT_ROBOT|LOW|REF + 0)
typedef struct _CowMatRef {
    U2K  CowMatCowId;
    U2K  CowGenNumber;
    U2   CowGenGrpNumber;
    DT   CowGenCalvingDate;
    DT   CowGenDryOffDate;
    U2   CowMatIntervalTime;
    BL   CowMatShortClean;
    BL   CowMatMilkSeparate;
    BL   CowMatCowSeparate;
    BL   CowMatDoNotMilk;
    BL   CowMatDoNotMilkTeatRF;
    BL   CowMatDoNotMilkTeatRR;
    BL   CowMatDoNotMilkTeatLR;
    BL   CowMatDoNotMilkTeatLF;
} CowMatRef;

/* sizeof(CowMatRef) = 20 */

#define CowMatResId (MAT_ROBOT|LOW|RES + 0)
typedef struct _CowMatRes {
    U2K  CowMatCowId;
    U2   CowMatIntervalTime;
    BL   CowMatShortClean;
    BL   CowMatMilkSeparate;
    BL   CowMatCowSeparate;
    BL   CowMatDoNotMilk;
    BL   CowMatDoNotMilkTeatRF;
    BL   CowMatDoNotMilkTeatRR;
    BL   CowMatDoNotMilkTeatLR;
    BL   CowMatDoNotMilkTeatLF;
} CowMatRes;

/* sizeof(CowMatRes) = 12 */

#define CowMatDataRefReqId (MAT_ROBOT|LOW|REFREQ + 1)
typedef struct _CowMatDataRefReq {
    U2K  CowMatDataCowId;
} CowMatDataRefReq;

/* sizeof(CowMatDataRefReq) = 2 */

#define CowMatDataRefId (MAT_ROBOT|LOW|REF + 1)
typedef struct _CowMatDataRef {
    U2K  CowMatDataCowId;
    ATK  CowMatDataTime;
    U2   CowMatDataMatNr;
    U2   CowMatDataAttachTimeRF;
    U2   CowMatDataAttachTimeRR;
    U2   CowMatDataAttachTimeLR;
    U2   CowMatDataAttachTimeLF;
    U2   CowMatDataAttachDurationRF;
    U2   CowMatDataAttachDurationRR;
    U2   CowMatDataAttachDurationLR;
    U2   CowMatDataAttachDurationLF;
    S2   CowMatDataPositionRFX;
    S2   CowMatDataPositionRFY;
    S2   CowMatDataPositionRFZ;
    S2   CowMatDataPositionRRX;
    S2   CowMatDataPositionRRY;
    S2   CowMatDataPositionRRZ;
    S2   CowMatDataPositionLRX;
    S2   CowMatDataPositionLRY;
    S2   CowMatDataPositionLRZ;
    S2   CowMatDataPositionLFX;
    S2   CowMatDataPositionLFY;
    S2   CowMatDataPositionLFZ;
    U2   CowMatDataSmplPosition;
    U2   CowMatDataFailureCode;
    BL   CowMatDataMilkingFailed;
    BL   CowMatDataMlkSeparated;
    BL   CowMatDataCowSeparated;
    U1   CowMatDataTimesAttached;
    U2   CowMatDataMilkYield;
    U1   CowMatDataConductRF;
    U1   CowMatDataConductRR;
    U1   CowMatDataConductLR;
    U1   CowMatDataConductLF;
} CowMatDataRef;

/* sizeof(CowMatDataRef) = 62 */

#define CowMatDataResId (MAT_ROBOT|LOW|RES + 1)
typedef struct _CowMatDataRes {
    U2K  CowMatDataCowId;
    U2   CowMatDataMatNr;
    U2   CowMatDataAttachTimeRF;
    U2   CowMatDataAttachTimeRR;
    U2   CowMatDataAttachTimeLR;
    U2   CowMatDataAttachTimeLF;
    U2   CowMatDataAttachDurationRF;
    U2   CowMatDataAttachDurationRR;
    U2   CowMatDataAttachDurationLR;
    U2   CowMatDataAttachDurationLF;
    S2   CowMatDataPositionRFX;
    S2   CowMatDataPositionRFY;
    S2   CowMatDataPositionRFZ;
    S2   CowMatDataPositionRRX;
    S2   CowMatDataPositionRRY;
    S2   CowMatDataPositionRRZ;
    S2   CowMatDataPositionLRX;
    S2   CowMatDataPositionLRY;
    S2   CowMatDataPositionLRZ;
    S2   CowMatDataPositionLFX;
    S2   CowMatDataPositionLFY;
    S2   CowMatDataPositionLFZ;
    U2   CowMatDataSmplPosition;
    U2   CowMatDataFailureCode;
    BL   CowMatDataMilkingFailed;
    BL   CowMatDataMlkSeparated;
    BL   CowMatDataCowSeparated;
    U1   CowMatDataTimesAttached;
} CowMatDataRes;

/* sizeof(CowMatDataRes) = 52 */

#define PerMatRefReqId (MAT_ROBOT|LOW|REFREQ + 2)
typedef struct _PerMatRefReq {
    U2K  PerMatNumber;
} PerMatRefReq;

/* sizeof(PerMatRefReq) = 2 */

#define PerMatRefId (MAT_ROBOT|LOW|REF + 2)
typedef struct _PerMatRef {
    U2K  PerMatNumber;
    U2   PerMatSepCapacity;
    U2   PerMatMaxWaitTime;
    U2   PerMatMaxAttachTime;
    U2   PerMatTeatSprayTime;
    U2   PerMatShortCleanTime;
    U2   PerMatFeedBox;
    U2   PerMatMilkMeter;
    U2   PerMatAccessStart1;
    U2   PerMatAccessEnd1;
    U2   PerMatAccessStart2;
    U2   PerMatAccessEnd2;
    BL   PerMatAutoMlkSep;
    U1   PerMatAutoDetachTime;
    U1   PerMatConductTreshold1;
    U1   PerMatConductTreshold2;
    U1   PerMatMaxReattachRatio;
    XX   Dummy;
} PerMatRef;

/* sizeof(PerMatRef) = 30 */

#define PerMatResId (MAT_ROBOT|LOW|RES + 2)
typedef struct _PerMatRes {
    U2K  PerMatNumber;
    U2   PerMatSepCapacity;
    U2   PerMatMaxWaitTime;
    U2   PerMatMaxAttachTime;
    U2   PerMatTeatSprayTime;
    U2   PerMatShortCleanTime;
    U2   PerMatAccessStart1;
    U2   PerMatAccessEnd1;
    U2   PerMatAccessStart2;
    U2   PerMatAccessEnd2;
    BL   PerMatAutoMlkSep;
    U1   PerMatAutoDetachTime;
    U1   PerMatConductTreshold1;
    U1   PerMatConductTreshold2;
    U1   PerMatMaxReattachRatio;
    XX   Dummy;
} PerMatRes;

/* sizeof(PerMatRes) = 26 */

#define CowMatRefusalResId (MAT_ROBOT|LOW|RES + 3)
typedef struct _CowMatRefusalRes {
    U2K  CowMatRefusalCowId;
} CowMatRefusalRes;

/* sizeof(CowMatRefusalRes) = 2 */

#define PerMatFeedRefId (MAT_ROBOT|LOW|REF + 14)
typedef struct _PerMatFeedRef {
    BL   PerMatFeedYesNo;
    XX   Dummy;
} PerMatFeedRef;

/* sizeof(PerMatFeedRef) = 2 */

#define PerMatMlkRefId (MAT_ROBOT|LOW|REF + 15)
typedef struct _PerMatMlkRef {
    BL   PerMatMlkStart;
    BL   PerMatMlkEnd;
    BL   PerMatMlkFailed;
    XX   Dummy;
} PerMatMlkRef;

/* sizeof(PerMatMlkRef) = 4 */

#define PerMatMlkResReqId (MAT_ROBOT|LOW|RESREQ + 15)
typedef struct _PerMatMlkResReq {
    S2   Empty;
} PerMatMlkResReq;

/* sizeof(PerMatMlkResReq) = 0 */

#define PerMatMlkResId (MAT_ROBOT|LOW|RES + 15)
typedef struct _PerMatMlkRes {
    U2   PerMatMlkAmount;
    U2   PerMatTmpActual;
    U2   PerMatTmpHigh;
    U1   PerMatConCurrentRF;
    U1   PerMatConCurrentRR;
    U1   PerMatConCurrentLR;
    U1   PerMatConCurrentLF;
    U1   PerMatConAvgHighRF;
    U1   PerMatConAvgHighRR;
    U1   PerMatConAvgHighLR;
    U1   PerMatConAvgHighLF;
} PerMatMlkRes;

/* sizeof(PerMatMlkRes) = 14 */


#endif
