#ifndef ROM_EEHC11_INCLUDED
#define ROM_EEHC11_INCLUDED 1

#pragma memory=dataseg(EEDATA1) 
 
extern unsigned char  EEAddress;
extern unsigned char  EEComProt;
extern unsigned char  EEDDBPresent;
extern unsigned char  EEPROGPresent;
extern unsigned short EEBankSize;
extern unsigned short EEBankStart;
extern unsigned char  EENrOfBanks;
extern unsigned char  EEBankPROG;
extern unsigned char  EEBankDBMS;
extern unsigned char  EEMilkMeterType;
extern unsigned char  EEConMeasuring;
extern unsigned char  EETempMeasuring;
extern unsigned char  EEFeeding;
 
#pragma memory=default

#endif
