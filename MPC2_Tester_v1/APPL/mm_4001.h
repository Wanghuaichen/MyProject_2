#ifndef MM_VP4001_INCLUDED
#define MM_VP4001_INCLUDED 1

#include "../include/romstnet.h"


extern void VP4001MMInit(void);
extern void VP4001MMDone(void);
extern void VP4001MMContinue(void);
extern void VP4001MMMeas(void);
extern void VP4001MMCleaning(void);
extern void VP4001MMDefault(void);

#if DEBUG_VP4001
void TestIdleVP4001(void);
void TestMeasureVP4001(void);
void InitMsgCntr(void);


extern unsigned char msgCntr;
#endif

void HandleMlkAmntRef(NETIO_MAIL *MailPtr);
unsigned char isMilkingActive(void);
#endif