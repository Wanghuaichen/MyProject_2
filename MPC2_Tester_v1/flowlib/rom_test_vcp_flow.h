
#ifndef rom_test_vcp_flow

#define rom_test_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define LoggerOnId (ROM_TEST|LOW|REF + 0)
typedef struct _LoggerOn {
        S2   Empty;
} LoggerOn;

/* sizeof(LoggerOn) = 0 */

#define LoggerOffId (ROM_TEST|LOW|REF + 1)
typedef struct _LoggerOff {
        S2   Empty;
} LoggerOff;

/* sizeof(LoggerOff) = 0 */

#define PerFuncTestId (ROM_TEST|LOW|REF + 2)
typedef struct _PerFuncTest {
        U2   PerFuncTestCommand;
        struct {
                U1   PerFuncTestDataHigh;
                U1   PerFuncTestDataLow;
        } Array[4];     /* sizeof(Array) = 8 */
} PerFuncTest;

/* sizeof(PerFuncTest) = 4 */


#endif
