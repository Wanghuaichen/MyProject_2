#ifndef MEMOLAC_INCLUDED
#define MEMOLAC_INCLUDED 1



#define         BOTTOM_REED_SENSOR       0x1
#define         MIDDLE_REED_SENSOR       0x2
#define         TOP_REED_SENSOR          0x4
#define         CALC_BOTTOM_REED_SENSOR  0x8
#define         CALC_MIDDLE_REED_SENSOR  0x10

#define         EMPTY_MILK_METER 0
#define         FILL_MILK_METER  1
 
extern unsigned short StartCleanPhase1, StartCleanPhase2, StartCleanPhase4, ClnPhsStartTime;
extern unsigned char ValveState, SensorState;

extern void HndlMemoCP_Controlled(void);
extern void HndlMemoCP_Controlled_1(void); 
extern void HndlMemoCP_Controlled_Valve(void); 
extern void HndlMemoCP_Controlled_0_1(void);
extern void HndlMemoCP_ControlledStart(void); 
extern void HndlMemoCP_PreClean(void);
extern void HndlMemoCP_End(void);
extern void HndlMemoCP_2(void);
extern void HndlMemoCP_CleanState(void);
extern void HndlMemoCP_1_STOP(void);
extern void HndlMemoCP_0(void);
extern void HndlMemoCPStart(void);

extern void MemolacMeasuring(void);
extern void HndlMemolacM1Int(void);
extern void HndlMemolacM2Int(void);
extern void HndlMemolacM3Int(void);
 

#endif
