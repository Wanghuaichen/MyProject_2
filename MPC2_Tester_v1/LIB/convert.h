#ifndef CONVERT_INCLUDED
#define CONVERT_INCLUDED 1

#include "../lib/boolean.h"
#include "../lib/item.h"
#include "../key/disp.h"


#define POS_RIGHT   0
#define POS_LEFT    1
#define POS_2       2
#define POS_3       3
#define POS_4       4
#define POS_5       5
#define POS_6       6
#define POS_7       7
#define POS_8       8
#define POS_9       9
#define POS_10     10


extern unsigned char  DispTable[11];
extern unsigned char  iic_disp[11];
extern unsigned short DispFlash;
extern unsigned short disp_point;

extern void ItemToDisp(ITEM_INFO *p, unsigned char Pos);
extern unsigned long DispToItem(void);
extern void AsciiToDisp(unsigned char n, unsigned char start, unsigned char nmbr);
extern void BoolToDisp(BOOLEAN n, unsigned char start);
extern void CharToDisp(unsigned char n, unsigned char start, unsigned char nmbr);
extern void ShortToDisp(unsigned short n, unsigned char start, unsigned char nmbr);
extern void LongToDisp(unsigned long n, unsigned char start, unsigned char nmbr);
extern void IntToStr(unsigned long n, char *s, unsigned char start, unsigned char nmbr);
extern void DisplayMessage(unsigned char msg_nr);
extern void InitDisp(void);
extern void Display(void);

#endif

