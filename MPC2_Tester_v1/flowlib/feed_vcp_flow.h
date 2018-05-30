
#ifndef feed_vcp_flow

#define feed_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"


#define CowFedRefReqId (FEED|LOW|REFREQ + 0)
typedef struct _CowFedRefReq {
  U2K  PerGenNumber;
  U2K  CowGenRespNumber;
} CowFedRefReq;

/* sizeof(CowFedRefReq) = 4 */

#define CowFedRefId (FEED|LOW|REF + 0)
typedef struct _CowFedRef {
  U2K  CowGenRespNumber;
  struct {
    U2   CowFedPerBalance;
    U2   GrpLmtSpeed;
  } Array[1];  /* sizeof(Array) = 4 */
} CowFedRef;

/* sizeof(CowFedRef) = 6 */

#define CowFedResId (FEED|LOW|RES + 0)
typedef struct _CowFedRes {
  U2K  PerGenNumber;
  U2K  CowGenRespNumber;
  struct {
    U2   CowFedAmountFed;
  } Array[1];  /* sizeof(Array) = 2 */
} CowFedRes;

/* sizeof(CowFedRes) = 6 */

#define CowFedParlRefReqId (FEED|LOW|REFREQ + 1)
typedef struct _CowFedParlRefReq {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
} CowFedParlRefReq;

/* sizeof(CowFedParlRefReq) = 4 */

#define ExtCowFedParlRefReqId (FEED|LOW|REFREQ + 8)
typedef struct _ExtCowFedParlRefReq {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
} ExtCowFedParlRefReq;

/* sizeof(ExtCowFedParlRefReq) = 6 */

#define CowFedParlRefId (FEED|LOW|REF + 1)
typedef struct _CowFedParlRef {
  U2K  CowGenNumber;
  struct {
    U2   CowFedParlBalance;
    U2   GrpLmtSpeed;
  } Array[1];  /* sizeof(Array) = 4 */
} CowFedParlRef;

/* sizeof(CowFedParlRef) = 6 */

#define ExtCowFedParlRefId (FEED|LOW|REF + 8)
typedef struct _ExtCowFedParlRef {
  U4K  CowGenNumber;
  struct {
    U2   CowFedParlBalance;
    U2   GrpLmtSpeed;
  } Array[1];  /* sizeof(Array) = 4 */
} ExtCowFedParlRef;

/* sizeof(ExtCowFedParlRef) = 8 */

#define CowFedParlResId (FEED|LOW|RES + 1)
typedef struct _CowFedParlRes {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
  struct {
    U2   CowFedParlAmountFed;
  } Array[1];  /* sizeof(Array) = 2 */
} CowFedParlRes;

/* sizeof(CowFedParlRes) = 6 */

#define ExtCowFedParlResId (FEED|LOW|RES + 2)
typedef struct _ExtCowFedParlRes {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
  struct {
    U2   CowFedParlAmountFed;
  } Array[1];  /* sizeof(Array) = 2 */
} ExtCowFedParlRes;

/* sizeof(ExtCowFedParlRes) = 8 */

#define CowFedInfoRefReqId (FEED|LOW|REFREQ + 2)
typedef struct _CowFedInfoRefReq {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
} CowFedInfoRefReq;

/* sizeof(CowFedInfoRefReq) = 4 */

#define ExtCowFedInfoRefReqId (FEED|LOW|REFREQ + 9)
typedef struct _ExtCowFedInfoRefReq {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
} ExtCowFedInfoRefReq;

/* sizeof(ExtCowFedInfoRefReq) = 4 */

#define CowFedInfoRefId (FEED|LOW|REF + 2)
typedef struct _CowFedInfoRef {
  U2K  CowGenNumber;
  struct {
    U2K  CowFedNumber;
    U2   CowFedPrevTotalFed;
    U2   CowFedTotal;
    U2   CowFedBalance;
    U2   CowFedPerBalance;
    U2   CowFedCurrentTotalFed;
    U1   CowFedNrOfBackCycles;
    U1   CowFedCurrentPeriod;
  } Array[1];  /* sizeof(Array) = 14 */
} CowFedInfoRef;

/* sizeof(CowFedInfoRef) = 16 */

#define ExtCowFedInfoRefId (FEED|LOW|REF + 9)
typedef struct _ExtCowFedInfoRef {
  U4K  CowGenNumber;
  struct {
    U2K  CowFedNumber;
    U2   CowFedPrevTotalFed;
    U2   CowFedTotal;
    U2   CowFedBalance;
    U2   CowFedPerBalance;
    U2   CowFedCurrentTotalFed;
    U1   CowFedNrOfBackCycles;
    U1   CowFedCurrentPeriod;
  } Array[1];  /* sizeof(Array) = 14 */
} ExtCowFedInfoRef;

/* sizeof(ExtCowFedInfoRef) = 18 */

#define CalFedRefId (FEED|LOW|REF + 3)
typedef struct _CalFedRef {
  U2K  CalFedRespNumber;
  U1   CalFedMtrNumber;
  U1   CalFedNrOfRevolutions;
} CalFedRef;

/* sizeof(CalFedRef) = 4 */

#define CowFedAccRefReqId (FEED|LOW|REFREQ + 4)
typedef struct _CowFedAccRefReq {
  U2K  PerGenNumber;
  U2K  CowGenRespNumber;
  U2K  CowFedAccDoorNumber;
} CowFedAccRefReq;

/* sizeof(CowFedAccRefReq) = 6 */

#define CowFedAccRefId (FEED|LOW|REF + 4)
typedef struct _CowFedAccRef {
  U2K  CowGenRespNumber;
  U2K  CowFedAccDoorNumber;
  BL   CowFedAccGranted;
  XX   Dummy;
} CowFedAccRef;

/* sizeof(CowFedAccRef) = 6 */

#define CowFedAccResId (FEED|LOW|RES + 4)
typedef struct _CowFedAccRes {
	U2K  PerGenNumber;
	U2K  CowGenRespNumber;
} CowFedAccRes;

/* sizeof(CowFedAccRes) = 4 */

#define CowFedPrlRspRefReqId (FEED|LOW|REFREQ + 5)
typedef struct _CowFedPrlRspRefReq {
  U2K  PerGenNumber;
  U2K  CowGenRespNumber;
} CowFedPrlRspRefReq;

/* sizeof(CowFedPrlRspRefReq) = 4 */

#define CowFedPrlRspRefId (FEED|LOW|REF + 5)
typedef struct _CowFedPrlRspRef {
  U2K  CowGenNumber;
  U2K  CowGenRespNumber;
  struct {
    U2   CowFedParlBalance;
    U2   GrpLmtSpeed;
  } Array[1];  /* sizeof(Array) = 4 */
} CowFedPrlRspRef;

/* sizeof(CowFedPrlRspRef) = 8 */

#define ExtCowFedPrlRspRefId (FEED|LOW|REF + 10)
typedef struct _ExtCowFedPrlRspRef {
  U4K  CowGenNumber;
  U2K  CowGenRespNumber;
  struct {
    U2   CowFedParlBalance;
    U2   GrpLmtSpeed;
  } Array[1];  /* sizeof(Array) = 4 */
} ExtCowFedPrlRspRef;

/* sizeof(ExtCowFedPrlRspRef) = 10 */

#define KbdCowFedRefReqId (FEED|LOW|REFREQ + 6)
typedef struct _KbdCowFedRefReq {
  U2   KbdCommand;
  U2K  CowFedCowId;
} KbdCowFedRefReq;

/* sizeof(KbdCowFedRefReq) = 4 */

#define KbdCowFedRefId (FEED|LOW|REF + 6)
typedef struct _KbdCowFedRef {
  U2K  CowFedCowId;
  U2K  CowFedParlNumber;
  U2   CowFedParlTotalOrg;
  U2   CowFedParlTotal;
  U2   CowFedParlCumulative;
  U2   CowFedParlBalance;
  U2   CowFedParlCurrentTotalFed;
  U2   CowFedParlPrevTotalFed;
  DT   CowFedParlNextCalcDate;
  TM   CowFedParlNextCalcTime;
  U1   CowFedMethod;
  BL   CowFedFixed;
  struct {
    U2K  CowFedNumber;
    U2   CowFedTotal;
    U2   CowFedNextTotal;
    U2   CowFedOrigTotal;
    U2   CowFedFixedTotal;
    U2   CowFedCumulative;
    U2   CowFedBalance;
    U2   CowFedPerBalance;
    U2   CowFedCurrentTotalFed;
    U2   CowFedPrevTotalFed;
    U1   CowFedCurrentPeriod;
    U1   CowFedCurrentCycle;
  } Array[1];  /* sizeof(Array) = 22 */
} KbdCowFedRef;

/* sizeof(KbdCowFedRef) = 44 */

#define KbdCowFedResId (FEED|LOW|RES + 6)
typedef struct _KbdCowFedRes {
  U2   KbdCommand;
  U2K  CowFedCowId;
  U2K  CowFedParlNumber;
  U2   CowFedParlTotal;
  U1   CowFedMethod;
  BL   CowFedFixed;
  struct {
    U2K  CowFedNumber;
    U2   CowFedFixedTotal;
  } Array[1];  /* sizeof(Array) = 4 */
} KbdCowFedRes;

/* sizeof(KbdCowFedRes) = 14 */


#define CowFedParlInfoRefReqId (FEED|LOW|REFREQ + 7)
typedef struct _CowFedParlInfoRefReq {
	U2K  PerGenNumber;
	U2K  CowGenNumber;
} CowFedParlInfoRefReq;

/* sizeof(CowFedParlInfoRefReq) = 4 */

#define ExtCowFedParlInfoRefReqId (FEED|LOW|REFREQ + 10)
typedef struct _ExtCowFedParlInfoRefReq {
	U2K  PerGenNumber;
	U4K  CowGenNumber;
} ExtCowFedParlInfoRefReq;

/* sizeof(ExtCowFedParlInfoRefReq) = 6 */

#define CowFedParlInfoRefId (FEED|LOW|REF + 7)
typedef struct _CowFedParlInfoRef {
	U2K  CowGenNumber;
	struct {
		U2K  CowFedNumber;
		U2   CowFedParlTotal;
		U2   CowFedParlBalance;
		U2   CowFedParlCurrentTotalFed;
		U2   CowFedParlPrevTotalFed;
	} Array[1];	/* sizeof(Array) = 10 */
} CowFedParlInfoRef;

/* sizeof(CowFedParlInfoRef) = 12 */

#define ExtCowFedParlInfoRefId (FEED|LOW|REF + 11)
typedef struct _ExtCowFedParlInfoRef {
	U4K  CowGenNumber;
	struct {
		U2K  CowFedNumber;
		U2   CowFedParlTotal;
		U2   CowFedParlBalance;
		U2   CowFedParlCurrentTotalFed;
		U2   CowFedParlPrevTotalFed;
	} Array[1];	/* sizeof(Array) = 10 */
} ExtCowFedParlInfoRef;

/* sizeof(ExtCowFedParlInfoRef) = 14 */

#define CowExtFedParlRefId (FEED|LOW|REF + 12)
typedef struct _CowExtFedParlRef {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
  U2   CowExtParlTotal;
} CowExtFedParlRef;

/* sizeof(CowExtFedParlRef) = 6 */

#endif
