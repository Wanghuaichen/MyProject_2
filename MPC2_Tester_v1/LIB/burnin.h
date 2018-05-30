#ifndef BURNIN_INCLUDED
#define BURNIN_INCLUDED 1
 
#define BURN_IN             0x1
#define HP_TEST             0x2


typedef struct { 
  unsigned char Ready;
  unsigned char RomResult;     
  unsigned char RamResult;         
  unsigned char VC3Result;        
  unsigned char PMResult;        
  unsigned char RelaisResult;    
  unsigned char UartResult;
  unsigned char DummyResult;      
  } BURN_IN_RESULT;

  
 typedef struct { 
  unsigned short NrOfTests;
  unsigned short RomResult;     
  unsigned short RamResult;         
  unsigned short VC3Result;        
  unsigned short PMResult;        
  unsigned short RelaisResult;    
  unsigned short UartResult;
  unsigned short DummyResult;      
  } BURN_IN_TOT_RESULT;


/* function prototypes -----------------------------------------------*/

unsigned char GetBurnInAddress(void);
//unsigned char CheckForTestConfig(void);
void BurnInProg(void); 

extern unsigned char TestMode, TestCommand;
extern BURN_IN_TOT_RESULT TotalTestResult; 
extern BURN_IN_RESULT TestResult; 

#endif
