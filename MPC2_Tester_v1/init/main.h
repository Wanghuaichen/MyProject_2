#ifndef MAIN_INCLUDED
#define MAIN_INCLUDED 1

extern unsigned char IIC_ERR;

/* function prototypes -----------------------------------------------*/

non_banked void Main(void);
void RequestPerGenRef(void);
non_banked unsigned char CheckDDB(void);

#endif
