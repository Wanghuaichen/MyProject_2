#ifndef test_mpc1_flow

#define test_mpc1_flow 1

#include "../flowlib/test_nums.h"
#include "../flowlib/dbmstype.h"

#define FTBurnInResReqId (DUURPROEF_NMS|LOW|RESREQ + 0)

typedef struct _FTBurnInResReq {
    S2 Empty;
} FTBurnInResReq;

#define FTBurnInRefId (DUURPROEF_NMS|LOW|REF + 0)

typedef struct _FTBurnInRef {
    S2 Empty;
} FTBurnInRef;

#define FTBurnInResId (DUURPROEF_NMS|LOW|RES + 0)

typedef struct _FTBurnInRes {
    U1 FTReady;
    U1 FTRomResult;
    U1 FTRamResult;
    U1 FTVC3Result;
    U1 FTPMResult;
    U1 FTRelaisResult;
    U1 FTUartResult;
    U1 Dummy;
} FTBurnInRes;


#define FTBurnInTotResReqId (INTERNAL_TEST_NMS|LOW|RESREQ + 0)

typedef struct _FTBurnInTotResReq {
    S2 Empty;
} FTBurnInTotResReq;

#define FTBurnInTotRefId (INTERNAL_TEST_NMS|LOW|REF + 0)

typedef struct _FTBurnInTotRef {
    U1 Command;
    U1 Empty;
} FTBurnInTotRef;

#define FTBurnInTotResId (INTERNAL_TEST_NMS|LOW|RES + 0)

typedef struct _FTBurnInTotRes {
    U2 FTNrOfTests;
    U2 FTTotRomResult;
    U2 FTTotRamResult;
    U2 FTTotVC3Result;
    U2 FTTotPMResult;
    U2 FTTotRelaisResult;
    U2 FTTotUartResult;
} FTBurnInTotRes;

  

#endif
