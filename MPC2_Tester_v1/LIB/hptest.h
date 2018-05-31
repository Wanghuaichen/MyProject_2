#ifndef TESTPROG_INCLUDED
#define TESTPROG_INCLUDED 1
 

#define HP_ROM_TEST      0x1F10
#define HP_RAM_TEST      0x1F11
#define HP_EEPROM_TEST   0x1F12
#define HP_COND__LV_TEST 0x1F30
#define HP_COND__LA_TEST 0x1F31
#define HP_COND__RA_TEST 0x1F32
#define HP_COND__RV_TEST 0x1F33
#define HP_TEMP_TEST     0x1F40
#define HP_TEMP_REF1     0x1F41
#define HP_TEMP_REF2     0x1F42
#define HP_RTN_REF1      0x1F43
#define HP_RTN_REF2      0x1F44
#define HP_CURR_TEST     0x1F50 
#define HP_WD_TEST       0x1F60
#define HP_RELAIS_0_TEST 0x2100       // default

#define HP_UART_TEST 0x40  
//#define HP_START_TEST 0x60  

/* New definitions: 2-bytes commands from the HP Tester */
// I2C Command Test: Request firmware version from PIC-A
#define HP_REQUEST_PICA_VERSION						0x241A
// I2C Command Test: Request firmware version from PIC-B
#define HP_REQUEST_PICB_VERSION						0x241B
// I2C Command Test: Request firmware version from Conductivity Controller via PIC-A
#define HP_REQUEST_CONDUCTIVITY_VERSION				0x241C
#define HP_DISPLAY_TEST								0x2501
#define HP_LED_TEST									0x2502
#define HP_LAMP_TEST 								0x2503
// Keyboard Test for 27 buttons
#define HP_KEY_COW_TEST								0x2601
#define HP_KEY_FEED_TEST								0x260
#define HP_KEY_MILK_TEST								0x260
#define HP_KEY_DECEAS_TEST								0x260
#define HP_KEY_CALEND_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260

#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260

#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260

#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260
#define HP_KEY_TEST								0x260



// 68HC11 MCU Response Code
#define TEST_PASSED					0xFF50		//Actual Test Succeeded
#define TEST_FAILED					0xFF51		//Actual Test Failed
#define TEST_ERROR					0xFF52		//General Error
#define COMMAND_INVALID				0xFF53
#define NO_PICA_COMMUNICATION		0xFF54
#define NO_PICB_COMMUNICATION		0xFF55


// function prototypes ---------------------------------------------------------

non_banked void HpTest(void);
non_banked unsigned char FTChecksumRom(unsigned char ProgChange);
non_banked unsigned char UartTest( void );

#endif
