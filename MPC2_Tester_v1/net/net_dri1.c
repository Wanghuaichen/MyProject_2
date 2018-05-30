/*******************************************************************/
/*                                                                 */
/*    AMICE 4.0  -  68HC!!                                         */
/*                                                                 */
/*    Datalink / network interrupt driver                          */
/*                                                                 */
/*******************************************************************/

#include "ext_net.h"
#include "net_state_nrs.h"
#include "local_net.h"
#include "net_target.h"


#pragma codeseg(EEPAGE)

/****************** local function prototypes *********************/

#define NULL (void *)0
#define CrcValid 0

/****************** external function prototypes ******************/

/* ManyNodes = 0 -> Recv PacketNrs[Source] Send PacketNrs[Destin] */
/* ManyNodes = 1 -> Recv PacketNrs[Destin] Send PacketNrs[Source] */


/*******************************************************************/
/*                                                                 */
/*    Uart Rx interrupt on 'Channel'                               */
/*                                                                 */
/*    Input: NetPntr - pointer to struct 'Net[Channel]'            */
/*                                                                 */
/*******************************************************************/


void  NNetRx(void)
{
  unsigned char  Byte, pack_nr;

  switch (Net.RxState) {        /* Receiver state machine */

    default:
    case RxReset : {            /* rest, illegal state  handled in NetRx */

      Byte = ReadUart();                                        /*  clear uartbyte                                */
/*    if ( Net.NetStat.Mode != ArbiMaster) {  */                /*  if not arbitrationmaster                      */
        WakeUpModeOn();                                         /*    set wakeup mode on                          */
/*    }   */                                                    /*  endif not arbitrationmaster                   */
      break;
    }

    case RxSource : {           /* expect source address */

      Byte = ReadUart();                                        /*  read second byte of message from uart        */
      Crc12(Byte, &Net.RxDrCb.Crc);                             /*  update CRC                                   */
      Net.RxDrCb.MsgType      = Byte & MsgTypeMask;             /*  messagetype to Rx drivercontrolblock         */
      if ( Net.RxDrCb.Source != (Byte & AddressMask)) {         // NEW 
        Net.RxDrCb.Source = Byte & AddressMask;                 // different source
        Net.RxDrCb.RxDrPacketNr |= AcceptPacketNr;              // accept message                
      }                                                         // NEW
      Net.StatusPacket.Destin = Byte & AddressMask;             /*  prepare destinationbyte of statuspacket      */
                                                                /*  the sourceaddress is transferred to RxDrCb   */
                                                                /*  in RxState RxLongFstCrc                      */
      if (Byte & ShortMsgTypeMask) {                            /*  if shortframe                                */
        Net.RxState = RxShortData;                              /*     next RxState = see left                   */
      } else {                                                  /*  else                                         */
        Net.RxState = RxLongPackNr;                             /*     next RxState = see left                   */
      }                                                         /*  endif                                        */
      break;
    }

    case RxShortData : {        /* expect data byte of short frame */

      Byte = ReadUart();
      Net.StatusPacket.Data = Byte;
      Crc12(Byte, &Net.RxDrCb.Crc);
      Net.RxState = RxShortFstCrc;
      break;
    }

    case RxShortFstCrc : {      /* expect 1rst crc-byte short frame */

      Byte = ReadUart();
      Crc12(Byte, &Net.RxDrCb.Crc);
      Net.RxState = RxShortSndCrc;
      break;
    }

    case RxShortSndCrc : {      /* expect 2nd crc-byte short frame */

      Byte = ReadUart();
      Crc12(Byte,&Net.RxDrCb.Crc);
      if (Net.RxDrCb.Crc == ValidCrc) {                /* test crc */
        if (Net.RxDrCb.MsgType == StatusMsgType ) {
          if ((Net.TxState == TxStatWait) ||
              (Net.TxState == TxSyncWaitStat)) {
            StopTimeout();
            if (Net.TxState == TxStatWait) {
              /* valid status packet received on long message  */
              Net.TxState = TxStatValid;
            } else {
              /* valid status packet received on sync message */
              Net.TxState = TxSyncStatAck;
            }
            NetTx();                                                           /* handle received status packet */
          }
        } else {
          if (Net.RxDrCb.MsgType == LooptestMsgType) {                          /*  looptest or syncpacket   */
            if (Net.StatusPacket.Data == SyncLowPri) {
              /* status low sync packet receive */
              // ManyNodes && source == Master -> destin else source 
              if (Net.ManyNodes && !Net.RxDrCb.RxIdPntr->Source  ) {
                Net.PacketNrsRx[Net.StatusPacket.Destin] &= ClearLowPriRx;      // synchronise low priority  
              } else {
                Net.PacketNrsRx[Net.StatusPacket.Source] &= ClearLowPriRx;      // synchronise low priority  
              }
            } else {                                                            /* receiver                  */
              if (Net.StatusPacket.Data == SyncHighPri) {
                /* status high sync packet received */
                // ManyNodes && source == Master -> destin else source 
                if (Net.ManyNodes && !Net.RxDrCb.RxIdPntr->Source  ) {
                  Net.PacketNrsRx[Net.StatusPacket.Source] &= ClearHighPriRx;     // synchronise low priority  
                } else {
                  Net.PacketNrsRx[Net.StatusPacket.Destin] &= ClearHighPriRx;     // synchronise low priority  
                }
              }                                                                 /* else looptest             */
            }
            Net.StatusPacket.Data = StatusAccept;
            /* send acknowledge to sender */
            Net.TxState = TxStatDest;
            NetTx();                                                            /* return received looptest packet */
          }
        }
      } else {
        Net.ErrStat.CrcCnt++;                                                   /* count CRC error           */
      }
      if (Net.NetStat.Mode != ArbiMaster) {
        WakeUpModeOn();                                                         /* receiver to initial state */
      }
      Net.RxState = RxReset;
      break;
    }

    case RxLongLength : {       /* expect length byte long frame */

      Byte = ReadUart();
      Crc12(Byte, &Net.RxDrCb.Crc);
      Net.RxDrCb.MsgLength = Byte+1;
      if (Net.RxDrCb.RxIdPntr != NULL) {                                        /* ID in use?                */
        IntPutFreeRxId(&Net,Net.RxDrCb.RxIdPntr);                               /* then return to stack      */
      }
      Net.RxDrCb.RxIdPntr = GetFreeRxId(&Net);                                  /* claim free RxId block */
      if (Net.RxDrCb.RxIdPntr == NULL) {                                        /* no block available?       */
        Net.StatusPacket.Data = StatusNotReady;                                 /* status 'not ready'        */
        Net.RxState = RxLongDataError;
      } else {
        if (Net.RxDrCb.MsgLength > Net.NetStat.Size) {
          Net.StatusPacket.Data = StatusSizeError;                              /* status 'size error'       */
          Net.ErrStat.BufOvf++;                                                 /* count buffer overflow     */
          Net.RxState = RxLongDataError;
        } else {
          Net.StatusPacket.Data = StatusAccept;                                 /* status packet accepted    */
          Net.RxDrCb.RxIdPntr->Length = Byte + 1;
          Net.RxDrCb.RxIdPntr->Destin = Net.RxDrCb.Destin;
          Net.RxDrCb.RxIdPntr->Source = Net.RxDrCb.Source;
          Net.RxDrCb.RxDataPntr = (signed char *) Net.RxDrCb.RxIdPntr->DataPntr;
          Net.RxState = RxLongData;
        }
      }
      break;
    }

    case RxLongData : {         /* expect databyte long frame */

      Byte = ReadUart();
      Crc12(Byte,&Net.RxDrCb.Crc);
      *Net.RxDrCb.RxDataPntr++ = Byte;
      Net.RxDrCb.MsgLength--;
      if (Net.RxDrCb.MsgLength == 0) {
        Net.RxState = RxLongFstCrc;
      }
      break;
    }

    case RxLongDataError : {    /* expect databyte long frame with size error */

      Byte = ReadUart();
      Crc12(Byte, &Net.RxDrCb.Crc);
      Net.RxDrCb.MsgLength--;
      if (Net.RxDrCb.MsgLength == 0) {
        Net.RxState = RxLongFstCrc;
      }
      break;
    }

    case RxLongFstCrc : {       /* expect first crc-byte long frame */

      Byte = ReadUart();
      Crc12(Byte, &Net.RxDrCb.Crc);
      if (!Net.StatusPacket.Data) {
        switch (Net.RxDrCb.MsgType) {
          case ConfirmMsgType: {     /* type confirmed long frame */
            Net.RxDrCb.RxIdPntr->Type = (Net.RxDrCb.Priority == High) + 1;
            break;
          }
          case UnconfirmMsgType: {   /* type unconfirmed long frame */
            Net.RxDrCb.RxIdPntr->Type = (Net.RxDrCb.Priority == High) + 3;
            break;
          }
        } /* end switch (Net.RxDrCb.MsgType) */
      }
      Net.RxState = RxLongSndCrc;
      break;
    }

    case RxLongSndCrc : {                                                       // expect 2nd crc-byte long frame      

      Byte = ReadUart();                                                        // read second CRC-byte    
      if (Net.NetStat.Mode != ArbiMaster) {                                     // if arbitrationslave     
        WakeUpModeOn();                                                         //   then to wake-up-mode  
      }                                                                         // endif arbitrationslave  
      Crc12(Byte, &Net.RxDrCb.Crc);                                             // update CRC              
      if (Net.RxDrCb.Crc == ValidCrc) {                                         // if CRC is correct       
        if (!Net.StatusPacket.Data) {                                           //   if no errors occured during receive      
          Byte = Net.RxDrCb.RxDrPacketNr;
          if (Byte != SkipPacketNr) {                                           //     if not message double received        
            Byte &= PacketNrMask;                                               //       Mask: 0x03
            if (Byte != 0) {                                                    //       if not unconfirmed message 
              Byte = (Byte << ShiftPacketNrToRx);                               //         update last packetnr, shift: 4          
              if (Net.RxDrCb.Priority != 0) {                                   //         if high priority                 
                Byte = (Byte << ShiftPacketNrToHigh);                           //           update high prior rec. packetnr, shift: 2  
                // ManyNodes && source == Master -> destin else source 
                if (Net.ManyNodes && !Net.RxDrCb.RxIdPntr->Source  ) {
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Destin] &= ClearHighPriRx; //           &= 0x3F
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Destin] |= Byte;           //           |= PacketNr << 6 
                } else {
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Source] &= ClearHighPriRx; //           &= 0x3F
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Source] |= Byte;           //           |= PacketNr << 6 
                }
              } else {                                                            //         else low priority
                if (Net.ManyNodes && !Net.RxDrCb.RxIdPntr->Source ) {
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Destin] &= ClearLowPriRx;  //           &= 0xCF
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Destin] |= Byte;           //           |= PacketNr << 4 
                } else {
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Source] &= ClearLowPriRx;  //           &= 0xCF
                  Net.PacketNrsRx[Net.RxDrCb.RxIdPntr->Source] |= Byte;           //           |= PacketNr << 4
                }
              }                                                                 //         endif high priority                
            }                                                                   //       endif not unconfirmed message        
            PutUsedRxId(&Net);                                                  //       put message ID into Rx fifo          
            Net.RxDrCb.RxIdPntr = NULL;
          }                                                                     //     endif not message double received      
        }                                                                       //   endif no errors occured during receive   
        if (Net.RxDrCb.MsgType == ConfirmMsgType) {                             //   if confirmed message                     
          Net.TxState = TxStatDest;
          NetTx();                                                              //     send status packet                    
        }                                                                       //   endif onfirmed message                   
      } else {                                                                  // else CRC not correct                       
        Net.ErrStat.CrcCnt++;                                                   //   increase CRC-error counter               
      }                                                                         // endif CRC is correct                       
      Net.RxState = RxReset;
      break;                                                                    // end of this RxState                       
    }

    case RxLongPackNr : {                                                       /* packetnumber long frame */

      Byte = ReadUart();                                                        /* read packetnr from uart                    */
      Crc12(Byte, &Net.RxDrCb.Crc);                                             // update CRC
      pack_nr = Net.RxDrCb.RxDrPacketNr;                                            
      Net.RxDrCb.RxDrPacketNr = Byte;                                          // save PacketNr                              

      if (Byte != 0) {                                                         // if not unconfirmed message                 
        // ManyNodes && source == Master -> destin else source 
        if (Net.ManyNodes && !Net.RxDrCb.RxIdPntr->Source  ) {
          Byte = Net.PacketNrsRx[Net.RxDrCb.Destin] ;
        } else {
          Byte = Net.PacketNrsRx[Net.RxDrCb.Source] ;
        }
        if (Net.RxDrCb.Priority != 0) {                                        //   if high priority
          Byte = (Byte >> ShiftHighRxToNormal);                                //     decode high priority packetnr, shift >> 6
        } else {                                                               //   else low priority                        
          Byte = (Byte >> ShiftLowRxToNormal);                                 //     decode low  priority packetnr, shift >> 4 
        }                                                                      //   endif  priority                     
        Byte &= PacketNrMask;
        if (Byte > 0) {                                                        //   if packetnr synchronised                 */
                                                                               // NEW
          if (!( pack_nr & AcceptPacketNr )){                                  // if message from same source 
            if (Byte == Net.RxDrCb.RxDrPacketNr) {                             //   if packetnr == last-packetnr           
              Net.RxDrCb.RxDrPacketNr = SkipPacketNr;                          //     mark message to be skipped         
            }                                                                  // NEW 
          }                                                                    /*     endif packet == last-packetnr          */
        }                                                                      /*   endif synchronised                       */
      }                                                                        /* endif not unconfirmed message              */
      Net.RxState = RxLongLength;                                               /* Next RxState is 5                          */
      break;                                                                    /* end of this RxState                        */
    }
  }
}


/*******************************************************************/
/*                                                                 */
/*    Uart Tx interrupt on 'Channel'                               */
/*                                                                 */
/*******************************************************************/

  const signed char TxTypeState[10] = {     /* Type to next Tx state */
    TxReset,
    TxLongSource,
    TxLongSource,
    TxLongSource,
    TxLongSource,
    TxPredFstData,
    TxPredFstData,
    TxPredFstData,
    TxPredFstData,
    TxShortSource
  };

  unsigned char  NetTxByte;
  unsigned char *NetTxUnsCharPntr;
  TxIdType      *NetTxHlpIdPntr;
  signed char    NetTxHlpStatus;
  signed char    NetTxType;

void  NetTx(void)
{


  switch (Net.TxState) {

    default:
    case TxReset : {            /* reset (disable transmitter) */

      DisableTx();
      break;
    }

    case TxDestination : {      /* token received and Tx request, send message */

      Net.TxDrCb.TxIdPntr = FindTxReq(&Net);                                    /* find next message to send             */
                                                                                /*                                       */
      if (Net.TxDrCb.TxIdPntr == NULL) {                                        /* if no message available               */
        StartTimeout(TimeoutTokTok);                                            /*    send next token                    */
        Net.TxState = TxToken;                                                  /*                                       */
      } else {                                                                  /* else message available: fiil TxDrCb   */
        NetTxType = Net.TxDrCb.TxIdPntr->Type;                                  /*                                       */
        NetTxHlpStatus = PriorCode[NetTxType];                                  /*                                       */
        Net.TxDrCb.Priority = NetTxHlpStatus;                                   /*   MsgPriority in TxDrCb               */
        Net.TxDrCb.MsgType = TypeCode[NetTxType];                               /*   MsgNetTxType in TxDrCb                      */
        if ((NetTxByte=Net.TxDrCb.TxIdPntr->TxIdPacketNr) == GetPacketNrNr) {   /*  if determine packetnr                */
          // ManyNodes && source == Master -> source else destin 
          if (Net.ManyNodes && !Net.TxDrCb.TxIdPntr->Destin  ) {
            NetTxUnsCharPntr = &(Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Source]);
          } else {
            NetTxUnsCharPntr = &(Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Destin]);
          }
          NetTxByte = *NetTxUnsCharPntr;                                        /*     get packetnr's of this node       */
          if (NetTxHlpStatus == 0) {                                            /*     if low priority                   */
            NetTxByte &= SyncLowPriTxMask;                                      /*       Decode low  prio packetnr       */
            if (NetTxByte == 0) {                                               /*       if not synchronised             */
              NetTxByte = SyncPacketNr;                                         /*         return syncpacketnr           */
            } else {                                                            /*       else synchronised               */
              if (NetTxByte == SyncPacketNrMax) {                               /*         increment and update packetnr */
                NetTxByte = 1;                                                  /*                          transmit     */
                *NetTxUnsCharPntr &= FirstLowPriTx;                             /*                                       */
              } else {                                                          /*                                       */
                NetTxByte++;                                                    /*                                       */
                *NetTxUnsCharPntr += 1;                                         /*                                       */
              }                                                                 /*                                       */
            }                                                                   /*       endif not synchronised          */

          } else {                                                              /*     else  high priority               */
            NetTxByte = (NetTxByte >> ShiftHighTxToNormal) & PacketNrMask;      /*       Decode high prio packetnr       */
            if (NetTxByte == 0) {                                               /*       if not synchronised             */
              NetTxByte = SyncPacketNr;                                         /*         return syncpacketnr           */
            } else {                                                            /*       else synchronised               */
              if (NetTxByte == SyncPacketNrMax) {                               /*         increment and update packetnr */
                NetTxByte = 1;                                                  /*                           transmit    */
                *NetTxUnsCharPntr &= FirstHighPriTx;                            /*                                       */
              } else {                                                          /*                                       */
                NetTxByte++;                                                    /*                                       */
                *NetTxUnsCharPntr += 4;                                         /*                                       */
              }                                                                 /*                                       */
            }                                                                   /*       endif not synchronised          */
          }                                                                     /*     endif low priority                */
          Net.TxDrCb.TxIdPntr->TxIdPacketNr = NetTxByte;                        /*   set packetnr in TxId                */
        }                                                                       /*   endif determine packetnr            */
        Net.TxDrCb.TxDrPacketNr = NetTxByte;                                    /*   store packetnr in TxDrCb            */
        SetBit9();                                                              /*   send first byte of frame            */
        NetTxByte = Net.TxDrCb.TxIdPntr->Destin + NetTxHlpStatus + DataFrame;
        EnableTx();
        Net.TxDrCb.Crc = InitCrc;                                               /*   initialise CRC                      */
        WriteUart(NetTxByte);
        Crc12(NetTxByte, &Net.TxDrCb.Crc);
        if (Net.TxDrCb.TxDrPacketNr == SyncPacketNr) {                          /*   determine next TxState              */
          Net.TxState = TxSyncSpace;                                            /*   if not synchronised                 */
        } else {                                                                /*     next TxState is see left          */
          Net.TxState = TxLongSpace;                                            /*   else                                */
        }                                                                       /*     next TxState is see left          */
      }
      break;
    }

    case TxSyncSpace : {        /* space between address and data byte  in synchronisation-packet */

      StartTimeout(TimeoutByte1);                                               /* Tx next byte after time out */
      NetTxType = Net.TxDrCb.TxIdPntr->Type;
      Net.TxState = TxSyncSource;                                               /* select next state           */
      DisableTx();                                                              /* stop Tx after this byte     */
      break;
    }

    case TxSyncSource : {       /* sync-packet type and sourcebyte */

      NetTxByte = (Net.TxDrCb.TxIdPntr->Source & AddressMask) + SyncPacketMsk;
      ClrBit9();
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxSyncData;
      break;

    }

    case TxSyncData : {         /* Synchronisation packet, send data byte */

      if (Net.TxDrCb.Priority == 0) {                           /*  if message of low priority     */
        NetTxByte = SyncLowPri;                                 /*    databyte = 2                 */
      } else {                                                  /*  else                           */
        NetTxByte = SyncHighPri;                                /*    databyte = 3                 */
      }                                                         /*  endif message of low priority  */
      WriteUart(NetTxByte);                                     /*  databyte to uart               */
      Crc12(NetTxByte, &Net.TxDrCb.Crc);                        /*  update CRC                     */
      Net.TxState = TxSyncFstCrc;                               /*  next TxState is SyncTxState4   */
      break;
    }

    case TxSyncFstCrc : {       /* synchronisation packet, send first crc byte */

      NetTxByte = (signed char) (Net.TxDrCb.Crc & 0x00FF);
      WriteUart(NetTxByte);
      Net.TxState = TxSyncSndCrc;
      break;
    }

    case TxSyncSndCrc : {       /* synchronisation packet, send second crc byte */

      NetTxByte = (signed char) ((Net.TxDrCb.Crc & 0x0F00) >> 8);
      WriteUart(NetTxByte);                                                     /*  2nd CRC-byte to uart                  */
      StartTimeout(TimeoutStatus);                                              /*  Start timeout on statuspacket         */
      Net.TxState = TxSyncWaitStat;                                             /*  Next state = wait until status packet */
      break;
    }

    case TxSyncWaitStat : {     /* wait until status packet (with time out) */

      DisableTx();
      break;
    }

    case TxSyncStatAck : {      /* valid status packet received on synchronisation-packet */

     if ( !Net.StatusPacket.Data ) {                                            // if valid statuspacket ack received        
       if ( Net.TxDrCb.Priority ) {                                             //   if message is low prio                  
         // ManyNodes && source == Master -> source else destin 
         if (Net.ManyNodes && !Net.TxDrCb.TxIdPntr->Destin  ) {
           Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Source] |= SyncHighPriTx;       //     synchronise high priority transmit    
         } else { 
           Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Destin] |= SyncHighPriTx;       //     synchronise high priority transmit   
         }
       } else {                                                                 //   else high priority                       
         // ManyNodes && source == Master -> source else destin 
         if (Net.ManyNodes && !Net.TxDrCb.TxIdPntr->Destin ) {
           Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Source] |= SyncLowPriTx;        //     synchronise low priority transmit     
         } else {
           Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Destin] |= SyncLowPriTx;        //     synchronise low priority transmit    
         }
       }                                                                        // endif message is low priority            
       Net.TxDrCb.TxIdPntr->TxIdPacketNr = SyncPacketNrMax;                     /* update packetnr in TxId                   */
       Net.TxDrCb.TxDrPacketNr = SyncPacketNrMax;                               /* update packetnr in Tx driver controlblock */
       Net.TxState = TxDestination;                                             /* next state is send message               */
       StartTimeout(TimeoutTokTok);                                             /* Tx next byte after time out              */
     } else {
       Net.TxState = TxStatValid;                                               /* next state is send message               */
       NetTx();
     }
     break;
    }

    case TxLongSpace : {        /* space between address and data byte */

      StartTimeout(TimeoutByte1);                               /* Tx next byte after time out              */
      NetTxType   = Net.TxDrCb.TxIdPntr->Type;
      Net.TxState = TxTypeState[NetTxType];                     /* select next state on msg type            */
      DisableTx();                                              /* stop Tx after this byte                  */
      break;
    }

    case TxLongSource : {       /* long frame, send type and source address */

      NetTxByte = (Net.TxDrCb.TxIdPntr->Source & AddressMask) + Net.TxDrCb.MsgType;
      ClrBit9();
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxLongPackNr;
      break;
    }

    case TxLongPackNr : {       /* long frame, send packetnr */

      NetTxByte = Net.TxDrCb.TxDrPacketNr;                                      /* Packetnr to uart  */
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);                                         /* update CRC        */
      Net.TxState = TxLongLength;                                               /* Next TxState is see left */
      break;
    }

    case TxLongLength : {       /* long frame send length byte */

      Net.TxDrCb.MsgLength = Net.TxDrCb.TxIdPntr->Length;                       /* length = nr of NetTxBytes -1 */
      Net.TxDrCb.TxDataPntr = Net.TxDrCb.TxIdPntr->DataPntr;
      NetTxByte = Net.TxDrCb.MsgLength-1;
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxLongData;
      break;
    }

    case TxLongData : {         /* long frame send data bytes */

      NetTxByte = *Net.TxDrCb.TxDataPntr++;
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxDrCb.MsgLength--;
      if (Net.TxDrCb.MsgLength == 0) {
        Net.TxState = TxLongFstCrc;                                             /* no more data, go to next state */
      }
      break;
    }

    case TxLongFstCrc : {       /* send first crc byte */

      NetTxByte = (signed char) (Net.TxDrCb.Crc & 0x00FF);
      WriteUart(NetTxByte);
      Net.TxState = TxLongSndCrc;
      break;
    }

    case TxLongSndCrc : {       /* send second crc byte */

      NetTxByte = (signed char) (Net.TxDrCb.Crc >> 8);
      WriteUart(NetTxByte);
      NetTxType = Net.TxDrCb.MsgType;
      if ((!(NetTxType & (signed char)ConfMsgTypeMask)) || (NetTxType == (char)LooptestMsgType)) {
        StartTimeout(TimeoutStatus);
        Net.TxState = TxStatWait;                                               /* wait until status packet */
      } else {                                                                  /* unconfirmed data packet  */
        Net.TxDrCb.TxIdPntr->Status = 10;                                       /* status ID not in use     */
        IntPutFreeTxId(&Net,FreeTxReq(&Net));                                   /* return ID to stack       */
        Net.TxState = TxTokenUncMess;
      }
      break;
    }

    case TxTokenUncMess : {     /* send token after unconfirmed message */

      SendNextToken();
      Net.TxState = TxReset;
      break;
    }

    case TxStatWait : {         /* wait until status packet (with time out) */

      DisableTx();
      break;
    }

    case TxStatValid : {        /* valid status packet received */

      if (!(Net.StatusPacket.Data & StatusAcceptMask)) {                        /* data packet accepted?                    */
        NetTxHlpIdPntr = FreeTxReq(&Net);
        NetTxHlpStatus = 0;                                                     /* Tx status 'transmission ready'           */
      } else {                                                                  /* data packet not accepted?                */
        if (Net.StatusPacket.Data & StatusSizeError) {                          /* message too long?                        */
          NetTxHlpIdPntr = FreeTxReq(&Net);
          NetTxHlpStatus = 5;                                                   /* Tx status 'buffer too small'             */
        } else {
          if (Net.TxPool[1].RetryCount == 0) {                                  /* no more retries?                         */
            NetTxHlpIdPntr = FreeTxReq(&Net);
            NetTxHlpStatus = NetTxHlpIdPntr->Type;                              /* use NetTxHlpStatus for scratchmem        */
            if (NetTxHlpStatus <= 6) {                                          /* if longframe confirmed (unconfirmed not possible */
              if ((NetTxHlpStatus & 3) == 1) {                                  /* if low priority                          */
                // ManyNodes && source == Master -> source else destin
                if (Net.ManyNodes && !Net.TxDrCb.TxIdPntr->Destin  ) {
                  Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Source] &= ClearLowPriTx;        // set low priority sending unsychronised   
                } else {
                  Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Destin] &= ClearLowPriTx;        // set low priority sending unsychronised   */
                }
              } else {
                // ManyNodes && source == Master -> source else destin 
                if (Net.ManyNodes && !Net.TxDrCb.TxIdPntr->Destin  ) {
                  Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Source] &= ClearHighPriTx;       // set high priority sending unsychronised  
                } else {
                  Net.PacketNrsTx[Net.TxDrCb.TxIdPntr->Destin] &= ClearHighPriTx;       // set high priority sending unsychronised  
                }
              }
            }

            if (Net.StatusPacket.Data & StatusTimeOut) {                        /* time out?                                */
              NetTxHlpStatus = 3;
            } else {                                                            /* no buffer                                */
              NetTxHlpStatus = 4;
            }
          } else {
            Net.TxPool[1].RetryCount--;
            NetTxHlpStatus = -1;
          }
        }
      }
      if (NetTxHlpStatus != -1) {                                               /* no retry?               */
        if (NetTxHlpIdPntr->Status == 7) {                                      /* immediate release ID?   */
          NetTxHlpIdPntr->Status = 10;                                          /* status ID not in use    */
          IntPutFreeTxId(&Net,NetTxHlpIdPntr);                                  /* return ID to stack      */
        } else {                                                                /* release ID by user?     */
          NetTxHlpIdPntr->Status = NetTxHlpStatus;                              /* return status           */
        }
      }
      SendNextToken();
      Net.TxState = TxReset;
      break;
    }

    case TxStatDest : {         /* status packet, send destination address */

      SetBit9();
      NetTxByte = Net.StatusPacket.Destin + DataFrame;
      EnableTx();
      Net.TxDrCb.Crc = InitCrc;
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxStatSpace;
      break;
    }

    case TxStatSpace : {        /* space between address and data byte */

      StartTimeout(TimeoutByte1);                               /* Tx next byte after time out */
      Net.TxState = TxStatSource;
      DisableTx();                                              /* stop Tx after this byte */
      break;
    }

    case TxStatSource : {       /* status packet, send source address */

      NetTxByte = Net.StatusPacket.Source + StatusMsgType;
      ClrBit9();
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxStatData;
      break;
    }

    case TxStatData : {         /* status packet, send status byte */

      NetTxByte = Net.StatusPacket.Data;
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxStatFstCrc;
      break;
    }

    case TxStatFstCrc : {       /* status packet, send first crc byte */

      NetTxByte = (signed char) (Net.TxDrCb.Crc & 0x00FF);
      WriteUart(NetTxByte);
      Net.TxState = TxStatSndCrc;
      break;
    }

    case TxStatSndCrc : {       /* status packet, send second crc byte */

      NetTxByte = (signed char) ((Net.TxDrCb.Crc & 0x0F00) >> 8);
      WriteUart(NetTxByte);
      if (Net.NetStat.Mode == ArbiMaster) {
        StartTimeout(TimeoutToken);        /* start token time out */
        WakeUpModeOff();
      }
      Net.TxState = TxReset;
      break;
    }

    case TxShortSource : {      /* short frame, send source address */

      NetTxByte = (Net.TxDrCb.TxIdPntr->Source & AddressMask) + Net.TxDrCb.MsgType;
      ClrBit9();
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxShortData;
      break;
    }

    case TxShortData : {        /* short frame, send data byte */

      NetTxByte = 0x00;
      WriteUart(NetTxByte);
      Crc12(NetTxByte, &Net.TxDrCb.Crc);
      Net.TxState = TxLongFstCrc;
      break;
    }

    case TxPredFstData : {      /* predefined frame, send first data byte */

      Net.TxDrCb.MsgLength = Net.TxDrCb.TxIdPntr->Length;                       /* length = nr of NetTxBytes -1 */
      Net.TxDrCb.TxDataPntr = Net.TxDrCb.TxIdPntr->DataPntr;
      ClrBit9();
      Net.TxState = TxPredData;                                                 /* no <break>, continue in state see left */
    }

    case TxPredData : {         /* predefined frame, send data bytes */

      NetTxByte = *Net.TxDrCb.TxDataPntr++;
      WriteUart(NetTxByte);
      Net.TxDrCb.MsgLength--;
      if (Net.TxDrCb.MsgLength == 0) {                                          /* end test message? */
        if (!TypeCode[Net.TxDrCb.TxIdPntr->Type]) {                             /* confirmed data packet?   */
          StartTimeout(TimeoutStatus);
          Net.TxState = TxStatWait;                                             /* wait until status packet */
        } else {                                                                /* unconfirmed data packet  */
          Net.TxDrCb.TxIdPntr->Status = 10;                                     /* status ID not in use     */
          IntPutFreeTxId(&Net,FreeTxReq(&Net));                                 /* return ID to stack       */
          Net.TxState = TxTokenUncMess;
        }
      }
      break;
    }

    case TxToken : {            /* send token */

/*    EnableTx();  */
      SendNextToken();
      Net.TxState = TxReset;
      break;
    }
  }
}

