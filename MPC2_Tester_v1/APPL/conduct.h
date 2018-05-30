#ifndef CONDUCTIVITY_INCLUDED
#define CONDUCTIVITY_INCLUDED 1

#define CM_MEASURE_ENABLE  0x01
#define CM_LONG_RECORD     0x02
#define CM_ONE_CHANNEL     0x04

#define CV_RV 1
#define CV_RA 2
#define CV_LA 3
#define CV_LV 4


typedef struct {
  unsigned char RV;
  unsigned char RA;
  unsigned char LA;
  unsigned char LV;
} CONDUCTIVITY_INFO; 

extern CONDUCTIVITY_INFO ConductivityBuffer[144];


typedef struct { 
  unsigned char  HVC_Values[40];
  unsigned char  average;
  unsigned char  r_average;
} HVC_BUFFER_INFO;              

typedef struct { 
  unsigned short CMErrors;
  unsigned char  CMErrCode;
} CM_ERROR_INFO;              

typedef struct{
  unsigned short h_cntr;
  unsigned short l_cntr;
} TAKE_OFF_SENSOR;

extern TAKE_OFF_SENSOR TakeOffSensor;
extern CM_ERROR_INFO   CMErrorInfo;
extern HVC_BUFFER_INFO HVC_Buffer[4];
extern unsigned char   ConDataAttentions;
extern unsigned short  CBWptr;

unsigned char GetConductivityValues( unsigned char *buf);
void          ConConMpkMpc( unsigned char *cv );
void          HndlConductMeasuring( unsigned char side );
void          InitConductivity(void);

#endif
