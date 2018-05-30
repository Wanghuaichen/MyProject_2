#ifndef SEND_MSG_INCLUDED
#define SEND_MSG_INCLUDED 1

/* net initialisatie  */

#define         ConLow          1
#define         ConHigh         2
#define         UnCnLow         3
#define         UnCnHigh        4
#define         TmCnLow         5
#define         TmCnHigh        6
#define         TmUnLow         7
#define         TmUnHigh        8
#define         LoopTest        9

#define         Cntrllr         0

#define         MESS_LEN(mess) (sizeof(unsigned short) + sizeof(mess))

/* function prototypes -----------------------------------------------*/

unsigned char SendMessage( unsigned short priority, unsigned short src_address, unsigned short dest_address, unsigned short message_length, unsigned char *message);

#endif
