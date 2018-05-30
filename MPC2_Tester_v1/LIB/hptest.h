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
 


// function prototypes ---------------------------------------------------------

non_banked void HpTest(void);
non_banked unsigned char FTChecksumRom(unsigned char ProgChange);
non_banked unsigned char UartTest( void );

#endif
