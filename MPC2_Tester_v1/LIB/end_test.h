#ifndef END_TEST_INCLUDED
#define END_TEST_INCLUDED 1


typedef struct _OneWireMessage {
  unsigned char Address;
  unsigned char MsgLength;
  unsigned char Message;
  struct {
    unsigned char Data;
  } Array[10];  
} OneWireMessage;

extern BOOLEAN EndTest;

void CheckForEndTest(void);

#endif
