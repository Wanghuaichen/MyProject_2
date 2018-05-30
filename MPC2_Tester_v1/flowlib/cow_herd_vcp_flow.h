
#ifndef cow_herd_vcp_flow

#define cow_herd_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"

#define CowGenRefReqId (COW_HERD|LOW|REFREQ + 0)
typedef struct _CowGenRefReq {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
} CowGenRefReq;

/* sizeof(CowGenRefReq) = 4 */

#define ExtCowGenRefReqId (COW_HERD|LOW|REFREQ + 3)
typedef struct _ExtCowGenRefReq {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
} ExtCowGenRefReq;

/* sizeof(ExtCowGenRefReq) = 6 */

#define CowGenRefId (COW_HERD|LOW|REF + 0)
typedef struct _CowGenRef {
  U2K  CowGenNumber;
  U2K  CowGenId;
  U2K  CowGenRespNumber;
  U2   CowGenGrpNumber;
  U2   CowGenNextGrpNumber;
} CowGenRef;

/* sizeof(CowGenRef) = 10 */

#define ExtCowGenRefId (COW_HERD|LOW|REF + 3)
typedef struct _ExtCowGenRef {
  U4K  CowGenNumber;
  U2K  CowGenId;
  U2K  CowGenRespNumber;
  U2   CowGenGrpNumber;
  U2   CowGenNextGrpNumber;
} ExtCowGenRef;

/* sizeof(ExtCowGenRef) = 10 */

#define CowGenResId (COW_HERD|LOW|RES + 0)
typedef struct _CowGenRes {
  U2K  CowGenNumber;
  U2   CowGenNextGrpNumber;
} CowGenRes;

/* sizeof(CowGenRes) = 4 */

#define ExtCowGenResId (COW_HERD|LOW|RES + 2)
typedef struct _ExtCowGenRes {
  U4K  CowGenNumber;
  U2   CowGenNextGrpNumber;
} ExtCowGenRes;

/* sizeof(ExtCowGenRes) = 6 */

#define KbdCowGenRefReqId (COW_HERD|LOW|REFREQ + 1)
typedef struct _KbdCowGenRefReq {
  U2   KbdCommand;
  U2K  CowGenNumber;
  U2K  CowGenRespNumber;
} KbdCowGenRefReq;

/* sizeof(KbdCowGenRefReq) = 6 */

#define KbdCowGenRefId (COW_HERD|LOW|REF + 1)
typedef struct _KbdCowGenRef {
  U2K  CowGenNumber;
  U2K  CowGenId;
  U2K  CowGenRespNumber;
  U2   CowGenGrpNumber;
  U2   CowGenNextGrpNumber;
  U2   CowGenSireNumber;
  DT   CowGenBirthDate;
  DT   CowGenCalvingDate;
  DT   CowGenExpCalvingDate;
  DT   CowGenHeatDate1;
  DT   CowGenHeatDate2;
  DT   CowGenHeatDate3;
  U2   CowGenHeatDays1;
  U2   CowGenHeatDays2;
  U2   CowGenHeatDays3;
  DT   CowGenHeatDate;
  U2   CowGenHeatDays;
  DT   CowGenDryOffDate;
  DT   CowGenRemoveDate;
  U2   CowGenRemoveReason;
  U2   CowGenCalvingIntv;
  U2   CowGenLactDays;
  U1   CowGenNrOfLact;
  U1   CowGenReproState;
  BL   CowGenPregnant;
  BL   CowGenColost;
  BL   CowGenHeatAttent;
  BL   CowGenInsemAttent;
  BL   CowGenDryOffAttent;
  BL   CowGenIncalfAttent;
  BL   CowGenInsem1;
  BL   CowGenInsem2;
  BL   CowGenInsem3;
  BL   CowGenInsem;
  U2   CowGenSireNr1;
  U2   CowGenSireNr2;
  U2   CowGenSireNr3;
  U2   CowGenNumberPc;
} KbdCowGenRef;

/* sizeof(KbdCowGenRef) = 64 */

#define KbdCowGenResId (COW_HERD|LOW|RES + 1)
typedef struct _KbdCowGenRes {
  U2   KbdCommand;
  U2K  CowGenId;
  U2K  CowGenNumber;
  U2K  CowGenRespNumber;
  U2   CowGenGrpNumber;
  U2   CowGenNextGrpNumber;
  U2   CowGenSireNumber;
  DT   CowGenBirthDate;
  DT   CowGenCalvingDate;
  DT   CowGenHeatDate;
  DT   CowGenDryOffDate;
  DT   CowGenRemoveDate;
  U2   CowGenRemoveReason;
  U1   CowGenNrOfLact;
  BL   CowGenPregnant;
  BL   CowGenColost;
  BL   CowGenInsem;
  U2   CowGenSireNr1;
  U2   CowGenSireNr2;
  U2   CowGenSireNr3;
  U2   CowGenNumberPc;
} KbdCowGenRes;

/* sizeof(KbdCowGenRes) = 38 */

#define KbdCowGenFastRefReqId (COW_HERD|LOW|REFREQ + 2)
typedef struct _KbdCowGenFastRefReq {
  U2   KbdCommand;
  U2K  CowGenNumber;
  U2K  CowGenRespNumber;
} KbdCowGenFastRefReq;

/* sizeof(KbdCowGenFastRefReq) = 6 */

#define KbdErrorRefId (COW_HERD|LOW|REF + 2)
typedef struct _KbdErrorRef {
  U2   KbdCommand;
  U2   KbdResult;
  U2   KbdMessId;
  U2K  CowGenId;
} KbdErrorRef;

/* sizeof(KbdErrorRef) = 8 */


#endif
