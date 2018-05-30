/*			 - intr6811.h -

   Intrinsic functions for the ICC6811
	   
   Version: 4.00 [IHAT]

*/

#ifndef _INTRINS_INCLUDED
#define _INTRINS_INCLUDED

#pragma language=extended

#if __TID__ & 0x8000
#pragma function=intrinsic(0)
#endif


void enable_interrupt(void);
void disable_interrupt(void);
void stop_CPU(void);
void wait_for_interrupt(void);
void software_interrupt(void);
void _opc(unsigned char);

#if __TID__ & 0x8000
#pragma function=default
#endif

#endif
