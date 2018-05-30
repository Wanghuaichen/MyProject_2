#ifndef IIC_INCLUDED
#define IIC_INCLUDED            1

extern unsigned char DispLeds;
extern unsigned char DispBlinkLeds;

extern unsigned short IicWriteErrors;
extern unsigned short IicReadErrors;

// definitie van de diverse leds -----------------------------------------------
#define IIC_MILK_ATTENTION     0x01        // d28
#define IIC_ACTIVITY_ATTENTION 0x02        // d33
#define IIC_CALENDAR_ATTENTION 0x04        // d34
#define IIC_MILK_SEP_ATTENTION 0x08        // d36
#define IIC_CONDUCT_ATTENTION  0x10        // d35

// Iic-address plus de diverse commando's --------------------------------------
#define KB_ADDRESS            0x60

#define IIC_GET_PROG_OPTIONS      28              // 1 byte
#define IIC_GET_PROG_VERSION_IC13 27              // 1 byte
#define IIC_WRITE_BLINK_TIMES     26              // 2 bytes
#define IIC_WRITE_LED_BLINK_INFO  25              // 1 byte
#define IIC_WRITE_POINT_INFO      24              // 2 bytes
#define IIC_WRITE_LED_INFO        23              // 1 byte
#define IIC_WRITE_FLASH_INFO      21              // 2 bytes
#define IIC_GET_KEY               20              // 1 byte
#define IIC_GET_DISPLAY_VALUES    10              // 10 bytes
#define IIC_DISPLAY_TEXT           0              // 10 bytes

// Iic-address plus de diverse commando's --------------------------------------
#define PICA_ADDRESS            0x50

#define IIC_READ_COND              0
#define IIC_READ_TEMP              4
#define IIC_ONE_WIRE_WRITE         5
#define IIC_ONE_WIRE_READ          6
#define IIC_WRITE_PORTB            8
#define IIC_READ_PORTB             8
#define IIC_READ_SCHANNEL         10
#define IIC_WRITE_PORTC         0x10
#define IIC_READ_PORTC          0x10
#define IIC_WRITE_MCSG          0x12
#define IIC_GET_PROG_VERSION_IC8  13

         void InitIic(void);
unsigned char IicStop( void);
unsigned char ReadIic(unsigned char DeviceAddress, unsigned char RegAddress, unsigned char nr_of_bytes, unsigned char *DestinationAddress);
unsigned char WriteIic(unsigned char DeviceAddress, unsigned char RegAddress, unsigned char nr_of_bytes, unsigned char *SourceAddress);

extern unsigned char HandleWriteIic( unsigned char DeviceAddress, unsigned char RegAddress,
                                     unsigned char nr_of_bytes, unsigned char *SourceAddress);
extern unsigned char HandleReadIic(unsigned char DeviceAddress, unsigned char RegAddress,
                                   unsigned char nr_of_bytes, unsigned char *DestinationAddress);

#endif
