
#ifndef cond_temp_vcp_flow

#define cond_temp_vcp_flow 1

#include "../flowlib/dbmstype.h"
#include "../flowlib/msg_nums.h"
#define CowTmpRefReqId (COND_TEMP|LOW|REFREQ + 0)
typedef struct _CowTmpRefReq {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
} CowTmpRefReq;

/* sizeof(CowTmpRefReq) = 4 */

#define ExtCowTmpRefReqId (COND_TEMP|LOW|REFREQ + 5)
typedef struct _ExtCowTmpRefReq {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
} ExtCowTmpRefReq;

/* sizeof(ExtCowTmpRefReq) = 6 */

#define CowTmpRefId (COND_TEMP|LOW|REF + 0)
typedef struct _CowTmpRef {
    U2K  CowGenNumber;
    U2   CowTmpMaximum;
} CowTmpRef;

/* sizeof(CowTmpRef) = 4 */

#define ExtCowTmpRefId (COND_TEMP|LOW|REF + 5)
typedef struct _ExtCowTmpRef {
    U4K  CowGenNumber;
    U2   CowTmpMaximum;
} ExtCowTmpRef;

/* sizeof(ExtCowTmpRef) = 6 */

#define CowTmpResId (COND_TEMP|LOW|RES + 0)
typedef struct _CowTmpRes {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
    U2   CowTmpLastResultTemp;
    BL   CowTmpAttention;
    XX   Dummy;
} CowTmpRes;

/* sizeof(CowTmpRes) = 8 */

#define ExtCowTmpResId (COND_TEMP|LOW|RES + 3)
typedef struct _ExtCowTmpRes {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
    U2   CowTmpLastResultTemp;
    BL   CowTmpAttention;
    XX   Dummy;
} ExtCowTmpRes;

/* sizeof(ExtCowTmpRes) = 10 */

#define CowConRefReqId (COND_TEMP|LOW|REFREQ + 1)
typedef struct _CowConRefReq {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
} CowConRefReq;

/* sizeof(CowConRefReq) = 4 */

#define ExtCowConRefReqId (COND_TEMP|LOW|REFREQ + 6)
typedef struct _ExtCowConRefReq {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
} ExtCowConRefReq;

/* sizeof(ExtCowConRefReq) = 6 */

#define CowConRefId (COND_TEMP|LOW|REF + 1)
typedef struct _CowConRef {
    U2K  CowGenNumber;
    U1   CowConDataValue1;
    U1   CowConDataValue2;
    U1   CowConDataValue3;
    U1   CowConDataValue4;
    U1   CowConRollingAverage1;
    U1   CowConRollingAverage2;
    U1   CowConRollingAverage3;
    U1   CowConRollingAverage4;
    U1   CowConDataAttentions;
    U1   HerdConNrOfValues;
    U1   HerdConThresholdValue1;
    U1   HerdConThresholdValue2;
    U1   HerdConThresholdValue3;
    U1   HerdConMinValue;
    U1   HerdConMaxValue;
    U1   HerdConAttValue;
    U1   CowConActStndDev1;
    U1   CowConActStndDev2;
    U1   CowConActStndDev3;
    U1   CowConActStndDev4;
    U1   CowConRollAvgStndDev1;
    U1   CowConRollAvgStndDev2;
    U1   CowConRollAvgStndDev3;
    U1   CowConRollAvgStndDev4;
} CowConRef;

/* sizeof(CowConRef) = 26 */

#define ExtCowConRefId (COND_TEMP|LOW|REF + 6)
typedef struct _ExtCowConRef {
    U4K  CowGenNumber;
    U1   CowConDataValue1;
    U1   CowConDataValue2;
    U1   CowConDataValue3;
    U1   CowConDataValue4;
    U1   CowConRollingAverage1;
    U1   CowConRollingAverage2;
    U1   CowConRollingAverage3;
    U1   CowConRollingAverage4;
    U1   CowConDataAttentions;
    U1   HerdConNrOfValues;
    U1   HerdConThresholdValue1;
    U1   HerdConThresholdValue2;
    U1   HerdConThresholdValue3;
    U1   HerdConMinValue;
    U1   HerdConMaxValue;
    U1   HerdConAttValue;
    U1   CowConActStndDev1;
    U1   CowConActStndDev2;
    U1   CowConActStndDev3;
    U1   CowConActStndDev4;
    U1   CowConRollAvgStndDev1;
    U1   CowConRollAvgStndDev2;
    U1   CowConRollAvgStndDev3;
    U1   CowConRollAvgStndDev4;
} ExtCowConRef;

/* sizeof(ExtCowConRef) = 28 */

#define CowConResId (COND_TEMP|LOW|RES + 1)
typedef struct _CowConRes {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
    U1   CowConDataValue1;
    U1   CowConDataValue2;
    U1   CowConDataValue3;
    U1   CowConDataValue4;
    U1   CowConRollingAverage1;
    U1   CowConRollingAverage2;
    U1   CowConRollingAverage3;
    U1   CowConRollingAverage4;
    U1   CowConDataAttentions;
    XX   Dummy;
} CowConRes;

/* sizeof(CowConRes) = 14 */

#define ExtCowConResId (COND_TEMP|LOW|RES + 4)
typedef struct _ExtCowConRes {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
    U1   CowConDataValue1;
    U1   CowConDataValue2;
    U1   CowConDataValue3;
    U1   CowConDataValue4;
    U1   CowConRollingAverage1;
    U1   CowConRollingAverage2;
    U1   CowConRollingAverage3;
    U1   CowConRollingAverage4;
    U1   CowConDataAttentions;
    XX   Dummy;
} ExtCowConRes;

/* sizeof(ExtCowConRes) = 16 */

#define HerdConRefReqId (COND_TEMP|LOW|REFREQ + 2)
typedef struct _HerdConRefReq {
    U2K  PerGenNumber;
} HerdConRefReq;

/* sizeof(HerdConRefReq) = 2 */

#define HerdConRefId (COND_TEMP|LOW|REF + 2)
typedef struct _HerdConRef {
    U1   HerdConNrOfValues;
    U1   HerdConThresholdValue1;
    U1   HerdConThresholdValue2;
    U1   HerdConThresholdValue3;
    U1   HerdConMinValue;
    U1   HerdConMaxValue;
    U1   HerdConAttValue;
    XX   Dummy;
} HerdConRef;

/* sizeof(HerdConRef) = 8 */

#define CowConSampleResId (COND_TEMP|LOW|RES + 2)
typedef struct _CowConSampleRes {
    U2K  PerGenNumber;
    U2K  CowGenNumber;
    U1K  CowConSampleTeatNumber;
    XX   Dummy;
    U1   CowConSample001;
    U1   CowConSample002;
    U1   CowConSample003;
    U1   CowConSample004;
    U1   CowConSample005;
    U1   CowConSample006;
    U1   CowConSample007;
    U1   CowConSample008;
    U1   CowConSample009;
    U1   CowConSample010;
    U1   CowConSample011;
    U1   CowConSample012;
    U1   CowConSample013;
    U1   CowConSample014;
    U1   CowConSample015;
    U1   CowConSample016;
    U1   CowConSample017;
    U1   CowConSample018;
    U1   CowConSample019;
    U1   CowConSample020;
    U1   CowConSample021;
    U1   CowConSample022;
    U1   CowConSample023;
    U1   CowConSample024;
    U1   CowConSample025;
    U1   CowConSample026;
    U1   CowConSample027;
    U1   CowConSample028;
    U1   CowConSample029;
    U1   CowConSample030;
    U1   CowConSample031;
    U1   CowConSample032;
    U1   CowConSample033;
    U1   CowConSample034;
    U1   CowConSample035;
    U1   CowConSample036;
    U1   CowConSample037;
    U1   CowConSample038;
    U1   CowConSample039;
    U1   CowConSample040;
    U1   CowConSample041;
    U1   CowConSample042;
    U1   CowConSample043;
    U1   CowConSample044;
    U1   CowConSample045;
    U1   CowConSample046;
    U1   CowConSample047;
    U1   CowConSample048;
    U1   CowConSample049;
    U1   CowConSample050;
    U1   CowConSample051;
    U1   CowConSample052;
    U1   CowConSample053;
    U1   CowConSample054;
    U1   CowConSample055;
    U1   CowConSample056;
    U1   CowConSample057;
    U1   CowConSample058;
    U1   CowConSample059;
    U1   CowConSample060;
    U1   CowConSample061;
    U1   CowConSample062;
    U1   CowConSample063;
    U1   CowConSample064;
    U1   CowConSample065;
    U1   CowConSample066;
    U1   CowConSample067;
    U1   CowConSample068;
    U1   CowConSample069;
    U1   CowConSample070;
    U1   CowConSample071;
    U1   CowConSample072;
    U1   CowConSample073;
    U1   CowConSample074;
    U1   CowConSample075;
    U1   CowConSample076;
    U1   CowConSample077;
    U1   CowConSample078;
    U1   CowConSample079;
    U1   CowConSample080;
    U1   CowConSample081;
    U1   CowConSample082;
    U1   CowConSample083;
    U1   CowConSample084;
    U1   CowConSample085;
    U1   CowConSample086;
    U1   CowConSample087;
    U1   CowConSample088;
    U1   CowConSample089;
    U1   CowConSample090;
    U1   CowConSample091;
    U1   CowConSample092;
    U1   CowConSample093;
    U1   CowConSample094;
    U1   CowConSample095;
    U1   CowConSample096;
    U1   CowConSample097;
    U1   CowConSample098;
    U1   CowConSample099;
    U1   CowConSample100;
    U1   CowConSample101;
    U1   CowConSample102;
    U1   CowConSample103;
    U1   CowConSample104;
    U1   CowConSample105;
    U1   CowConSample106;
    U1   CowConSample107;
    U1   CowConSample108;
    U1   CowConSample109;
    U1   CowConSample110;
    U1   CowConSample111;
    U1   CowConSample112;
    U1   CowConSample113;
    U1   CowConSample114;
    U1   CowConSample115;
    U1   CowConSample116;
    U1   CowConSample117;
    U1   CowConSample118;
    U1   CowConSample119;
    U1   CowConSample120;
    U1   CowConSample121;
    U1   CowConSample122;
    U1   CowConSample123;
    U1   CowConSample124;
    U1   CowConSample125;
    U1   CowConSample126;
    U1   CowConSample127;
    U1   CowConSample128;
    U1   CowConSample129;
    U1   CowConSample130;
    U1   CowConSample131;
    U1   CowConSample132;
    U1   CowConSample133;
    U1   CowConSample134;
    U1   CowConSample135;
    U1   CowConSample136;
    U1   CowConSample137;
    U1   CowConSample138;
    U1   CowConSample139;
    U1   CowConSample140;
    U1   CowConSample141;
    U1   CowConSample142;
    U1   CowConSample143;
    U1   CowConSample144;
} CowConSampleRes;

/* sizeof(CowConSampleRes) = 150 */

#define ExtCowConSampleResId (COND_TEMP|LOW|RES + 5)
typedef struct _ExtCowConSampleRes {
    U2K  PerGenNumber;
    U4K  CowGenNumber;
    U1K  CowConSampleTeatNumber;
    XX   Dummy;
    U1   CowConSample001;
    U1   CowConSample002;
    U1   CowConSample003;
    U1   CowConSample004;
    U1   CowConSample005;
    U1   CowConSample006;
    U1   CowConSample007;
    U1   CowConSample008;
    U1   CowConSample009;
    U1   CowConSample010;
    U1   CowConSample011;
    U1   CowConSample012;
    U1   CowConSample013;
    U1   CowConSample014;
    U1   CowConSample015;
    U1   CowConSample016;
    U1   CowConSample017;
    U1   CowConSample018;
    U1   CowConSample019;
    U1   CowConSample020;
    U1   CowConSample021;
    U1   CowConSample022;
    U1   CowConSample023;
    U1   CowConSample024;
    U1   CowConSample025;
    U1   CowConSample026;
    U1   CowConSample027;
    U1   CowConSample028;
    U1   CowConSample029;
    U1   CowConSample030;
    U1   CowConSample031;
    U1   CowConSample032;
    U1   CowConSample033;
    U1   CowConSample034;
    U1   CowConSample035;
    U1   CowConSample036;
    U1   CowConSample037;
    U1   CowConSample038;
    U1   CowConSample039;
    U1   CowConSample040;
    U1   CowConSample041;
    U1   CowConSample042;
    U1   CowConSample043;
    U1   CowConSample044;
    U1   CowConSample045;
    U1   CowConSample046;
    U1   CowConSample047;
    U1   CowConSample048;
    U1   CowConSample049;
    U1   CowConSample050;
    U1   CowConSample051;
    U1   CowConSample052;
    U1   CowConSample053;
    U1   CowConSample054;
    U1   CowConSample055;
    U1   CowConSample056;
    U1   CowConSample057;
    U1   CowConSample058;
    U1   CowConSample059;
    U1   CowConSample060;
    U1   CowConSample061;
    U1   CowConSample062;
    U1   CowConSample063;
    U1   CowConSample064;
    U1   CowConSample065;
    U1   CowConSample066;
    U1   CowConSample067;
    U1   CowConSample068;
    U1   CowConSample069;
    U1   CowConSample070;
    U1   CowConSample071;
    U1   CowConSample072;
    U1   CowConSample073;
    U1   CowConSample074;
    U1   CowConSample075;
    U1   CowConSample076;
    U1   CowConSample077;
    U1   CowConSample078;
    U1   CowConSample079;
    U1   CowConSample080;
    U1   CowConSample081;
    U1   CowConSample082;
    U1   CowConSample083;
    U1   CowConSample084;
    U1   CowConSample085;
    U1   CowConSample086;
    U1   CowConSample087;
    U1   CowConSample088;
    U1   CowConSample089;
    U1   CowConSample090;
    U1   CowConSample091;
    U1   CowConSample092;
    U1   CowConSample093;
    U1   CowConSample094;
    U1   CowConSample095;
    U1   CowConSample096;
    U1   CowConSample097;
    U1   CowConSample098;
    U1   CowConSample099;
    U1   CowConSample100;
    U1   CowConSample101;
    U1   CowConSample102;
    U1   CowConSample103;
    U1   CowConSample104;
    U1   CowConSample105;
    U1   CowConSample106;
    U1   CowConSample107;
    U1   CowConSample108;
    U1   CowConSample109;
    U1   CowConSample110;
    U1   CowConSample111;
    U1   CowConSample112;
    U1   CowConSample113;
    U1   CowConSample114;
    U1   CowConSample115;
    U1   CowConSample116;
    U1   CowConSample117;
    U1   CowConSample118;
    U1   CowConSample119;
    U1   CowConSample120;
    U1   CowConSample121;
    U1   CowConSample122;
    U1   CowConSample123;
    U1   CowConSample124;
    U1   CowConSample125;
    U1   CowConSample126;
    U1   CowConSample127;
    U1   CowConSample128;
    U1   CowConSample129;
    U1   CowConSample130;
    U1   CowConSample131;
    U1   CowConSample132;
    U1   CowConSample133;
    U1   CowConSample134;
    U1   CowConSample135;
    U1   CowConSample136;
    U1   CowConSample137;
    U1   CowConSample138;
    U1   CowConSample139;
    U1   CowConSample140;
    U1   CowConSample141;
    U1   CowConSample142;
    U1   CowConSample143;
    U1   CowConSample144;
} ExtCowConSampleRes;

/* sizeof(ExtCowConSampleRes) = 152 */

#define KbdCowTmpRefReqId (COND_TEMP|LOW|REFREQ + 3)
typedef struct _KbdCowTmpRefReq {
    U2   KbdCommand;
    U2K  CowTmpCowId;
} KbdCowTmpRefReq;

/* sizeof(KbdCowTmpRefReq) = 4 */

#define KbdCowTmpRefId (COND_TEMP|LOW|REF + 3)
typedef struct _KbdCowTmpRef {
    U2K  CowTmpCowId;
    struct {
        U1K  CowTmpRefNumber;
        BL   CowTmpAttention;
        DT   CowTmpResultDate;
        TM   CowTmpResultTime;
        U2   CowTmpLastResultTemp;
        U2   CowTmpRollingAverage;
        U2   CowTmpStandardDeviation;
        U2   CowTmpMaximum;
    } Array[1];    /* sizeof(Array) = 14 */
} KbdCowTmpRef;

/* sizeof(KbdCowTmpRef) = 16 */

#define KbdCowConRefReqId (COND_TEMP|LOW|REFREQ + 4)
typedef struct _KbdCowConRefReq {
    U2   KbdCommand;
    U2K  CowConCowId;
} KbdCowConRefReq;

/* sizeof(KbdCowConRefReq) = 4 */

#define KbdCowConRefId (COND_TEMP|LOW|REF + 4)
typedef struct _KbdCowConRef {
    U2K  CowConCowId;
    U1   CowConRollingAverage1;
    U1   CowConRollingAverage2;
    U1   CowConRollingAverage3;
    U1   CowConRollingAverage4;
    struct {
        DT   CowConDataDate;
        TM   CowConDataTime;
        U2   CowConDataPerNumber;
        U1   CowConDataValue1;
        U1   CowConDataValue2;
        U1   CowConDataValue3;
        U1   CowConDataValue4;
        BL   CowConDataAttention1;
        BL   CowConDataAttention2;
        BL   CowConDataAttention3;
        BL   CowConDataAttention4;
    } Array[1];    /* sizeof(Array) = 14 */
} KbdCowConRef;

/* sizeof(KbdCowConRef) = 20 */


#endif
