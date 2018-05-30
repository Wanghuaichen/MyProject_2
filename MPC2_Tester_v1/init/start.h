#ifndef START_INCLUDED
#define START_INCLUDED 1


#include "../flowlib/peripheral_vcp_flow.h"

/* function prototypes -----------------------------------------------*/

#define         MAX_NR_OF_MILKMETERS_VC3        16
#define         MAX_NR_OF_MILKMETERS_VC4        48
#define         MAX_NODES                       2

void Start(void);
//void HndlPerGenNumberRefReq(signed short PerAddress);
//void HndlDldPerDbmsResReq(void);
void InitMilkmeters(void);

extern unsigned short PerGenNumber[MAX_NODES];
extern PerGenRef *p_PerGenRef[MAX_NODES];
extern non_banked void Start_Non_Banked(void);
#endif
