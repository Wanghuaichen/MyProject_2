
#include  "net_def.h"

extern NetType Net;


#pragma codeseg(EEPAGE)


void NetIni(void)
{
  char          *P;
  int           i;

  P = (char *) &Net;

  for (i = (sizeof(Net) - 1) ; i >= 0 ; i--) {
    *P = (char)0 ;
    P++;
  }
}

