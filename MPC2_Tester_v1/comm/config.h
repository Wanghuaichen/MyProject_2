#ifndef CONFIG_INCLUDED
#define CONFIG_INCLUDED 1

#define         MaxAddress      50

typedef struct {
                unsigned int PerGenProgCode;
                unsigned char PerGenFuncIdentification;
                unsigned char PerGenFuncFeeding;
                unsigned char PerGenFuncMilkMeasuring;
                unsigned char PerGenFuncAccess;
                unsigned char PerGenFuncSeparation;
                unsigned char PerGenFuncWeight;
                unsigned char PerGenFuncTemperature;
                unsigned char PerGenFuncConductivity;
                unsigned char PerGenFuncActivity;
                unsigned char PerGenFuncDrinking;
                unsigned char PerGenFuncMarking;
                unsigned char PerGenType;
                unsigned char PerGenDownLoad;
                } CONFIG_ROM_INFO;


extern const CONFIG_ROM_INFO PerConfigData;

void DummyConfig(void);
#endif
