/* -------------------------------------------------------------------------- */
/*                                                                            */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                                   */
/*     ALL RIGHTS RESERVED                                                    */
/*                                                                            */
/* description: Flow Check                                                    */
/*                                                                            */

/* include files --------------------------------------------------------------*/
#include "../lib/io6811.h"
#include "../lib/reg_flags.h"
#include "../lib/boolean.h"
#include "../lib/rti_int.h"
#include "../lib/eehc11.h"
#include "../lib/schedule.h"
#include "../lib/taskdef.h"
#include "../include/romstnet.h"
#include "../include/romeehc1.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../comm/flowrcv.h"
#include "../comm/flowctrl.h"
#include "../comm/startnet.h"
#include "../net/ext_net.h"
#include "../init/start.h"

#define MAX_MSG_SIZE            BUFSIZE
#define MAIL_SIZE               (MAX_MSG_SIZE + sizeof(NETIO_MAIL))

#pragma codeseg(EEPAGE)


unsigned char Mail[MAIL_SIZE]; 
unsigned char StopNetReceive; 

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
void HandleMess(void)
{
  NETIO_MAIL  *MailPtr;
  signed short SrcAddress, DestAddress, MessageLength;
  signed short MessageType;

  StopNetReceive = FALSE;
  /*---------------------------------------------------------------------------*/
  while (1) {
    
    if (StopNetReceive == TRUE) {
       StopNetReceive = FALSE;
       TaskStop(NET_REC_TASK);
       PerPresenceResultTime = Cnt1sec +15;
    }                                                  
#if ( _EMC_TEST_ )
    //Eerste 60 seconden na opstarten geen Presence result sturen 
    if (Cnt1sec > PerPresenceResultTime && Cnt1sec > 5) {
       
      //Zend per. presence iedere 8 minuten
      PerPresenceResultTime = Cnt1sec + 3;
      MailPtr = (NETIO_MAIL *)Mail;
      MailPtr->channel = 1;
      MailPtr->msglnk = 0;
      if ((EEAddressCopy  > 0) && (EEAddressCopy  <= 50)) {
       MailPtr->address_dest = EEAddressCopy;
       HndlPerPresenceResReq(MailPtr);
      }
    }
#else
    //Eerste 60 seconden na opstarten geen Presence result sturen 
    if (Cnt1sec > PerPresenceResultTime && Cnt1sec > 60) {
       
      //Zend per. presence iedere 8 minuten
      PerPresenceResultTime = Cnt1sec + 480;
      MailPtr = (NETIO_MAIL *)Mail;
      MailPtr->channel = 1;
      MailPtr->msglnk = 0;
      if ((EEAddressCopy  > 0) && (EEAddressCopy  <= 50)) {
       MailPtr->address_dest = EEAddressCopy;
       HndlPerPresenceResReq(MailPtr);
      }
      if ((EEAddress2 > 0) && (EEAddress2 <= 50)) {
       MailPtr->address_dest = EEAddress2;
       HndlPerPresenceResReq(MailPtr);
      }
    }
#endif
    /* vraag een geheugenblok op voor een ontvangen bericht ---------------------*/
    MailPtr = (NETIO_MAIL *)Mail;

    /* controleer of er een bericht op het netwerk is binnengekomen -----------*/
    while ((MessageLength = NetReceive(1, &SrcAddress,  &DestAddress, &MessageType,
         (unsigned char *)(&(MailPtr->message_id)))) != 0) {
      //SrcAddress is het adres waar het bericht vandaan komt
      //DestAddress is het adres waar het antwoord naar toe moet
      /* controleer de grootte van het bericht --------------------------------*/
      if (MessageLength < 2) continue;

      /* maak het bericht klaar voor een mailbox ------------------------------*/

      MailPtr->msgpri = (MailPtr->message_id & 0x0400) ? 1 : 0;
      MailPtr->channel = 1;
      MailPtr->address_src = SrcAddress;
      MailPtr->address_dest = DestAddress;
      MailPtr->message_length = MessageLength - 2;
      MailPtr->msglnk = 0;

      // geef mail door aan FlowControlProcess ---------------------------------
      FlowControl(MailPtr);

    }  
    TaskExit(); 
  } // end while(1) ------------------------------------------------------------
} 


