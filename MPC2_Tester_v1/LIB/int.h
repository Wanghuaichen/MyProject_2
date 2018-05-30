#ifndef INTERRUPT_INCLUDED
#define INTERRUPT_INCLUDED 1

non_banked void Interrupts_Off(unsigned char *OldValue);
non_banked void Interrupts_On(unsigned char *OldValue);



#endif
