#ifndef DISP_INCLUDED
#define DISP_INCLUDED           1

#include "../lib/boolean.h"
#include "../init/start.h"
#include "../lib/item.h"

#define StandAlone()         (EEAddressCopy ==0)
#define StandAloneFeeding() (((EEAddressCopy ==0) && EEFeeding ) != 0 ? 1 : 0 )
#define SwingOver()          ( EEAddress2 )


// screen-number are divided in different groups -------------------------------
#define   SM_SERVICE              0
#define   SM_ROBOT                1
#define   SM_MILKING              2
#define   SM_DISEASE_MILK_INFO    3
#define   SM_ACT_DISEASE_INFO     4
#define   SM_COW_INFO             5
#define   SM_MILK_INFO            6
#define   SM_CALENDAR_INFO        7
#define   SM_FEED_INFO            8
#define   SM_PULSE_INFO           9
#define   SM_CAL_TEMP_INFO       10
#define   SM_ERROR               11
#if ( _EMC_TEST_ )
#define   SM_EMC_TEST            12
#endif

// screens SM_SERVICE ----------------------------------------------------------
#define   SCREEN_MILK_0           0
#define   SCREEN_MILK_1           1
#define   SCREEN_MILK_COW         1
#define   SCREEN_MILK_2           2
#define   SCREEN_MILK_3           3
#define   SCREEN_MILK_4           4
#define   SCREEN_MILK_5           5
#define   SCREEN_MILK_6           6
#define   SCREEN_MILK_7           7
#define   SCREEN_MILK_MAX         8
#define   SCREEN_CORFAC           100
#define   SCREEN_RESET            255

// screens SM_DECEASE_MILK_INFO ------------------------------------------------
#define   SCREEN_DISEASE_0        0
#define   SCREEN_DISEASE_1        1
#define   SCREEN_DISEASE_2        2
#define   SCREEN_DISEASE_3        3
#define   SCREEN_DISEASE_4        4
#define   SCREEN_DISEASE_5        5
#define   SCREEN_DISEASE_6        6
#define   SCREEN_DISEASE_7        7
#define   SCREEN_DISEASE_8        8
#define   SCREEN_DISEASE_9        9
#define   SCREEN_DISEASE_10      10
#define   SCREEN_DISEASE_11      11
#define   SCREEN_DISEASE_12      12
#define   SCREEN_DISEASE_13      13
#define   SCREEN_DISEASE_14      14
#define   SCREEN_DISEASE_15      15
#define   SCREEN_DISEASE_MAX     16

// screens SM_ACT_DECEASE_INFO  ------------------------------------------------
#define   SCREEN_CONDUCT_RF       0
#define   SCREEN_CONDUCT_RR       1
#define   SCREEN_CONDUCT_LR       2
#define   SCREEN_CONDUCT_LF       3
#define   SCREEN_TEMP             4
#define   SCREEN_ACT_DISEASE_MAX  5

// screens SM_CAL_TEMP_INFO  ---------------------------------------------------
#define   SCREEN_CAL_TEMP_1       0
#define   SCREEN_CAL_TEMP_2       1
#define   SCREEN_CAL_TEMP_3       2
#define   SCREEN_CAL_TEMP_4       3
#define   SCREEN_CAL_TEMP_MAX     4

// screens SM_COW_INFO  --------------------------------------------------------
#define   SCREEN_COW_INFO_0       0
#define   SCREEN_COW_INFO_1       1
#define   SCREEN_COW_INFO_2       2
#define   SCREEN_COW_INFO_3       3
#define   SCREEN_COW_INFO_4       4
#define   SCREEN_COW_INFO_MAX     5

// screens SM_MILK_INFO --------------------------------------------------------
#define   SCREEN_MILK_INFO_0      0
#define   SCREEN_MILK_INFO_1      1
#define   SCREEN_MILK_INFO_2      2
#define   SCREEN_MILK_INFO_3      3
#define   SCREEN_MILK_INFO_4      4
#define   SCREEN_MILK_INFO_5      5
#define   SCREEN_MILK_INFO_6      6
#define   SCREEN_MILK_INFO_7      7
#define   SCREEN_MILK_INFO_MAX    8

// screens SM_FEED_INFO --------------------------------------------------------
#define   SCREEN_FEED_INFO_0      0
#define   SCREEN_FEED_INFO_1      1
#define   SCREEN_FEED_INFO_2      2
#define   SCREEN_FEED_INFO_3      3
#define   SCREEN_FEED_INFO_4      4
#define   SCREEN_FEED_INFO_MAX    5

// screens SM_CALENDAR_INFO ----------------------------------------------------
#define   SCREEN_CALENDAR_INFO_0      0
#define   SCREEN_CALENDAR_INFO_1      1
#define   SCREEN_CALENDAR_INFO_2      2
#define   SCREEN_CALENDAR_INFO_3      3
#define   SCREEN_CALENDAR_INFO_4      4
#define   SCREEN_CALENDAR_INFO_MAX    5

// screens SM_PULSE_INFO -------------------------------------------------------
#define   SCREEN_PULSE_INFO_0     0
#define   SCREEN_PULSE_INFO_1     1
#define   SCREEN_PULSE_INFO_MAX   2

// screens SM_ERROR_INFO -------------------------------------------------------
#define   SCREEN_ERROR_INFO_0     0
#define   SCREEN_ERROR_INFO_1     1
#define   SCREEN_ERROR_INFO_MAX   2

#if ( _EMC_TEST_ )
// screens SM_EMC_TEST ---------------------------------------------------------
#define   SCREEN_EMC_TEST_0     0
#define   SCREEN_EMC_TEST_1     1
#define   SCREEN_EMC_TEST_2     2
#define   SCREEN_EMC_TEST_3     3
#define   SCREEN_EMC_TEST_4     4
#define   SCREEN_EMC_TEST_MAX   5
#endif

#define   DSP_POINT_1           0x0001
#define   DSP_POINT_2           0x0002
#define   DSP_POINT_3           0x0004
#define   DSP_POINT_4           0x0008
#define   DSP_POINT_5           0x0010
#define   DSP_POINT_6           0x0020
#define   DSP_POINT_7           0x0040
#define   DSP_POINT_8           0x0080
#define   DSP_POINT_9           0x0100
#define   DSP_POINT_10          0x0200


#define MAX_NR_OF_SEP_RES  5

extern unsigned char  ScreenMode;
extern unsigned char  ScreenModeOld;

extern unsigned short MsgDispTime;
extern unsigned short OtherDisplayTime;
extern BOOLEAN        DisplayOtherSide;

extern unsigned char  disease_number;
extern unsigned char  nr_of_disease_items;
extern unsigned char  disease_trt_number;
extern unsigned char  nr_of_disease_trt_items[MAX_NODES];
extern unsigned char  sub_screen_pntr;
extern unsigned char  CowSepNrOfItems;
extern unsigned short CowFedTotal;
extern unsigned short CowFedBalance;
extern unsigned short CowFedParlTotalFed;

extern unsigned short ExtraFeedDpl;
extern   signed short ExtraFeed[MAX_NODES];

extern unsigned short CowGenHeatDate[MAX_NODES];
extern unsigned short CowGenHeatDateDpl;
extern unsigned short CowGenInsemDate[MAX_NODES];
extern unsigned short CowGenInsemDateDpl;
extern unsigned short CowGenDryOffDate[MAX_NODES];
extern unsigned short CowGenDryOffDateDpl;
extern BOOLEAN        CowGenReproState[MAX_NODES];

enum {
    COMM_ERR_IC08 = 1,
    COMM_ERR_IC09,
    COMM_ERR_IC13,
    COMM_ERR_VC4,
    KEY_MODE_ERR,
    KEY_HOLD_ERR,
    COMM_INIT_VC4
  };

extern unsigned char  CommunicationError;


extern void DispScreen(void);
extern unsigned char  ScreenNumberOld;
extern unsigned char ScreenNumber;
extern unsigned char MilkOnScreen;
extern unsigned short ScreenTime;
extern unsigned long OldDispValue;
extern void DispValue(unsigned char *Ptr, unsigned char Type,
            unsigned char Size, unsigned char NrDec,
            unsigned char Attrib, unsigned char Pos);
extern void DispClear(void);
extern void DispScreenFail(unsigned char ErrorNr);
extern void DispScreenCorFacLvl(void);
extern void DispScreenKeyTest(void);

#endif
