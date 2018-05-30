#ifndef ROM_CSTARTUP_INCLUDED
#define ROM_CSTARTUP_INCLUDED 1


/* function prototypes -----------------------------------------------*/

#define  CheckSum_Warm                0x4100
#define  CheckSum_Comp_Warm           0x4104


#define  CheckSum_Appl                0xC000
#define  CheckSum_Comp_Appl           0xC004
#define  EE_Bank_Size_Appl            0xC008
#define  Start_EE_Bank_Appl           0xC00A
#define  Nr_EE_Banks_Appl             0xC00C
#define  Start_PROG_Bank_Appl         0xC00D
#define  Start_DBMS_Bank_Appl         0xC00E


#define  SCI_int                      0xC010
#define  SPI_int                      0xC013
#define  PAII_int                     0xC016
#define  PAOVI_int                    0xC019
#define  TOI_int                      0xC01C
#define  OC5I_int                     0xC01F
#define  OC4I_int                     0xC022
#define  OC3I_int                     0xC025
#define  OC2I_int                     0xC028
#define  OC1I_int                     0xC02B
#define  IC3I_int                     0xC02E
#define  IC2I_int                     0xC031
#define  IC1I_int                     0xC034
#define  RTII_int                     0xC037
#define  IRQ_int                      0xC03A
#define  XIRQ_int                     0xC03D
#define  SWI_int                      0xC040
#define  OPCI_int                     0xC043
#define  COPI_int                     0xC046
#define  CMFI_int                     0xC049

#define  Start_Appl                   0xC04C

#define  CheckSum_Func                0xC04E
#define  CheckSum_Comp_Func           0xC052

/* SKIPBEGIN */
extern non_banked void startup(void);
extern non_banked void EraseFlashProm(void);
/* SKIPEND */

#endif
