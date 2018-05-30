
#ifndef transfer_vcp_flow

#define transfer_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define DldPerEraseRefId (TRANSFER|LOW|REF + 0)
typedef struct _DldPerEraseRef {
   U4 DummyLong1;  /* (<[ */
   U4 DummyLong2;  /* INF */
   U4 DummyLong3;  /* ]>) */
   U2 EEBankSize;
   U2 StartEEBank;
   U1 NrEEBanks;
   U1 StartPROGBank;
   U1 StartDBMSBank;
   U1 Dummy;
   U4 DummyLong4;  /* FFFF */
   U4 DummyLong5;  /* #$% */
   U4 DummyLong6;  /* ROM */
   U4 DummyLong7;  /* %$# */
   struct {
   U4   DldPerData;
   } Array[1];     /* sizeof(Array) = 4 */
} DldPerEraseRef;

/* sizeof(DldPerEraseRef) = 2 */

#define DldPerEraseResReqId (TRANSFER|LOW|RESREQ + 0)
typedef struct _DldPerEraseResReq {
        struct {
                U2   DldPerData;
        } Array[1];     /* sizeof(Array) = 2 */
} DldPerEraseResReq;

/* sizeof(DldPerEraseResReq) = 2 */

#define DldPerEraseResId (TRANSFER|LOW|RES + 0)
typedef struct _DldPerEraseRes {
                U2 DldPerGenNumber;
                U2 DldPerResult;
} DldPerEraseRes;

/* sizeof(DldPerEraseRes) = 2 */

#define DldPerProgRefId (TRANSFER|LOW|REF + 1)
typedef struct _DldPerProgRef {
        U2  TotBytes;
        U2  DataBytes;
        U4  Address;
        struct {
                U2   DldPerData;
        } Array[1];     /* sizeof(Array) = 2 */
} DldPerProgRef;

/* sizeof(DldPerProgRef) = 2 */

#define DldPerProgResReqId (TRANSFER|LOW|RESREQ + 1)
typedef struct _DldPerProgResReq {
        struct {
                U2   DldPerData;
        } Array[1];     /* sizeof(Array) = 2 */
} DldPerProgResReq;

/* sizeof(DldPerProgResReq) = 2 */

#define DldPerProgResId (TRANSFER|LOW|RES + 1)
typedef struct _DldPerProgRes {
                U2 DldPerGenNumber;
                U2   DldPerResult;
} DldPerProgRes;

/* sizeof(DldPerProgRes) = 2 */

#define DldPerDbmsRefId (TRANSFER|LOW|REF + 2)
typedef struct _DldPerDbmsRef {
        struct {
                U2   DldPerData;
        } Array[1];     /* sizeof(Array) = 2 */
} DldPerDbmsRef;

/* sizeof(DldPerDbmsRef) = 2 */

#define DldPerDbmsResReqId (TRANSFER|LOW|RESREQ + 2)
typedef struct _DldPerDbmsResReq {
        struct {
                U2   DldPerData;
        } Array[1];     /* sizeof(Array) = 2 */
} DldPerDbmsResReq;

/* sizeof(DldPerDbmsResReq) = 2 */

#define DldPerDbmsResId (TRANSFER|LOW|RES + 2)
typedef struct _DldPerDbmsRes {
                U2 DldPerGenNumber;
                U2   DldPerResult;
} DldPerDbmsRes;

/* sizeof(DldPerDbmsRes) = 2 */


#endif
