#ifndef UART_INCLUDED
#define UART_INCLUDED 1

/**** UART utilities ****/






/* bits Uart Registers          */

/* Mode Register1               */

#define Mr1RxC 0x80
#define Mr1RxI 0x40
#define Mr1Err 0x20
#define Mr1Par 0x04


/* Mode Register2               */

#define Mr2TxC 0x20
#define Mr2Cts 0x10

/* Command Register             */

#define CrDTx 0x08
#define CrETx 0x04
#define CrDRx 0x02
#define CrERx 0x01

/*State Register                */

#define SrRBr 0x80
#define SrFEr 0x40
#define SrPEr 0x20
#define SrOEr 0x10
#define SrTxE 0x08
#define SrTxR 0x04
#define SrFul 0x02
#define SrRxR 0x01

/* Interrupt Status Register    */

#define ISrRxR 0x04
#define ISrTxR 0x01
#define ISrTxE 0x02

/* Interrupt Mask Register      */

#define IMrRxR 0x04
#define IMrTxR 0x01
#define IMrTxE 0x02


#define SerOut 0x08
#define ETx 0x04
#define DTx 0x08
#define TxRdy 0x04
#define TxEmt 0x08
#define RcCh 0x04
#define RecErr 0x70
#define BufPMax 8
#define RErr 0x40

/* UartState    */

#define STORE 0x01
#define RnrNew 0x02



/* CSR  masks ----------------------------------*/
#define SccBaud1200             0x66
#define SccBaud2400             0x88
#define SccBaud4800             0x99
#define SccBaud9600             0xBB

/* MR 1 masks ----------------------------------*/
#define Sccbits_8               0x03
#define Sccbits_7               0x02
#define SccEvenPar              0x00
#define SccOddPar               0x04
#define SccForcePar             0x08
#define SccWithPar              0x00
#define SccNoPar                0x10


/* MR 2 masks ----------------------------------*/
#define SccStopbit_0            0x00
#define SccStopbit_1            0x07
#define SccStopbit_2            0x0F


/* CR   masks ----------------------------------*/
#define SccStopAll              0x9A
#define SccResetRegPntr         0x10
#define SccResetTx              0x30
#define SccResetRx              0x20


/* ACR  masks ----------------------------------*/
#define SccPowerDownOff         0x08
#define SccTimerMPI_PIN         0x40
#define SccTxC_1                0x02


/* IMR  masks ----------------------------------*/
#define SccNoInts               0x00
#define RxRdyFFull              0x04






/* UartRegisters                */

#define MR1  (*(unsigned char *) 0x0078)
#define MR2  (*(unsigned char *) 0x0078)
#define SR   (*(unsigned char *) 0x0079)
#define CSR  (*(unsigned char *) 0x0079)
#define CREG (*(unsigned char *) 0x007A)
#define RHR  (*(unsigned char *) 0x007B)
#define THR  (*(unsigned char *) 0x007B)
#define ACR  (*(unsigned char *) 0x007C)
#define ISR  (*(unsigned char *) 0x007D)
#define IMR  (*(unsigned char *) 0x007D)
#define CTU  (*(unsigned char *) 0x007E)
#define CTUR (*(unsigned char *) 0x007E)
#define CTL  (*(unsigned char *) 0x007F)
#define CTLR (*(unsigned char *) 0x007F)

//SccRxState
#define RECEIV                  0x1
#define RECEIV_FAIL             0x2
#define RECEIV_READY            0x4

//SccTxStatu
#define TRANSMITTING            0x1
#define TRANSMITTING_FAIL       0x2
#define TRANSMITTING_READY      0x4



extern non_banked void InitUart(void);
extern non_banked void SccRx(void);
extern void SccRxBank(void);
extern non_banked void SccTx(void);
extern void SccTxBank(void);

extern unsigned char SccUartIntMaskStatus;
extern unsigned char SccRxDataByte;
extern unsigned char SccRxState;
extern unsigned char SccTxState;


extern unsigned char *Ptr2SccRecBytes;
extern unsigned char *Ptr2SccRecBytesEnd;
extern unsigned char *Ptr2SccSndBytes;
extern unsigned char *Ptr2SccSndBytesEnd;
#endif
