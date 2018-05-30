/* -------------------------------------------------------------------------- */
/*                                                                            */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   */
/*     ALL RIGHTS RESERVED                                                    */
/*                                                                            */
/* description: Send messages                                                 */
/*                                                                            */
/* -------------------------------------------------------------------------- */

/* include files ---------------------------------------------------------------------------------------------*/

#include "../lib/stdlib.h"
#include "../lib/string.h"
#include "../comm/netsend.h"
#include "../usx/fake_usx.h"

extern unsigned char msgTimeoutTime;

#pragma codeseg(EEPAGE)


/* functions --------------------------------------------------------------------------------------------------*/

unsigned char SendMessage(unsigned short priority, unsigned short src_address, unsigned short dest_address, unsigned short message_length, unsigned char *message)
{
  int rval = 0;
	
  UsxGetRes(NET_BUF_RES);
  // MsgBuffer.Channel will be zero if buffer is empty
  if (MsgBuffer.Channel == 0){
    memcpy(MsgBuffer.Data, message, message_length);
    MsgBuffer.Type        = priority;
    MsgBuffer.SrcAddress  = src_address;
    MsgBuffer.DestAddress = dest_address;
    MsgBuffer.Length      = message_length;
    MsgBuffer.Channel     = 1;                      // mark buffer as filled
    rval = 1;
  } else {
    // check for comm-error; otherwise programm stalls (e.g HndlSndMsgToMasterSlave)
    if (msgTimeoutTime==0) rval =1;
  }
  UsxRelRes(NET_BUF_RES);
  return rval;	
}

