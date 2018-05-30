
#ifndef disease_weight_vcp_flow

#define disease_weight_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define CowDiseaseRefReqId (DISEASE_WEIGHT|LOW|REFREQ + 0)
typedef struct _CowDiseaseRefReq {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
} CowDiseaseRefReq;

/* sizeof(CowDiseaseRefReq) = 4 */

#define ExtCowDiseaseRefReqId (DISEASE_WEIGHT|LOW|REFREQ + 1)
typedef struct _ExtCowDiseaseRefReq {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
} ExtCowDiseaseRefReq;

/* sizeof(ExtCowDiseaseRefReq) = 6 */

#define CowDiseaseRefId (DISEASE_WEIGHT|LOW|REF + 0)
typedef struct _CowDiseaseRef {
    U2K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
    } Array[1];    /* sizeof(Array) = 2 */
} CowDiseaseRef;

/* sizeof(CowDiseaseRef) = 4 */

#define ExtCowDiseaseRefId (DISEASE_WEIGHT|LOW|REF + 1)
typedef struct _ExtCowDiseaseRef {
    U4K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
    } Array[1];    /* sizeof(Array) = 2 */
} ExtCowDiseaseRef;

/* sizeof(ExtCowDiseaseRef) = 6 */

#define CowDiseaseResId (DISEASE_WEIGHT|LOW|RES + 0)
typedef struct _CowDiseaseRes {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
    } Array[1];    /* sizeof(Array) = 2 */
} CowDiseaseRes;

/* sizeof(CowDiseaseRes) = 6 */

#define ExtCowDiseaseResId (DISEASE_WEIGHT|LOW|RES + 4)
typedef struct _ExtCowDiseaseRes {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
    } Array[1];    /* sizeof(Array) = 2 */
} ExtCowDiseaseRes;

/* sizeof(ExtCowDiseaseRes) = 8 */

#define CowDiseaseCuredResId (DISEASE_WEIGHT|LOW|RES + 1)
typedef struct _CowDiseaseCuredRes {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        XX   Dummy;
    } Array[1];    /* sizeof(Array) = 2 */
} CowDiseaseCuredRes;

/* sizeof(CowDiseaseCuredRes) = 6 */

#define ExtCowDiseaseCuredResId (DISEASE_WEIGHT|LOW|RES + 5)
typedef struct _ExtCowDiseaseCuredRes {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
    struct {
        U1K  CowDiseaseCode;
        XX   Dummy;
    } Array[1];    /* sizeof(Array) = 2 */
} ExtCowDiseaseCuredRes;

/* sizeof(ExtCowDiseaseCuredRes) = 8 */

#define CowWhtResId (DISEASE_WEIGHT|LOW|RES + 2)
typedef struct _CowWhtRes {
    U2K  PerGenNumber;
    U2K  CowGenRespNumber;
    U2   CowWhtWeight;
} CowWhtRes;

/* sizeof(CowWhtRes) = 6 */

#define KbdCowDisRefReqId (DISEASE_WEIGHT|LOW|REFREQ + 3)
typedef struct _KbdCowDisRefReq {
    U2   KbdCommand;
    U2K  CowDiseaseCowId;
} KbdCowDisRefReq;

/* sizeof(KbdCowDisRefReq) = 4 */

#define KbdCowDisRefId (DISEASE_WEIGHT|LOW|REF + 3)
typedef struct _KbdCowDisRef {
    U2K  CowDiseaseCowId;
    U2   CowWhtWeight;
    DT   CowWhtResultDate;
    TM   CowWhtResultTime;
    U2   CowWhtPerNumber;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
        DT   CowDiseaseStart;
        DT   CowDiseaseEnd;
        BL   CowDiseaseCured;
        XX   Dummy;
    } Array[1];    /* sizeof(Array) = 8 */
} KbdCowDisRef;

/* sizeof(KbdCowDisRef) = 18 */

#define KbdCowDisResId (DISEASE_WEIGHT|LOW|RES + 3)
typedef struct _KbdCowDisRes {
    U2   KbdCommand;
    U2K  CowDiseaseCowId;
    struct {
        U1K  CowDiseaseCode;
        U1   CowDiseaseTreatment;
        DT   CowDiseaseStart;
        DT   CowDiseaseEnd;
        BL   CowDiseaseCured;
        XX   Dummy;
    } Array[1];    /* sizeof(Array) = 8 */
} KbdCowDisRes;

/* sizeof(KbdCowDisRes) = 12 */


#endif
