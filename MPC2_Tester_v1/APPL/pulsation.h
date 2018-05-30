#ifndef PULSATION_INCLUDED
#define PULSATION_INCLUDED 1

#include "../lib/boolean.h"


extern unsigned char  PlsState;
extern unsigned short StimStartTime; 
extern BOOLEAN        FirstTimeStimulation;
extern BOOLEAN        ManualPulseToggle;  
extern unsigned char  KeyExtPulseControl;

#define PLS_DISABLE          0x00
#define PLS_ENABLE           0x01
#define PLS_MANUAL_ENABLE    0x02
#define PLS_HAPPEL_SYSTEM    0x04                        // geeft aan dat pulsatie moet starten indien I2 laag wordt
#define PLS_HAPPEL_PRE_PULSE 0x08

#define ENABLE_CORR         0x0001
#define ENABLE_STRIP_STIM   0x0002
#define ENABLE_STRIPPING    0x0004
#define ENABLE_STIMULATION  0x0008
#define ENABLE_TAKE_OFF     0x0010
#define ENABLE_PULSATION    0x0020

#define STANDARD_PULSE_MODE 0
#define STIMULATION_PULSE_MODE 1 

#define WASH_I2_COMM_CONTROLLED_AND_MASTER   (((EEI2Control&0x03)==0x02) & EERowSize)
#define WASH_I2_DEPENDANT_NO_MASTER           ((EEI2Control&0x03)==0x00)
#define WASH_I2_INDEPENDANT                   ((EEI2Control&0x07)==0x01)


typedef struct {
  unsigned short PTHighFront;         
  unsigned short PTLowFront;
  unsigned short PTHighBack;         
  unsigned short PTLowBack;
  unsigned short StimStartTime; 
    signed char  PlsFreqCorr;
    signed char  PlsRatCorr;
  BOOLEAN        FirstTimeStimulation;
  BOOLEAN        ManualPulseToggle;
  BOOLEAN        PlsMsgControlled;
} PLS_PULSE_CNTRL;

extern PLS_PULSE_CNTRL PLSCntrl;

enum {
  PLS_IDLE = 0,
  PLS_SET_PULSATION,
  PLS_INIT_PULSATION,
  PLS_WF_START_PULSATION,
  PLS_START_PULSATION,
  PLS_STANDARD_PULSATION,
  PLS_START_STIM_PULSATION,
  PLS_STIMULATION_PULSATION,
  PLS_START_TA_STIM_PULSATION,
  PLS_TA_STIM_PULSATION,
  PLS_END_PULSATION,
  PLS_STOP_PULSATION,
  PLS_PRE_IDLE
  };
       

// prototypes ------------------------------------------------------------------
unsigned char SetPulsationTimes( unsigned char mode );
void InitPulsation(void );
void start_pulsation( void );
void stop_pulsation( void );
void TogglePulseOutputs( void);
void HndlPulsation(void);
void CheckPulsation( void);
void PulseControl(void);
#endif
