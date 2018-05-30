#ifndef FAKE_USX_INCLUDED
#define FAKE_USX_INCLUDED 1


#define MaxNrOfResources 10

#define IIC_RES           4
#define NET_BUF_RES       5

void InitUsx(void);
short TransformResNr(short ResNr);
short UsxGetRes(short ResNr);
short UsxSltTsk(void (*TaskName)(void));
unsigned long UsxGetAbsTime(void);
short UsxRelRes(short ResNr);
short UsxReqMem(short NrOfBytes, short type_requested);
short UsxRelMem(void *Addresspointer);
void SUSPEND(void);
short GetRes(short ResNr, unsigned char test);
short RelRes(short ResNr);

#endif
