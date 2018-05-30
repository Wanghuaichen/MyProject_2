
#ifndef activity_vcp_flow

#define activity_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define CowActMlkStandResId (ACTIVITY|LOW|RES + 0)
typedef struct _CowActMlkStandRes {
        struct {
                U2K  CowGenRespNumber;
                U2   CowActCounterValue;
        } Array[1];     /* sizeof(Array) = 4 */
} CowActMlkStandRes;

/* sizeof(CowActMlkStandRes) = 4 */

#define CowActRefReqId (ACTIVITY|LOW|REFREQ + 0)
typedef struct _CowActRefReq {
        U2K  PerGenNumber;
        U2K  CowGenNumber;
} CowActRefReq;

/* sizeof(CowActRefReq) = 4 */

#define ExtCowActRefReqId (ACTIVITY|LOW|REFREQ + 6)
typedef struct _ExtCowActRefReq {
        U2K  PerGenNumber;
        U4K  CowGenNumber;
} ExtCowActRefReq;

/* sizeof(ExtCowActRefReq) = 6 */

#define CowActRefId (ACTIVITY|LOW|REF + 0)
typedef struct _CowActRef {
        U2K  CowGenNumber;
        S2   CowActDeviation;
        BL   CowActAttention;
        XX   Dummy;
} CowActRef;

/* sizeof(CowActRef) = 6 */

#define ExtCowActRefId (ACTIVITY|LOW|REF + 6)
typedef struct _ExtCowActRef {
        U4K  CowGenNumber;
        S2   CowActDeviation;
        BL   CowActAttention;
        XX   Dummy;
} ExtCowActRef;

/* sizeof(ExtCowActRef) = 8 */

#define CowActResId (ACTIVITY|LOW|RES + 1)
typedef struct _CowActRes {
        U2K  CowGenRespNumber;
        U2   CowActCounterValue;
} CowActRes;

/* sizeof(CowActRes) = 4 */

#define KbdCowActRefReqId (ACTIVITY|LOW|REFREQ + 2)
typedef struct _KbdCowActRefReq {
        U2   KbdCommand;
        U2K  CowActCowId;
} KbdCowActRefReq;

/* sizeof(KbdCowActRefReq) = 4 */

#define KbdCowActRefId (ACTIVITY|LOW|REF + 2)
typedef struct _KbdCowActRef {
        U2K  CowActCowId;
        struct {
                DT   CowActDate;
                TM   CowActTime;
                U2   CowActCounterValue;
                U2   CowActPeriodAvg;
                U2   CowActPeriodRollAvg;
                S2   CowActDeviation;
                BL   CowActAttention;
                XX   Dummy;
        } Array[1];     /* sizeof(Array) = 14 */
} KbdCowActRef;

/* sizeof(KbdCowActRef) = 16 */

#define CowActMemResId (ACTIVITY|LOW|RES + 3)
typedef struct _CowActMemRes {
	U2K  CowGenRespNumber;
	U1   CowActMemPeriod;
	U1   CowActMemMultiplier;
	U1   CowActMemData01;
	U1   CowActMemData02;
	U1   CowActMemData03;
	U1   CowActMemData04;
	U1   CowActMemData05;
	U1   CowActMemData06;
	U1   CowActMemData07;
	U1   CowActMemData08;
	U1   CowActMemData09;
	U1   CowActMemData10;
	U1   CowActMemData11;
	U1   CowActMemData12;
	U1   CowActMemData13;
	U1   CowActMemData14;
} CowActMemRes;

/* sizeof(CowActMemRes) = 18 */

#define CowActLayResId (ACTIVITY|LOW|RES + 4)
typedef struct _CowActLayRes {
	U2K  CowGenRespNumber;
	U1   CowActLayPeriod;
	U1   CowActLayMultiplier;
	U1   CowActLayData01;
	U1   CowActLayData02;
	U1   CowActLayData03;
	U1   CowActLayData04;
	U1   CowActLayData05;
	U1   CowActLayData06;
	U1   CowActLayData07;
	U1   CowActLayData08;
	U1   CowActLayData09;
	U1   CowActLayData10;
	U1   CowActLayData11;
	U1   CowActLayData12;
	U1   CowActLayData13;
	U1   CowActLayData14;
} CowActLayRes;

/* sizeof(CowActLayRes) = 18 */

#define CowActMemAttRefReqId (ACTIVITY|LOW|REFREQ + 5)
typedef struct _CowActMemAttRefReq {
	U2K  PerGenNumber;
	U2K  CowGenNumber;
} CowActMemAttRefReq;

/* sizeof(CowActMemAttRefReq) = 4 */

#define ExtCowActMemAttRefReqId (ACTIVITY|LOW|REFREQ + 7)
typedef struct _ExtCowActMemAttRefReq {
	U2K  PerGenNumber;
	U4K  CowGenNumber;
} ExtCowActMemAttRefReq;

/* sizeof(ExtCowActMemAttRefReq) = 6 */

#define CowActMemAttRefId (ACTIVITY|LOW|REF + 5)
typedef struct _CowActMemAttRef {
	U2K  CowGenNumber;
	TM   CowActMemPerSrtTime;
	TM   CowActMemPerEndTime;
	TM   CowActMemPerMaxSrtTime;
	TM   CowActMemPerMaxEndTime;
	S2   CowActMemPerDev;
	BL   CowActMemPerMaxAtt;
	XX   Dummy;
	S2   CowActMemPerDev01;
	S2   CowActMemPerDev02;
	S2   CowActMemPerDev03;
	S2   CowActMemPerDev04;
	S2   CowActMemPerDev05;
	S2   CowActMemPerDev06;
	S2   CowActMemPerDev07;
	S2   CowActMemPerDev08;
	S2   CowActMemPerDev09;
	S2   CowActMemPerDev10;
	S2   CowActMemPerDev11;
	S2   CowActMemPerDev12;
	S2   CowActMemPerDev13;
	S2   CowActMemPerDev14;
} CowActMemAttRef;

/* sizeof(CowActMemAttRef) = 42 */

#define ExtCowActMemAttRefId (ACTIVITY|LOW|REF + 7)
typedef struct _ExtCowActMemAttRef {
	U4K  CowGenNumber;
	TM   CowActMemPerSrtTime;
	TM   CowActMemPerEndTime;
	TM   CowActMemPerMaxSrtTime;
	TM   CowActMemPerMaxEndTime;
	S2   CowActMemPerDev;
	BL   CowActMemPerMaxAtt;
	XX   Dummy;
	S2   CowActMemPerDev01;
	S2   CowActMemPerDev02;
	S2   CowActMemPerDev03;
	S2   CowActMemPerDev04;
	S2   CowActMemPerDev05;
	S2   CowActMemPerDev06;
	S2   CowActMemPerDev07;
	S2   CowActMemPerDev08;
	S2   CowActMemPerDev09;
	S2   CowActMemPerDev10;
	S2   CowActMemPerDev11;
	S2   CowActMemPerDev12;
	S2   CowActMemPerDev13;
	S2   CowActMemPerDev14;
} ExtCowActMemAttRef;

/* sizeof(ExtCowActMemAttRef) = 44 */


#endif
