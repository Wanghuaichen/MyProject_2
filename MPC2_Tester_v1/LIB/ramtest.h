#ifndef RAM_INCLUDED
#define RAM_INCLUDED 1


#define RAM_TEST_SKIP           2
#define RAM_TEST_ERROR          1
#define RAM_TEST_OK             0


extern unsigned char RamTestStatus;

/* functie declaraties ------------------------------------------------------------------------*/
extern non_banked unsigned char RamTestAsm(unsigned char Option);

#endif
