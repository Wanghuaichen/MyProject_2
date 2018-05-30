#ifndef MANUFLOW_INCLUDED
#define MANUFLOW_INCLUDED 1



#define         REED_SENSOR       0x1


 
extern unsigned char ValveState;

extern void ManuflowMeasuring(void);
extern void HndlManuflowM1Int(void);
 

#endif
