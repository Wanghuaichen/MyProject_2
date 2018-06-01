#ifndef TESTPROG_INCLUDED
#define TESTPROG_INCLUDED 1

// Define HP test or not
#define _HP_TEST_	1

#define HP_ROM_TEST      0x1F10
#define HP_RAM_TEST      0x1F11
#define HP_EEPROM_TEST   0x1F12
// Conductivity Test
#define HP_COND__LV_TEST 0x1F30
#define HP_COND__LA_TEST 0x1F31
#define HP_COND__RA_TEST 0x1F32
#define HP_COND__RV_TEST 0x1F33
// Temperature Test
#define HP_TEMP_TEST     0x1F40
#define HP_TEMP_REF1     0x1F41
#define HP_TEMP_REF2     0x1F42
#define HP_RTN_REF1      0x1F43
#define HP_RTN_REF2      0x1F44
#define HP_CURR_TEST     0x1F50
// Watchdog Test
#define HP_WD_TEST       0x1F60
//#define HP_RELAIS_0_TEST 0x2100       // default

#define HP_UART_TEST 0x40  
//#define HP_START_TEST 0x60  

/*** New definitions: 2-bytes commands from the HP Tester ***/

// I2C Command Test: Request firmware version from PIC-A
#define HP_REQUEST_PICA_VERSION						0x241A
// I2C Command Test: Request firmware version from PIC-B
#define HP_REQUEST_PICB_VERSION						0x241B
// I2C Command Test: Request firmware version from Conductivity Controller via PIC-A
#define HP_REQUEST_CONDUCTIVITY_VERSION				0x241C

// LED Display Segments Test via PIC-B
#define HP_DISPLAY_TEST								0x2501
// LED Test via PIC-B for 5 indication LEDs
#define HP_LED_TEST									0x2502
// Red Indication LAMP Test by PD5 (68HC11)
#define HP_LAMP_TEST 								0x2503

// Keyboard Test for 27 buttons via PIC-B
#define HP_KEY_COW_TEST								0x2601
#define HP_KEY_FEED_TEST							0x2602
#define HP_KEY_MILK_TEST							0x2603
#define HP_KEY_DECEAS_TEST							0x2604
#define HP_KEY_CALEND_TEST							0x2605
#define HP_KEY_SHIFT_TEST							0x2606
#define HP_KEY_1_TEST								0x2607
#define HP_KEY_2_TEST								0x2608
#define HP_KEY_3_TEST								0x2609
#define HP_KEY_4_TEST								0x260A
#define HP_KEY_5_TEST								0x260B
#define HP_KEY_6_TEST								0x260C
#define HP_KEY_7_TEST								0x260D
#define HP_KEY_8_TEST								0x260E
#define HP_KEY_9_TEST								0x260F
#define HP_KEY_CLEAR_TEST							0x2610
#define HP_KEY_0_TEST								0x2611
#define HP_KEY_ENTER_TEST							0x2612		// Lamp is enabled while key E is pressed
#define HP_KEY_ENTRAN_TEST							0x2613
#define HP_KEY_CENTRL_TEST							0x2614
#define HP_KEY_WATERTAP_TEST						0x2615		// Water Tap Symbol
#define HP_KEY_EXIT_TEST							0x2616
#define HP_KEY_STOP_TEST							0x2617
#define HP_KEY_MILK_V_TEST							0x2618
#define HP_KEY_CLUSTR_TEST							0x2619
#define HP_KEY_AUTO_TEST							0x261A
#define HP_KEY_MANUAL_TEST							0x261B

// Read input status for input test
// M1 ~ M3 on K3 connector
#define	HP_READ_INPUT_M1							0x2711		// M1 ---> PA0 (68HC11)
#define HP_READ_INPUT_M2							0x2712		// M2 ---> PA1 (68HC11)
#define HP_READ_INPUT_M3							0x2713		// M3 ---> PA2 (68HC11)
// I1 ~ I4 on K5 connector
#define HP_READ_INPUT_I1							0x2721		// I1 ---> PE3 (68HC11)
#define HP_READ_INPUT_I2							0x2722		// I2 ---> PE2 (68HC11)
#define HP_READ_INPUT_I3							0x2723		// I3 ---> PE1 (68HC11)
#define HP_READ_INPUT_I4							0x2724		// I4 ---> PE0 (68HC11)
// I5 on K4 connector
#define HP_READ_INPUT_I5							0x2725		// I5 ---> RXD2 ---> RA4 (PIC-A)

// Read analog input
// VSS input is on K4 connector
// To read VSS Voltage
#define HP_READ_ANALOG_INPUT_VSS					0x2801		// VSS ---> Current Sensing Circuit ---> PE4/AN4 (68HC11)
// To read Motor Current
#define HP_READ_ANALOG_INPUT_IMOTOR					0x2802		// Imotor -----------------------------> PE6/AN6 (68HC11)

// Set digital output
// P1 ~ P3 outputs are on K6 connector
#define HP_SET_OUTPUT_P1							0x2911		// PA3 (68HC11) ---> P1
#define HP_SET_OUTPUT_P2							0x2912		// PA4 (68HC11) ---> P2
#define HP_SET_OUTPUT_P3							0x2913		// PA5 (68HC11) ---> P3
// L output is on K13 connector for LAMP
#define HP_SET_OUTPUT_L								0x2914		// PD5 (68HC11) ---> L (LAMP)
// Reset digital output
// P1 ~ P3 outputs are on K6 connector
#define HP_RESET_OUTPUT_P1							0x2901		// PA3 (68HC11) ---> P1
#define HP_RESET_OUTPUT_P2							0x2902		// PA4 (68HC11) ---> P2
#define HP_RESET_OUTPUT_P3							0x2903		// PA5 (68HC11) ---> P3
// L output is on K13 connector for LAMP
#define HP_RESET_OUTPUT_L							0x2904		// PD5 (68HC11) ---> L (LAMP)

// Set Relay via PIC A
#define HP_SET_RELAY_1								0x2111		// RB5 (PIC-A) -> RB5a -----------------> Relay 1
#define HP_SET_RELAY_3								0x2113		// RB0 (PIC-A) -> RB0a ---> Inverter ---> Relay 3
#define HP_SET_RELAY_4								0x2114		// RB1 (PIC-A) -> RB1a ---> Inverter ---> Relay 4
#define HP_SET_RELAY_5								0x2115		// RB2 (PIC-A) -> RB2a ---> Inverter ---> Relay 5
#define HP_SET_RELAY_6								0x2116		// RB3 (PIC-A) -> RB3a ---> Inverter ---> Relay 6
#define HP_SET_RELAY_7								0x2117		// RB4 (PIC-A) -> RB4a ---> Inverter ---> Relay 7
// Reset Relay via PIC A
#define HP_RESET_RELAY_1							0x2101		// RB5 (PIC-A) -> RB5a -----------------> Relay 1
#define HP_RESET_RELAY_3							0x2103		// RB0 (PIC-A) -> RB0a ---> Inverter ---> Relay 3
#define HP_RESET_RELAY_4							0x2104		// RB1 (PIC-A) -> RB1a ---> Inverter ---> Relay 4
#define HP_RESET_RELAY_5							0x2105		// RB2 (PIC-A) -> RB2a ---> Inverter ---> Relay 5
#define HP_RESET_RELAY_6							0x2106		// RB3 (PIC-A) -> RB3a ---> Inverter ---> Relay 6
#define HP_RESET_RELAY_7							0x2107		// RB4 (PIC-A) -> RB4a ---> Inverter ---> Relay 7

// Motor Control Test
#define HP_MOTOR_START								0x22A1
#define	HP_MOTOR_STOP								0x22A2
#define HP_BRAKE_ENABLE								0x22B1
#define HP_BRAKE_DISABLE							0x22B2
#define HP_READ_MOTOR_HOME_FLAG						0x22C0		// READ PE7 (68HC11)

// 68HC11 MCU Response Code
#define TEST_PASSED									0xFF50		// Actual Test Succeeded
#define TEST_FAILED									0xFF51		// Actual Test Failed
#define TEST_ERROR									0xFF52		// General Error
#define COMMAND_INVALID								0xFF53
#define NO_PICA_COMMUNICATION						0xFF54
#define NO_PICB_COMMUNICATION						0xFF55


// function prototypes ---------------------------------------------------------

non_banked void HpTest(void);
non_banked unsigned char FTChecksumRom(unsigned char ProgChange);
non_banked unsigned char UartTest( void );

#endif
