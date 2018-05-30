
#ifndef separation_vcp_flow

#define separation_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"

#define CowSepRefReqId (SEPARATION|LOW|REFREQ + 0)
typedef struct _CowSepRefReq {
  U2K  PerGenNumber;
  U2K  CowGenRespNumber;
} CowSepRefReq;

/* sizeof(CowSepRefReq) = 4 */

#define CowSepRefId (SEPARATION|LOW|REF + 0)
typedef struct _CowSepRef {
  U2K  CowGenRespNumber;
  U2K  CowGenId;
  BL   CowSepSeparationYesNo;
  U1   PerSepGateNumber;
} CowSepRef;

/* sizeof(CowSepRef) = 6 */

#define CowSepResId (SEPARATION|LOW|RES + 0)
typedef struct _CowSepRes {
  U2K  PerGenNumber;
  U2K  CowGenId;
  BL   CowSepSeparationYesNo;
  U1   PerSepGateNumber;
} CowSepRes;

/* sizeof(CowSepRes) = 6 */

#define CowSepInfoRefReqId (SEPARATION|LOW|REFREQ + 1)
typedef struct _CowSepInfoRefReq {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
} CowSepInfoRefReq;

/* sizeof(CowSepInfoRefReq) = 4 */

#define ExtCowSepInfoRefReqId (SEPARATION|LOW|REFREQ + 3)
typedef struct _ExtCowSepInfoRefReq {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
} ExtCowSepInfoRefReq;

/* sizeof(ExtCowSepInfoRefReq) = 6 */

#define CowSepInfoRefId (SEPARATION|LOW|REF + 1)
typedef struct _CowSepInfoRef {
  U2K  CowGenNumber;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
  } Array[1];  /* sizeof(Array) = 2 */
} CowSepInfoRef;

/* sizeof(CowSepInfoRef) = 4 */

#define ExtCowSepInfoRefId (SEPARATION|LOW|REF + 3)
typedef struct _ExtCowSepInfoRef {
  U4K  CowGenNumber;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
  } Array[1];  /* sizeof(Array) = 2 */
} ExtCowSepInfoRef;

/* sizeof(ExtCowSepInfoRef) = 6 */

#define CowSepInfoResId (SEPARATION|LOW|RES + 1)
typedef struct _CowSepInfoRes {
  U2K  PerGenNumber;
  U2K  CowGenNumber;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
  } Array[1];  /* sizeof(Array) = 2 */
} CowSepInfoRes;

/* sizeof(CowSepInfoRes) = 6 */

#define ExtCowSepInfoResId (SEPARATION|LOW|RES + 3)
typedef struct _ExtCowSepInfoRes {
  U2K  PerGenNumber;
  U4K  CowGenNumber;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
  } Array[1];  /* sizeof(Array) = 2 */
} ExtCowSepInfoRes;

/* sizeof(ExtCowSepInfoRes) = 8 */

#define KbdCowSepRefReqId (SEPARATION|LOW|REFREQ + 2)
typedef struct _KbdCowSepRefReq {
  U2   KbdCommand;
  U2K  CowSepCowId;
} KbdCowSepRefReq;

/* sizeof(KbdCowSepRefReq) = 4 */

#define KbdCowSepRefId (SEPARATION|LOW|REF + 2)
typedef struct _KbdCowSepRef {
  U2K  CowSepCowId;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
    DT   CowSepStartDate;
    TM   CowSepStartTime;
    DT   CowSepEndDate;
    TM   CowSepEndTime;
  } Array[1];  /* sizeof(Array) = 10 */
} KbdCowSepRef;

/* sizeof(KbdCowSepRef) = 12 */

#define KbdCowSepResId (SEPARATION|LOW|RES + 2)
typedef struct _KbdCowSepRes {
  U2   KbdCommand;
  U2K  CowSepCowId;
  struct {
    U1K  CowSepFunction;
    U1   CowSepNrOfSeparations;
    DT   CowSepStartDate;
    TM   CowSepStartTime;
    DT   CowSepEndDate;
    TM   CowSepEndTime;
  } Array[1];  /* sizeof(Array) = 10 */
} KbdCowSepRes;

/* sizeof(KbdCowSepRes) = 14 */


#endif
