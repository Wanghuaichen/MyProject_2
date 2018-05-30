#ifndef TEMPERATURE_INCLUDED
#define TEMPERATURE_INCLUDED 1

#define TMP_MEASURE_MILK 1
#define TMP_DISP_SERVICE 2

typedef struct { 
  unsigned short TMErrors;
  unsigned char  TMErrCode;
} TM_ERROR_INFO;              

extern TM_ERROR_INFO    TMErrorInfo;
extern unsigned short   CowTemperature;
extern unsigned short   MaxCowTemperature;
extern unsigned short   ActTemperature[10];
extern unsigned char    ActUadc;
extern unsigned char    CowTempAttention;

extern unsigned char  GetTemperature( unsigned char command, short *temperature, short *claw_temperature );
extern           void HndlTempMeasuring(void);
extern           void InitTemperature(void);

#endif
