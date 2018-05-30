#ifndef STARTNET_INCLUDED
#define STARTNET_INCLUDED 1


#define         ArbModeVCP      0


#define CHANNEL 1
#define BUFFERS 4
#define BUFSIZE 255


/* function prototypes -----------------------------------------------*/

void InitStartNet(void);
void InitNetw(void);


extern signed short PeripheralAddress;

#endif
