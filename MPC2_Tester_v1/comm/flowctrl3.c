// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// =============================================================================

// include files ---------------------------------------------------------------

#include "flowctrl.h"
#include "../include/romstnet.h"
#include "../lib/stdlib.h"
#include "../lib/schedule.h"
#include "../lib/extport.h"
#include "../lib/eeprom.h"
#include "../lib/eehc11.h"
#include "../lib/string.h"
#include "../comm/send_msg.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/conduct.h"

#pragma codeseg(EEPAGE)

extern unsigned char TempCorr;


// =============================================================================
void HndlPerGenRef(NETIO_MAIL *MailPtr)
{
    signed char  idx;
  unsigned short nrofbytes;

  PerGenRef *PerGenRefPtr = ( PerGenRef *)( &( MailPtr->message_start));

  if (MailPtr->message_length < sizeof(unsigned short)) {goto exit;}  /* bericht is te klein !!! */

  if (MailPtr->message_length < sizeof(PerGenRef) - sizeof(PerGenRefPtr->Array[0])) {
    nrofbytes = sizeof(PerGenRef) - sizeof(PerGenRefPtr->Array[0]);
  } else {
    nrofbytes = MailPtr->message_length;
  }

  idx = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_PerGenRef[idx] != NULL) {free(p_PerGenRef[idx]); p_PerGenRef[idx] = NULL;}
  while (p_PerGenRef[idx] == NULL) {p_PerGenRef[idx] = calloc(1, nrofbytes);}
  memcpy(p_PerGenRef[idx], PerGenRefPtr, MailPtr->message_length);

  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock1 , &EEDummy1     );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock2 , &EEMtrRow     );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock3 , &EEClxTime    );
  TempCorr = (unsigned char)p_PerGenRef[idx]->PerGenBlock3;
  if (TempCorr == 0) TempCorr = 100;
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock4 , &EEDetectPerNr);
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock5 , &EECal1       );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock6 , &EECal2       );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock7 , &EECal3       );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock8 , &EECal4       );
  WriteEEpromShort(p_PerGenRef[idx]->PerGenBlock9 , &EECal5       );

  ConCor1 = (unsigned char)p_PerGenRef[idx]->PerGenBlock10;
  if (ConCor1 == 0) ConCor1 = 100;
  ConCor2 = (unsigned char)p_PerGenRef[idx]->PerGenBlock11;
  if (ConCor2 == 0) ConCor2 = 100;
  ConCor3 = (unsigned char)p_PerGenRef[idx]->PerGenBlock12;
  if (ConCor3 == 0) ConCor3 = 100;
  ConCor4 = (unsigned char)p_PerGenRef[idx]->PerGenBlock13;
  if (ConCor4 == 0) ConCor4 = 100;

  nrofbytes = p_PerGenRef[idx]->PerGenBlock14;
  WriteEEpromByte((unsigned char)(nrofbytes>>8), &EEExtMode    );
  WriteEEpromByte((unsigned char) nrofbytes, &EEMlkStateUpdateCntr );

  ContScheduler();

exit: ;

}
// =============================================================================
// PerGenRef Request
// =============================================================================
unsigned char HndlPerGenRefReq(unsigned short PerNumber, signed short PerAddress)
{
  unsigned char Buffer[MESS_LEN(PerGenRefReq)], idx;

  *((unsigned short *)Buffer)   = PerGenRefReqId;
  *((unsigned short *)Buffer+1) = PerNumber;
  if (PerAddress != 0) {
    idx = GetNodeIndex(PerAddress);
    if (p_PerGenRef[idx] != NULL) {free(p_PerGenRef[idx]); p_PerGenRef[idx] = NULL;}

    return SendMessage( ConLow, PerAddress, 0, sizeof(Buffer), Buffer );
  }
  return 1;  // OK nothing to send
}


// =============================================================================
void HndlPerGenNumberRef(NETIO_MAIL *MailPtr)
{
  signed char idx;
  PerGenNumberRef *PerGenNumberRefPtr = ( PerGenNumberRef *)( &( MailPtr->message_start));

  if (MailPtr->message_length < sizeof(PerGenNumberRef)) {goto exit;}  /* bericht is te klein !!! */

  StopScheduler();
  idx = GetNodeIndex(MailPtr->address_dest);
  PerGenNumber[idx] = PerGenNumberRefPtr->PerGenNumber;
  ContScheduler();

exit: ;
}


// =============================================================================
// Handle CowGenRes
// =============================================================================
unsigned char HndlMlkStatusUpdateRes( unsigned short amount, unsigned char state)
{
  unsigned short len;
  unsigned char  Buffer[MESS_LEN(MlkStateUpdateRes)];
  unsigned char  idx    = GetSideIndex(SideState);
  MlkStateUpdateRes *MlkStateUpdateResPtr = (MlkStateUpdateRes *)(Buffer +2);

  // zend nieuw pergenres
  *((unsigned short *)Buffer) = MlkStateUpdateResId;
  ((MlkStateUpdateRes *)MlkStateUpdateResPtr)->PerGenNumber = p_PerGenRef[idx]->PerGenNumber;
  ((MlkStateUpdateRes *)MlkStateUpdateResPtr)->CowMlkYldAmount = amount;
  ((MlkStateUpdateRes *)MlkStateUpdateResPtr)->MlkState = state;
  ((MlkStateUpdateRes *)MlkStateUpdateResPtr)->Dummy = p_MlkRef[idx] == NULL ? 0xff : ((MlkRef **)p_MlkRef)[idx]->CowGenNumber;
  len = MESS_LEN(MlkStateUpdateRes);

  return SendMessage( ConLow, p_PerGenRef[idx]->PerGenNumber%100, 0, len, Buffer);
}

