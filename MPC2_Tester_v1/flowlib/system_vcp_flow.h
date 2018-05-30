
#ifndef system_vcp_flow

#define system_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define SysVcpcNumberRefReqId (SYSTEM|HIGH|REFREQ + 0)
typedef struct _SysVcpcNumberRefReq {
        S2   Empty;
} SysVcpcNumberRefReq;

/* sizeof(SysVcpcNumberRefReq) = 0 */

#define SysVcpcNumberRefId (SYSTEM|HIGH|REF + 0)
typedef struct _SysVcpcNumberRef {
        U1   VcpcNumber;
        XX   Dummy;
} SysVcpcNumberRef;

/* sizeof(SysVcpcNumberRef) = 2 */

#define SysGenAbsTimeBroadcastId (SYSTEM|HIGH|REF + 1)
typedef struct _SysGenAbsTimeBroadcast {
        AT   SysGenAbsTime;
} SysGenAbsTimeBroadcast;

/* sizeof(SysGenAbsTimeBroadcast) = 4 */


#endif
