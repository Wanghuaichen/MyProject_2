/***************************************************************************************/
/* fake_usx.c                                                                          */
/*                                                                                     */
/* simulation of some usx_functions is a non-usx environment                           */
/* author       L.A. Boulogne                                                          */
/* date         19-12-91                                                               */
/***************************************************************************************/

#include "fake_usx.h"
#include "../usx/resource.h"
#include "../net/net_def.h"
#include "../lib/intr6811.h"   
#include "../lib/rti_int.h"   
#include "../lib/watchdog.h" 
#include "../lib/time.h"
#include "../lib/schedule.h"
#include "../include/romver.h"
#include "../comm/startnet.h"

#pragma codeseg(EEPAGE)

/*
#define NETBLOCKSIZE ((((NodeListLengthMax + 2) +1) & 0xFFFE) + \ /* arbitration list */
                      2*Buffers*BufSize +                   \   /* Rx and Tx databuffers */
                      sizeof(RxIdType)*Buffers +            \   /* RxId blocks   */
                      sizeof(RxIdType *)*(Buffers + 1)*3 +  \   /* size of fifo' */
                      sizeof(TxIdType)*Buffers +            \   /* TxId blocks   */
                      sizeof(TxIdType *)*((Buffers+1)*2+2))     /* size of fifo' */
*/


#define NETBLOCKSIZE ( ((((NodeListLengthMax + 2) +1) & 0xFFFE)) + \
                      (2*BUFFERS*((BUFSIZE+1) &0xFFFE))+                   \
                      (sizeof(RxIdType)*BUFFERS) +            \
                      (sizeof(RxIdType *)*(BUFFERS + 1)*3) +  \
                      (sizeof(TxIdType)*BUFFERS)+            \
                      (sizeof(TxIdType *)*((BUFFERS+1)*2+2)))




signed char NetBlock[NETBLOCKSIZE+1];
short       RequestedBlockSize;
static char Resources[MaxNrOfResources];



// =============================================================================
void SUSPEND(void)
{
  WatchDog();
// while (1);
}

// =============================================================================
void InitUsx(void)
{
 int i;

 for (i = 0 ; i < MaxNrOfResources;i +=1 ) Resources[i] = 0;

}

// =============================================================================
short TransformResNr(short ResNr)
{
  if (ResNr == NetSendResLow) ResNr = 0;
  else if (ResNr == NetSendRes) ResNr = 1;
    else if (ResNr == NetRelRes) ResNr = 2;
      else if (ResNr == NetRecRes) ResNr = 3;
        else if (ResNr == IIC_RES)  ResNr = 4;
          else if (ResNr == NET_BUF_RES) ResNr = 5;
            else return -1;

  return ResNr;
}

// =============================================================================
short UsxGetRes(short ResNr )
{
  unsigned short time;
  ResNr = TransformResNr(ResNr);

  if ((ResNr < MaxNrOfResources) && (ResNr != -1)) {
    	
wait_for_res:
    time = Cnt1sec;
    while (Resources[ResNr] == 1){
      WatchDog();
      if ( TimeControl( Cnt1sec, time+5)){
      	if (ResNr == 4) Resources[ResNr] = 0;
      }
      TaskExit();	
    }	 
    disable_interrupt();
    if ( Resources[ResNr] == 1 ){
      enable_interrupt();
      goto wait_for_res;
    } else {
      Resources[ResNr] = 1;
      enable_interrupt();
    }  
    return 0;                                         /* return okay  */
  } else {
    return -1;                                        /* return error */
  }

}

// =============================================================================
short UsxSltTsk(void (*TaskName)(void))
{
 int i;
 i = ((short) TaskName) +1;

 return 1;
}


// =============================================================================
unsigned long UsxGetAbsTime(void)
{
  return ( AbsTime );
}


// =============================================================================
short UsxRelRes(short ResNr)
{
  ResNr = TransformResNr(ResNr);

  if (ResNr < MaxNrOfResources) {
    Resources[ResNr] = 0;
    return 0;
  } else {
    return -1;
  }
}


// =============================================================================
short UsxReqMem(short NrOfBytes, short type_requested)
{
  type_requested +=1;

  RequestedBlockSize = NrOfBytes;
  if (NrOfBytes > NETBLOCKSIZE) {
    return -1;
  } else {
    return (short) NetBlock;
  }
}


// =============================================================================
short UsxRelMem(void *Addresspointer)
{     
  unsigned char dummy;
 
  dummy =   *(unsigned char *)Addresspointer;

  return 1;
}

