#ifndef IO6811_INCLUDED
#define IO6811_INCLUDED         1

/*                              IO6811.H

                This file #defines the register block addresses.

*/


#ifndef HC_REG_OFFSET
/* #define HC_REG_OFFSET 0x01000               Default for 68HC11 */
#define HC_REG_OFFSET 0x0B000            /* Not Default for 68HC11 */
#endif


#define PORTA (* (unsigned char *) (HC_REG_OFFSET + 0))
                /* I/O port A */

#define PIOC (* (unsigned char *) (HC_REG_OFFSET + 2))
                /* Parallel I/O Control Register */

#define PORTC (* (unsigned char *) (HC_REG_OFFSET + 3))

#define PORTB (* (unsigned char *) (HC_REG_OFFSET + 4))

#define PORTCL (* (unsigned char *) (HC_REG_OFFSET + 5))
                /* Alternate latched port C */

#define DDRC (* (unsigned char *) (HC_REG_OFFSET + 7))
                /* Data direction for port C */

#define PORTD (* (unsigned char *) (HC_REG_OFFSET + 8))

#define DDRD (* (unsigned char *) (HC_REG_OFFSET + 9))
                /* Data direction for port D */

#define PORTE (* (unsigned char *) (HC_REG_OFFSET + 0xA))

#define CFORC (* (unsigned char *) (HC_REG_OFFSET + 0xB))
                /* Compare force register */

#define OC1M (* (unsigned char *) (HC_REG_OFFSET + 0xC))
                /* OC1 action mask register */

#define OC1D (* (unsigned char *) (HC_REG_OFFSET + 0xD))
                /* OC1 action data register */


#define TCNT (* (unsigned int *) (HC_REG_OFFSET + 0xE))
                /* Timer counter register */

#define TIC1 (* (unsigned int *) (HC_REG_OFFSET + 0x10))
                /* Input capture 1 register */

#define TIC2 (* (unsigned int *) (HC_REG_OFFSET + 0x12))

#define TIC3 (* (unsigned int *) (HC_REG_OFFSET + 0x14))

#define TOC1 (* (unsigned int *) (HC_REG_OFFSET + 0x16))
                /* Output compare 1 register */

#define TOC2 (* (unsigned int *) (HC_REG_OFFSET + 0x18))

#define TOC3 (* (unsigned int *) (HC_REG_OFFSET + 0x1A))

#define TOC4 (* (unsigned int *) (HC_REG_OFFSET + 0x1C))

#define TOC5 (* (unsigned int *) (HC_REG_OFFSET + 0x1E))

#define TCTL1 (* (unsigned char *) (HC_REG_OFFSET + 0x20))
                /* Timer control register 1 */

#define TCTL2 (* (unsigned char *) (HC_REG_OFFSET + 0x21))

#define TMSK1 (* (unsigned char *) (HC_REG_OFFSET + 0x22))
                /*Main timer interrupt mask register 1 */

#define TFLG1 (* (unsigned char *) (HC_REG_OFFSET + 0x23))
                /* Main timer interrupt flag register 1 */

#define TMSK2 (* (unsigned char *) (HC_REG_OFFSET + 0x24))

#define TFLG2 (* (unsigned char *) (HC_REG_OFFSET + 0x25))

#define PACTL (* (unsigned char *) (HC_REG_OFFSET + 0x26))
                /* Pulse accumulator control register */

#define PACNT (* (unsigned char *) (HC_REG_OFFSET + 0x27))
                /* Pulse accumulator count register */

#define SPCR (* (unsigned char *) (HC_REG_OFFSET + 0x28))
                /* SPI control register */

#define SPSR (* (unsigned char *) (HC_REG_OFFSET + 0x29))
                /* SPI status register */

#define SPDR (* (unsigned char *) (HC_REG_OFFSET + 0x2A))
                /* SPI data in/out */

#define BAUD (* (unsigned char *) (HC_REG_OFFSET + 0x2B))
                /* SCI baud rate control */

#define SCCR1 (* (unsigned char *) (HC_REG_OFFSET + 0x2C))
                /* SCI control register */

#define SCCR2 (* (unsigned char *) (HC_REG_OFFSET + 0x2D))

#define SCSR (* (unsigned char *) (HC_REG_OFFSET + 0x2E))
                /* SCI status register */

#define SCDR (* (unsigned char *) (HC_REG_OFFSET + 0x2F))
                /* SCI data (read RDR, write TDR) */

#define ADCTL (* (unsigned char *) (HC_REG_OFFSET + 0x30))
                /* A to D control register */

#define ADR1 (* (unsigned char *) (HC_REG_OFFSET + 0x31))
                /* A to D result register 1 */

#define ADR2 (* (unsigned char *) (HC_REG_OFFSET + 0x32))

#define ADR3 (* (unsigned char *) (HC_REG_OFFSET + 0x33))

#define ADR4 (* (unsigned char *) (HC_REG_OFFSET + 0x34))

#define BPROT (* (unsigned char *) (HC_REG_OFFSET + 0x35))

#define OPTION (* (unsigned char *) (HC_REG_OFFSET + 0x39))
                /* System configuration options */

#define COPRST (* (unsigned char *) (HC_REG_OFFSET + 0x3A))
                /* Arm/reset COP timer circuitry */

#define PPROG (* (unsigned char *) (HC_REG_OFFSET + 0x3B))
                /* EEPROM programming control register */

#define HPRIO (* (unsigned char *) (HC_REG_OFFSET + 0x3C))
                /* Highest priority I-bit interrupt and misc. */


/* #define INIT (* (unsigned char *) (HC_REG_OFFSET + 0x3D))  */
                /* RAM -I/O maping register */


#define INIT (* (unsigned char *) (0x103D))
                /* RAM -I/O maping register */

#define TEST1 (* (unsigned char *) (HC_REG_OFFSET + 0x3E))
                /* Factory TEST control register */

#define CONFIG (* (unsigned char *) (HC_REG_OFFSET + 0x3F))
                /* COP, ROM & EEPROM enables (EEPROM cells) */


#endif
