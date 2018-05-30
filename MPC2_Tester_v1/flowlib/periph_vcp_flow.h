
#ifndef periph_vcp_flow

#define periph_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"   

#define PerMimiRefReqId (PERIPH|LOW|REFREQ + 0)
typedef struct _PerMimiRefReq {
	U2K  PerMimiPerGenNumber;
	struct {
		U2K  PerMimiStandNr;
	} Array[1];	/* sizeof(Array) = 2 */
} PerMimiRefReq;

/* sizeof(PerMimiRefReq) = 4 */

#define PerMimiRefId (PERIPH|LOW|REF + 0)
typedef struct _PerMimiRef {
	U2K  PerMimiPerGenNumber;
	U2K  PerMimiStandNr;
	U2   PerMimiUniqueAddress;
	U1   PerMimiType;
	XX   PerMimiDummy;
} PerMimiRef;

/* sizeof(PerMimiRef) = 8 */

#define PerMimiRefEndId (PERIPH|LOW|RESREQ + 0)
typedef struct _PerMimiRefEnd {
	S2   Empty;
} PerMimiRefEnd;

/* sizeof(PerMimiRefEnd) = 0 */

#define PerMimiResId (PERIPH|LOW|RES + 0)
typedef struct _PerMimiRes {
	U2K  PerMimiPerGenNumber;
	U2K  PerMimiStandNr;
	U2   PerMimiUniqueAddress;
	U2   PerMimiVersion;
	U2   PerMimiErrors;
	U1   PerMimiType;
	U1   PerMimiPowerLevel;
	U1   PerMimiPortData1;
	U1   PerMimiPortData2;
	U1   PerMimiPortData3;
	U1   PerMimiPortData4;
	U1   PerMimiPortData5;
	U1   PerMimiPortData6;
	U1   PerMimiPortData7;
	U1   PerMimiPortData8;
	U2   PerMimiUnused1;
	U2   PerMimiUnused2;
	U2   PerMimiUnused3;
	U2   PerMimiUnused4;
} PerMimiRes;

/* sizeof(PerMimiRes) = 28 */

#define PerMimiInstallResEndId (PERIPH|LOW|REF + 1)
typedef struct _PerMimiInstallResEnd {
	S2   Empty;
} PerMimiInstallResEnd;

/* sizeof(PerMimiInstallResEnd) = 0 */

#define PerMimiInstallResReqId (PERIPH|LOW|RESREQ + 1)
typedef struct _PerMimiInstallResReq {
	U2K  PerMimiPerGenNumber;
} PerMimiInstallResReq;

/* sizeof(PerMimiInstallResReq) = 2 */

#define PerMimiInstallResId (PERIPH|LOW|RES + 1)
typedef struct _PerMimiInstallRes {
	U2K  PerMimiPerGenNumber;
	U2K  PerMimiStandNr;
	U2   PerMimiUniqueAddress;
	U2   PerMimiVersion;
	U2   PerMimiErrors;
	U1   PerMimiType;
	U1   PerMimiPowerLevel;
	U1   PerMimiPortData1;
	U1   PerMimiPortData2;
	U1   PerMimiPortData3;
	U1   PerMimiPortData4;
	U1   PerMimiPortData5;
	U1   PerMimiPortData6;
	U1   PerMimiPortData7;
	U1   PerMimiPortData8;
	U2   PerMimiUnused1;
	U2   PerMimiUnused2;
	U2   PerMimiUnused3;
	U2   PerMimiUnused4;
} PerMimiInstallRes;

/* sizeof(PerMimiInstallRes) = 28 */

#define PerMimiStatusResEndId (PERIPH|LOW|REF + 2)
typedef struct _PerMimiStatusResEnd {
	S2   Empty;
} PerMimiStatusResEnd;

/* sizeof(PerMimiStatusResEnd) = 0 */

#define PerMimiStatusResReqId (PERIPH|LOW|RESREQ + 2)
typedef struct _PerMimiStatusResReq {
	U2K  PerMimiPerGenNumber;
} PerMimiStatusResReq;

/* sizeof(PerMimiStatusResReq) = 2 */

#define PerMimiStatusResId (PERIPH|LOW|RES + 2)
typedef struct _PerMimiStatusRes {
	U2K  PerMimiPerGenNumber;
	U2K  PerMimiStandNr;
	U2   PerMimiUniqueAddress;
	U2   PerMimiVersion;
	U2   PerMimiErrors;
	U1   PerMimiType;
	U1   PerMimiPowerLevel;
	U1   PerMimiPortData1;
	U1   PerMimiPortData2;
	U1   PerMimiPortData3;
	U1   PerMimiPortData4;
	U1   PerMimiPortData5;
	U1   PerMimiPortData6;
	U1   PerMimiPortData7;
	U1   PerMimiPortData8;
	U2   PerMimiUnused1;
	U2   PerMimiUnused2;
	U2   PerMimiUnused3;
	U2   PerMimiUnused4;
} PerMimiStatusRes;

/* sizeof(PerMimiStatusRes) = 28 */

#define PerMimiConfigRefId (PERIPH|LOW|REF + 3)
typedef struct _PerMimiConfigRef {
	U2K  PerMimiPerGenNumber;
} PerMimiConfigRef;

/* sizeof(PerMimiConfigRef) = 2 */

#define PerMimiErrorResId (PERIPH|LOW|RES + 3)
typedef struct _PerMimiErrorRes {
	U2K  PerMimiPerGenNumber;
	U2K  PerMimiStandNr;
	U2   PerMimiErrors;
} PerMimiErrorRes;

/* sizeof(PerMimiErrorRes) = 6 */

#define PerMPCInstRefId (PERIPH|LOW|REF + 4)
typedef struct _PerMPCInstRef {
	U2K  PerGenNumber;
	struct {
		U2   PerGenBlock1;
	} Array[1];	/* sizeof(Array) = 2 */
} PerMPCInstRef;

/* sizeof(PerMPCInstRef) = 4 */

#define PerMPCInstResReqId (PERIPH|LOW|RESREQ + 4)
typedef struct _PerMPCInstResReq {
	U2K  PerGenNumber;
} PerMPCInstResReq;

/* sizeof(PerMPCInstResReq) = 2 */

#define PerMPCInstResId (PERIPH|LOW|RES + 4)
typedef struct _PerMPCInstRes {
	U2K  PerGenNumber;
	struct {
		U2   PerGenBlock1;
	} Array[1];	/* sizeof(Array) = 2 */
} PerMPCInstRes;

/* sizeof(PerMPCInstRes) = 4 */


#endif
