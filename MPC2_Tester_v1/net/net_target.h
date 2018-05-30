#ifndef NET_TARGET_INCLUDED
#define NET_TARGET_INCLUDED 1


/*******************************************************************/
/*                                                                 */
/*    AMICE 4.0  -  68HC11                                         */
/*                                                                 */
/*    Target specific I/O and timer routines                       */
/*                                                                 */
/*******************************************************************/

#include   "../lib/io6811.h"
/*nclude   <io6811.h>   */

/**** Timer utilities ****/

#define Enable_Timer_Interrupts  (TimerIntMaskStatus = 1)
/*         (TMSK1 |= 0x08)      */

#define Disable_Timer_Interrupts (TimerIntMaskStatus = 0)
/*         (TMSK1 &= 0xF7)      */

#define   StartTimeout(Time)  \
           (Net.TimerInit = Time);\
           TOC5 = TCNT + (unsigned int)Time; \
           TFLG1 = 0x08;  \
           Enable_Timer_Interrupts;\
         /* WARNING: may not be interrupted */

#define  ReloadTimeout()  \
           (TOC5 = TCNT + Net.TimerInit)


#define  StopTimeout()  \
          Disable_Timer_Interrupts;\
         /* WARNING: may not be interrupted */


#define  StopTimeoutReal()    (TMSK1 &= 0xF7);         \
                              (TimerIntMaskStatus = 0)

/**** UART utilities ****/

#define Enable_Uart_Interrupts   (UartIntMaskStatus = 1)
#define Disable_Uart_Interrupts  (UartIntMaskStatus = 0)

#define  ReadUart()         (SCDR)

#define  WriteUart(Byte)    (SCDR = Byte)

#define  WakeUpModeOn()     (UartIntMaskStatus |= 0x02)      /* bit RWU = 1 */
#define  WakeUpModeOnReal()  (SCCR2 |= 0x02) ; WakeUpModeOn()     /* bit RWU = 1 */

#define  WakeUpModeOff()    (UartIntMaskStatus &= 0xFD)      /* bit RWU = 0 */
#define  WakeUpModeOffReal()  (SCCR2 &= 0xFD); WakeUpModeOff()       /* bit RWU = 0 */

#define  RxUartError()      ((SCSR) & 0x0E)     /* Overrrun, noise and frame-error */

#define  Rx9thBit()         ((SCCR1) & 0x80)     /* bit R8 */

#define  SetBit9()          (SCCR1 |= 0x40)     /* bit T8 = 1 */

#define  ClrBit9()          (SCCR1 &= 0xBF)     /* bit T8 = 0 */

#define  DisableTx()        (UartIntMaskStatus &= 0x3F)     /* bit TIE = 0 , TCIE = 0 */

#define  EnableTx()         (UartIntMaskStatus |= 0x80)     /* bit TIE = 1 */

#define  EnableTxE()        (UartIntMaskStatus |= 0x40)     /* bit TCIE = 1 */

#define  DisableRx()        (UartIntMaskStatus &= 0xDF)     /* bit RIE = 0 */

#define  EnableRx()         (UartIntMaskStatus |= 0x20)     /* bit RIE = 1 */

#define  UartTxEmpty()      ((SCSR) & 0x80)      /* bit TDRE */

#define  UartDis()          (UartIntMaskStatus &= 0x1F)     /* bit TIE = RIE = 0 */

#define  UartDisReal()        (SCCR2             &= 0x1F) ;  \
                              (UartIntMaskStatus &= 0x1F)      /* bit TIE = RIE = 0 */

#define  UartEnb()          (UartIntMaskStatus |= 0x02); \
                            (UartIntMaskStatus |= 0x2C)      /* WU on  */  /* bit RIE = TIE = RE = 1 */

#define  UartEnbReal()      (SCCR2 |= 0x02); \
                            (SCCR2 |= 0x2C); \
                            UartEnb()

#define  EmptyRxUart()      (SCSR);  \
                            (Byte = SCDR) ; \
                            (SCSR);  \
                            (Byte = SCDR)     
                            
#endif                             
