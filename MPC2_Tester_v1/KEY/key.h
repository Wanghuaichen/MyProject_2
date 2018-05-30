#ifndef KEY_INCLUDED
#define KEY_INCLUDED 1

#include "../lib/boolean.h"
#include "../lib/item.h"


extern ITEM_INFO   **DefTable;
extern unsigned char KeyExtSwitch;
extern unsigned char  nr_of_cal_feed_portions;

#define KEY_SHIFT_0					 0x30+0x80
#define KEY_SHIFT_1					 0x31+0x80
#define KEY_SHIFT_2					 0x32+0x80
#define KEY_SHIFT_3					 0x33+0x80
#define KEY_SHIFT_4					 0x34+0x80
#define KEY_SHIFT_5					 0x35+0x80
#define KEY_SHIFT_6					 0x36+0x80
#define KEY_SHIFT_7					 0x37+0x80
#define KEY_SHIFT_8					 0x38+0x80
#define KEY_SHIFT_9					 0x39+0x80

// definitie toetsen keyboard ---------------------------------------------
/*
#define KEY_CLEAR                    0x40
#define KEY_SHIFT_CLEAR              0xc0       // dmv shift
#define KEY_ENTER                    0x41
#define KEY_DISEASE_INFO             0x42
#define KEY_SHIFT_DISEASE_INFO       0xC2       // dmv shift
#define KEY_COW_INFO                 0x43
#define KEY_FEED_INFO                0x44
#define KEY_MILK_INFO                0x45
#define KEY_SHIFT_MILK_INFO          0xc5            // dmv shift
#define KEY_SHIFT                    0x46
#define KEY_K3_GATE                  0x47
#define KEY_SHIFT_K3_GATE            0xc7            // dmv shift
#define KEY_WASH                     0x48
#define KEY_SHIFT_WASH               0xc8            // dmv shift
#define KEY_K4_GATE                  0x49
#define KEY_SHIFT_K4_GATE            0xc9       // dmv shift
#define KEY_K5_GATE                  0x50
#define KEY_SHIFT_K5_GATE            0xd0       // dmv shift
#define KEY_START_PULSE              0xd1            // dmv shift
#define KEY_STOP_PULSE               0x51
#define KEY_RESET                    0xd3            // dmv shift
#define KEY_ACT_MILK_VALVE           0x53
#define KEY_MANUAL_REL_CLUSTER       0x56
#define KEY_SHIFT_MANUAL_REL_CLUSTER 0xd6            // dmv shift
#define KEY_CLUSTER_REMOVAL          0x57
#define KEY_SHIFT_CLUSTER_REMOVAL    0xd7       // dmv shift
#define KEY_AUTO_RELEASE             0x58
#define KEY_SHIFT_AUTO_RELEASE       0xd8            // dmv shift
#define KEY_CALENDAR_INFO            0x59
#define KEY_ERROR                    0x60
#define KEY_DUMMY                    0x61
#define KEY_REL                      0xff
*/

//                                      PIC-Offset+shift
#define KEY_CLEAR                    	0x40-0x3f
#define KEY_SHIFT_CLEAR              	0x40-0x3f+0x80
#define KEY_ENTER                    	0x41-0x3f
#define KEY_DISEASE_INFO             	0x42-0x3f
#define KEY_SHIFT_DISEASE_INFO       	0x42-0x3f+0x80
#define KEY_COW_INFO                 	0x43-0x3f
#define KEY_FEED_INFO                	0x44-0x3f
#define KEY_SHIFT_FEED_INFO           0x44-0x3f+0x80
#define KEY_MILK_INFO                	0x45-0x3f
#define KEY_SHIFT_MILK_INFO          	0x45-0x3f+0x80
#define KEY_SHIFT                    	0x46-0x3f
#define KEY_K3_GATE                  	0x47-0x3f
#define KEY_SHIFT_K3_GATE            	0x47-0x3f+0x80
#define KEY_WASH                     	0x48-0x3f
#define KEY_SHIFT_WASH               	0x48-0x3f+0x80
#define KEY_K4_GATE                  	0x49-0x3f
#define KEY_SHIFT_K4_GATE            	0x49-0x3f+0x80
#define KEY_K5_GATE                  	0x50-0x3f
#define KEY_SHIFT_K5_GATE            	0x50-0x3f+0x80
#define KEY_START_PULSE              	0x51-0x3f+0x80
#define KEY_STOP_PULSE               	0x51-0x3f
#define KEY_RESET                    	0x53-0x3f+0x80
#define KEY_ACT_MILK_VALVE           	0x53-0x3f
#define KEY_SHIFT_ACT_MILK_VALVE      0x53-0x3f+0x80
#define KEY_MANUAL_REL_CLUSTER       	0x56-0x3f
#define KEY_SHIFT_MANUAL_REL_CLUSTER 	0x56-0x3f+0x80
#define KEY_SHIFT_2_MANUAL_REL_CLUSTER 	0x56-0x3f+0x40
#define KEY_CLUSTER_REMOVAL          	0x57-0x3f
#define KEY_SHIFT_CLUSTER_REMOVAL    	0x57-0x3f+0x80
#define KEY_AUTO_RELEASE             	0x58-0x3f
#define KEY_SHIFT_AUTO_RELEASE       	0x58-0x3f+0x80
#define KEY_SHIFT_2_AUTO_RELEASE      0x58-0x3f+0x40
#define KEY_CALENDAR_INFO           	0x59-0x3f
#define KEY_ERROR                    	0x60					// rechtstreeks vergelijk in ReadKey
#define KEY_DUMMY                    	0x61-0x3f

// keys internally generated --------------------------
#define KEY_AUTO_RELEASE_FIRST_ATTACH_HOLD 0x61
#define KEY_AUTO_RELEASE_FIRST_ATTACH      0x62
#define KEY_AUTO_RELEASE_HOLD              0x63
#define KEY_AUTO_RELEASE_EXT_HOLD          0x64
#define KEY_AUTO_RELEASE_EXT         	     0x65     // external key tbv swing over
#define KEY_MANUAL_REL_CLUSTER_EXT   	     0x66
#define KEY_MANUAL_REL_CLUSTER_HOLD        0x67
#define KEY_REL                      	0xff-0x3f


// Key Mode --------------------------------------------------------------------
#define         KM_MILKING      0
#define         KM_CLEANING     1
#define         KM_SERVICE      2
#define         KM_DHIA         3
#define         KM_INPUT        4
#define         KM_CAL          5
#define         KM_ADDRESS      6
#define         KM_RBT_MILKING  7
#define         KM_BURN_IN      8
#define         KM_TEST         9
#define         KM_P_TEST      10
#define         KM_ERROR       11
#define         KM_DISP_MSG    12

#if ( _EMC_TEST_ )
#define   KM_EMC_TEST            13
#endif

extern unsigned short ExternMessages;

#define  SEND_CLEAN_START_MESSAGES           0x0001
#define  SEND_CLEAN_STOP_MESSAGES            0x0002
#define  SEND_SET_PULSATION_MESSAGES         0x0004
#define  SEND_STOP_PULSATION_MESSAGES        0x0008
#define  SEND_WASH_MODE_4_START_MESSAGES     0x0010
#define  SEND_PRE_WASH_START_MESSAGES		     0x0020
#define  SEND_WASH_MODE_3_START_MESSAGES     0x0040
#define  SEND_EXTRA_DUMP_MESSAGES            0x0080
#define  SEND_WASH_MODE_I2C_START_MESSAGES	 0x0100
#define  SEND_WASH_MODE_I2C_STOP_MESSAGES    0x0200

// defines die over het vc4-kanaal gestuurd worden
#define START_CLEAN_PROCESS               1
#define STOP_CLEAN_PROCESS                2
#define SET_PULSATION_MESSAGE             3
#define STOP_PULSATION_MESSAGE            4
#define SET_WASH_MODE_4                   5
#define SET_PRE_WASH_MODE				          6
#define SET_WASH_MODE_3                   7
#define EXTRA_DUMP                        8
#define START_I2C_WASH_MODE_MESSAGE				9
#define STOP_I2C_WASH_MODE_MESSAGE			 10

// Key State -------------------------------------------------------------------
#define KS_NORMAL             0
#define KS_CONTINUE        0x80
#define KS_CONTINUE_2	     0x40


extern unsigned char KeyState;

extern BOOLEAN       ModeChange;
extern unsigned char WaitForI2High;

extern unsigned char  KeyA;
extern unsigned char  KeyMode;
extern unsigned char  KeyModeOld;
extern unsigned char  KeyOld;
extern unsigned char  EditItem;
extern unsigned char  KeyValue;
extern unsigned char  KeyHold;
extern unsigned short KeyShiftTime;
extern unsigned short KeyHoldTime;

extern unsigned char FieldNr, FieldNrMax;



extern unsigned char  pt_result;

extern void InitKey(void);

extern void RestartNetwork(void);
extern non_banked void HandleKey_Non_Banked(void);
extern unsigned char   ReadKey(void);
extern void            HandleKey(void);
extern void            SetFactorySettings(void);
unsigned char          GetManualorAutoRelease(void);
#endif
