#ifndef EEPROM_INCLUDED
#define EEPROM_INCLUDED 1


#define ProgTime 20000

// function prototypes  --------------------------------------------------------

unsigned char WriteEEpromShort( unsigned short value, unsigned short *address);
unsigned char WriteEEpromByte( unsigned char value, unsigned char *address);
void EraseEEprom( void);
void ProgWait(void);


#endif
