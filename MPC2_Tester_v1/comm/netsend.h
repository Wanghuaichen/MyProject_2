#ifndef NETSEND_INCLUDED
#define NETSEND_INCLUDED 1

#include "../net/ext_net.h"

typedef struct{
  signed short Channel;
  signed short SrcAddress; 
  signed short DestAddress;
  NET_MESSAGE_TYPE Type;
  signed short Length;
  unsigned char Data[255];
} NET_SEND_MESSAGE;


extern NET_SEND_MESSAGE MsgBuffer;

// function prototype -------------------------------
void HndlNetSend(void);

#endif
