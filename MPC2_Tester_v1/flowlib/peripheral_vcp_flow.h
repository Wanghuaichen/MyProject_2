
#ifndef peripheral_vcp_flow

#define peripheral_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"

#define PerGenRefReqId (PERIPHERAL|LOW|REFREQ + 0)
typedef struct _PerGenRefReq {
        U2K  PerGenNumber;
} PerGenRefReq;

/* sizeof(PerGenRefReq) = 2 */

#define PerGenRefId (PERIPHERAL|LOW|REF + 0)
typedef struct _PerGenRef {
        U2K  PerGenNumber;
        AT   PerGenAbsTime;
        U2   PerGenBlock1;
        U2   PerGenBlock2;
        U2   PerGenBlock3;
        U2   PerGenBlock4;
        U2   PerGenBlock5;
        U2   PerGenBlock6;
        U2   PerGenBlock7;
        U2   PerGenBlock8;
        U2   PerGenBlock9;
        U2   PerGenBlock10;
        U2   PerGenBlock11;
        U2   PerGenBlock12;
        U2   PerGenBlock13;
        U2   PerGenBlock14;
        struct {
                U2   MtrGenPortion;
        } Array[1];     /* sizeof(Array) = 2 */
} PerGenRef;

/* sizeof(PerGenRef) = 36 */

#define PerGenResId (PERIPHERAL|LOW|RES + 0)
typedef struct _PerGenRes {
        U2K  PerGenNumber;
        U2   PerGenBlock1;
        U2   PerGenBlock2;
        U2   PerGenBlock3;
        U2   PerGenBlock4;
        U2   PerGenBlock5;
        U2   PerGenBlock6;
        U2   PerGenBlock7;
        U2   PerGenBlock8;
        U2   PerGenBlock9;
        U2   PerGenBlock10;
        U2   PerGenBlock11;
        U2   PerGenBlock12;
        U2   PerGenBlock13;
        U2   PerGenBlock14;
} PerGenRes;

/* sizeof(PerGenRes) = 30 */

#define PerGenFunctionsRefReqId (PERIPHERAL|LOW|REFREQ + 1)
typedef struct _PerGenFunctionsRefReq {
        U2K  PerGenNumber;
} PerGenFunctionsRefReq;

/* sizeof(PerGenFunctionsRefReq) = 2 */

#define PerGenFunctionsRefId (PERIPHERAL|LOW|REF + 1)
typedef struct _PerGenFunctionsRef {
        U2K  PerGenNumber;
        U2   PerGenFunctions;
} PerGenFunctionsRef;

/* sizeof(PerGenFunctionsRef) = 4 */

#define PerGenInstResReqId (PERIPHERAL|LOW|RESREQ + 1)
typedef struct _PerGenInstResReq {
        U2K  PerGenNumber;
} PerGenInstResReq;

/* sizeof(PerGenInstResReq) = 2 */

#define PerGenInstResId (PERIPHERAL|LOW|RES + 1)
typedef struct _PerGenInstRes {
        U2K  PerGenNumber;
        U2   PerGenProgCode;
        U1   PerGenProgVersionHigh;
        U1   PerGenProgVersionLow;
        BL   PerGenFuncIdentification;
        BL   PerGenFuncFeeding;
        BL   PerGenFuncMilkMeasuring;
        BL   PerGenFuncAccess;
        BL   PerGenFuncSeparation;
        BL   PerGenFuncWeight;
        BL   PerGenFuncTemperature;
        BL   PerGenFuncConductivity;
        BL   PerGenFuncActivity;
        BL   PerGenFuncDrinking;
        BL   PerGenFuncMarking;
        U1   PerGenType;
        BL   PerGenDownload;
        U1   PerGenEEProgVersHigh;
        U1   PerGenEEProgVersLow;
        U1   PerGenDldBrdcst;
} PerGenInstRes;

/* sizeof(PerGenInstRes) = 22 */

#define PerGenErrorResId (PERIPHERAL|LOW|RESREQ + 2)
typedef struct _PerGenErrorRes {
        U2K  PerGenNumber;
        U2   PerGenErrorInfo;
        U2   PerGenWarningInfo;
} PerGenErrorRes;

/* sizeof(PerGenErrorRes) = 6 */

#define PerGenErrorResetRefId (PERIPHERAL|LOW|REF + 2)
typedef struct _PerGenErrorResetRef {
        S2   Empty;
} PerGenErrorResetRef;

/* sizeof(PerGenErrorResetRef) = 0 */

#define PerGenNumberRefReqId (PERIPHERAL|LOW|REFREQ + 3)
typedef struct _PerGenNumberRefReq {
        S2   Empty;
} PerGenNumberRefReq;

/* sizeof(PerGenNumberRefReq) = 0 */

#define PerGenNumberRefId (PERIPHERAL|LOW|REF + 3)
typedef struct _PerGenNumberRef {
        U2K  PerGenNumber;
} PerGenNumberRef;

/* sizeof(PerGenNumberRef) = 2 */

#define PerPresenceResReqId (PERIPHERAL|LOW|RESREQ + 3)
typedef struct _PerPresenceResReq {
        S2   Empty;
} PerPresenceResReq;

/* sizeof(PerPresenceResReq) = 0 */

#define PerPresenceResId (PERIPHERAL|LOW|RES + 3)
typedef struct _PerPresenceRes {
        U2K  PerGenNumber;
} PerPresenceRes;

/* sizeof(PerPresenceRes) = 2 */

#define PerGenExtRefReqId (PERIPHERAL|LOW|REFREQ + 4)
typedef struct _PerGenExtRefReq {
        U2K  PerGenExtNumber;
} PerGenExtRefReq;

/* sizeof(PerGenExtRefReq) = 2 */

#define PerGenExtRefId (PERIPHERAL|LOW|REF + 4)
typedef struct _PerGenExtRef {
        U2K  PerGenExtNumber;
        U2   PerGenExtBlock1;
        U2   PerGenExtBlock2;
        U2   PerGenExtBlock3;
        U2   PerGenExtBlock4;
        U2   PerGenExtBlock5;
        U2   PerGenExtBlock6;
        U2   PerGenExtBlock7;
        U2   PerGenExtBlock8;
        U2   PerGenExtBlock9;
        U2   PerGenExtBlock10;
        U2   PerGenExtBlock11;
        U2   PerGenExtBlock12;
        U2   PerGenExtBlock13;
        U2   PerGenExtBlock14;
        U2   PerGenExtBlock15;
        U2   PerGenExtBlock16;
} PerGenExtRef;

/* sizeof(PerGenExtRef) = 34 */


#define KbdPerGenCowRefReqId (PERIPHERAL|LOW|REFREQ + 5)
typedef struct _KbdPerGenCowRefReq {
	U2K  PerGenNumber;
} KbdPerGenCowRefReq;

/* sizeof(KbdPerGenCowRefReq) = 2 */

#define KbdPerGenCowRefId (PERIPHERAL|LOW|REF + 5)
typedef struct _KbdPerGenCowRef {
	U2K  PerGenNumber;
	U2   PerGenIdCow;
	U2   PerGenCowNumber;
	U2   PerGenRespNumber;
	DT   PerGenRespDate;
	TM   PerGenRespTime;
} KbdPerGenCowRef;

/* sizeof(KbdPerGenCowRef) = 12 */


#define PerHfrowBroadcastId (PERIPHERAL|HIGH|REF + 0)
typedef struct _PerHfrowBroadcast {
        U1K  PerHfrowNumber;
        XX   Dummy;
} PerHfrowBroadcast;

/* sizeof(PerHfrowBroadcast) = 2 */


#endif
