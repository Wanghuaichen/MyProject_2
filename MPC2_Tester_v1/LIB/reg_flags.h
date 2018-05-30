#ifndef REG_FLAGS_INCLUDED
#define REG_FLAGS_INCLUDED      1



/*
  PORT
*/
#define P_7   0x80            // Port output 7
#define P_6   0x40            // Port output 6
#define P_5   0x20            // Port output 5
#define P_4   0x10            // Port output 4
#define P_3   0x08            // Port output 3
#define P_2   0x04            // Port output 2
#define P_1   0x02            // Port output 1
#define P_0   0x01            // Port output 0

/*
  SCSR
*/
#define TDRE  0x80            /* Transmit Data Register Empty               */
#define TC    0x40            /* Transmit Compleet                          */
#define RDRF  0x20            /* Receiver Data Register Full                */
#define IdLe  0x10            /* Idle Line detect bit                       */
#define OR    0x08            /* Overrun Error                              */
#define NF    0x04            /* Noise Flag                                 */
#define FE    0x02            /* Framing Error                              */

/*
  SCCR
*/
#define TIE   0x80            /* Timer Interrupt Enable                     */
#define TCIE  0x40            /* Transmission Complete Interrupt Enable     */
#define RIE   0x20            /* Receive Interrupt Enable                   */
#define ILIE  0x10            /* Idle Line Interrupt Enable                 */
#define TE    0x08            /* Transmit Enable                            */
#define RE    0x04            /* Receive Enable                             */
#define RWU   0x02            /* Receiver Wake Up                           */
#define SBk   0x01            /* Send Break                                 */

/*
  SPCR
*/
#define SPIE  0x80            /* Serial Peripheral Interrupt Enable         */
#define SPE   0x40            /* Serial Peripheral System Enable            */
#define DWOM  0x20            /* Port D Wire-OR- MOde options               */
#define MSTR  0x10            /* Master MOde Select                         */
#define CPOL  0x08            /* Clock Polarity                             */
#define CPHA  0x04            /* Clock Phase                                */
#define SPR1  0x02            /* SPI  Clock Rate Selects                    */
#define SPR0  0x01            /* SPI CLock Rate Selects                     */


#define MISO  0x04
#define MOSI  0x08
#define SCK   0x10
#define WCOL  0x40
#define SPIF  0x80


/*
  PACT
*/
#define DDRA7 0x80            /* Data Direction Register poort A            */
#define PAEn  0x40            /* Pulse Accumulator system Enable            */
#define PAMod 0x20            /* Pulse Accumulator Mode                     */
#define PEdge 0x10            /* Pulse Accumalator Edge Control             */
#define RTR1  0x02            /* RTI Interrupt Rate Select 1                */
#define RTR2  0x01            /* RTI Interrupt Rate Select 1                */

/*
  TMSK1
*/
#define OC1I  0x80            /* Output Compare 1 Interrupt                 */
#define OC2I  0x40            /* Output Compare 2 Interrupt                 */
#define OC3I  0x20            /* Output Compare 3 Interrupt                 */
#define OC4I  0x10            /* Output Compare 4 Interrupt                 */
#define OC5I  0x08            /* Output Compare 5 Interrupt                 */
#define IC1I  0x04            /* Input Capture 1 Interrupt                  */
#define IC2I  0x02            /* Input Capture 2 Interrupt                  */
#define IC3I  0x01            /* Input Capture 3 Interrupt                  */

/*
  TCTl2-bits
*/

#define EDG1B 0x20
#define EDG1A 0x10
#define EDG2B 0x08
#define EDG2A 0x04
#define EDG3B 0x02
#define EDG3A 0x01

/*
  TMSK2
*/
#define TOI   0x80            /* Timer Overflow Interrupt Enable            */
#define RTII  0x40            /* RTI Interrupt Enable                       */
#define PAOVI 0x20            /* Puls Accumulator Overflow Interrupt Enable   */
#define PAII  0x10            /* Pulse Accumulator Input Interrupt Enable   */
#define PR1   0x02            /* Timer Prescaler Selects                    */
#define PR0   0x01            /* Timer Prescaler Selects                    */

/*
  TFLG1
*/
#define OC1F  0x80            /* Output Compare 1 Flag                      */
#define OC2F  0x40            /* Output Compare 2 Flag                      */
#define OC3F  0x20            /* Output Compare 3 FLag                      */
#define OC4F  0x10            /* Output Compare 4 Flag                      */
#define OC5F  0x08            /* Output Compare 5 Flag                      */
#define IC1F  0x04            /* Input Capture 1 Flag                       */
#define IC2F  0x02            /* Input Capture 2 Flag                       */
#define IC3F  0x01            /* Input Capture 3 Flag                       */

/*
  TFLG2
*/
#define TOF   0x80            /* Timer Overflow FLag                        */
#define RTIF  0x40            /* Real Time Interrupt FLag                   */
#define PAOvF 0x20            /* Pulse Accumulator Overflow Interrupt Flag  */
#define PAIF  0x10            /* Pulse Accumulator Input Edge Interrupt Flag*/

/*
  ADCT
*/
#define CCF   0x80            /* Conversion Complete Flag                   */
#define SCAN  0x20            /* Continuous Scan Control                    */
#define MULT  0x10            /* Multiple-Channel/Single Channel Control    */
#define CD    0x08            /* Channel Select D                           */
#define CC    0x04            /* Channel Select C                           */
#define CB    0x02            /* Channel Select B                           */
#define CA    0x01            /* Channel Select A                           */

/*
  OPTION
*/
#define ADPU  0x80            /* A/D Powerup                                */
#define CSEL  0x40            /* A/D Clock Source Select                    */
#define IRQE  0x20            /* IRQ Edge/level Sensitive                   */
#define DLY   0x10            /* STOP Exit Turn_On Delay                    */
#define CME   0x08            /* Clock Monitor Enable                       */
#define CR1   0x02            /* COP Time Rate Selects                      */
#define CR0   0x01            /* COP Time Rate Selects                      */

/*
  PPro
*/
#define ODD   0x80            /* Used to Program Odd Rows (TEST)            */
#define EVEN  0x40            /* Used to Program Even Rows (TEST)           */
#define BYTE  0x10            /* Used for Erasing Bytes                     */
#define ROW   0x08            /* Used for Row Erasing                       */
#define ERASE 0x04            /* Enable the Erase                           */
#define EELAT 0x02            /* EEprom Latch Control                       */
#define EEPGM 0x01            /* Program Command                            */

/*
  HPRIO
*/
#define RBOOT 0x80            /* The Read Bootstrap ROM Bit                 */
#define SMOD  0x40            /* Special Mode write only Bit                */
#define MDA   0x20            /* The mode Select A Bit                      */
#define IRV   0x10            /* Internal Read Visibility Bit               */
#define PSEL3 0x08            /* Priority Select Bit 3                      */
#define PSEL2 0x04            /* Priority Select Bit 2                      */
#define PSEL1 0x02            /* Priority Select Bit 1                      */
#define PSEL0 0x01            /* Priority Select Bit 0                      */

/*
  CONFIG
*/
#define NOSEC 0x08            /* Security Mode Option Bit                   */
#define NOCOP 0x04            /* COP Sytem Off                              */
#define ROMON 0x02            /* Enable On-Chip ROM Select                  */
#define EEON  0x01            /* Enable EEprom On                           */

// BPROT
#define PTCON 0x10H
#define BPRT3 0x08H
#define BPRT2 0x04H
#define BPRT1 0x02H
#define BPRT0 0x01H

#endif
