#ifndef KEY_INT_INCLUDED
#define KEY_INT_INCLUDED        1


#define         KEY_SCAN_TIME   5000            /* 5000*0.5 us = 2.5 ms             */

extern unsigned short Cnt10msec;

extern interrupt void ScanKey(void);

#endif
