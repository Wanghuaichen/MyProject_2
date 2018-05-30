#ifndef NIVEAU_INCLUDED
#define NIVEAU_INCLUDED 1

//Milk Mode
#define         MM_INIT         0
#define         MM_MEAS         1
#define         MM_CAL          2
#define         MM_DHIA         3

//Calibration States

#define         CAL_START       0
#define         CAL_15LTR       1
#define         CAL_5LTR        2
#define         CAL_0LTR        3
#define         CAL_VAL1        4
#define         CAL_VAL2        5
#define         CAL_RDY         6
 

extern void NiveauMeasuring(void);
extern void NiveauTakeOff(void);
extern void HndlMeasurePulseWidth(void);

extern unsigned short PulsWidthMeas, PulsWidthCor,PulsWidthAve; 
extern unsigned char CalState;

#endif 


