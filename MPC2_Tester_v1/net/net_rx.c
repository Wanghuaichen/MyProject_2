/*******************************************************************/
/*                                                                 */
/*    AMICE 4.0  -  68HC11                                         */
/*                                                                 */
/*    Rx first entry: bit 9=1 handle here, else call NNetRx()      */
/*                                                                 */
/*******************************************************************/

#include "ext_net.h"
#include "local_net.h"
#include "net_target.h"
#include "net_state_nrs.h"


#pragma codeseg(EEPAGE)

/************** external function prototypes *******************/

/***************************************************************/

void NetRx(void)
{
  unsigned char  Byte;

  ReloadTimeout();                              /* reload time out timer */


  if ( RxUartError() != 0) {                    /* if error during reading byte */
    EmptyRxUart();
    StopTimeout();
    DisableTx();

    Net.TxState = TxReset;
    Net.RxState = RxReset;
    Net.ErrStat.ReSync++;
    return;
  }
  Net.NetStat.BusAct = TRUE;                    /* signal bus activity */

  if (Rx9thBit() != 0) {                        /* bit 9 is '1'?, handle here */
    if (Net.RxState != RxReset) {               /* resynchronize if necessary */
      Net.ErrStat.ReSync++;
      Net.RxState = RxReset;
    }
    Byte = ReadUart();
    if (!(Byte & TokenFrameMask)) {             /* token received? */
      if (Byte == CheckToken(Byte)) {
        Net.NetStat.Arbit = TRUE;
        if (!Net.TxDrCb.TxRequest) {            /* quick test on messages */
          DisableTx();
          StartTimeout(TimeoutTokTok);
          Net.TxState = TxToken;
        } else {
          Net.TxState = TxDestination;
          NetTx();
        }
      } else {
        /* do not start sending anything when another node is already  */
        /* arbitrated due to a too great delay before starting to send */
        /* try next time, hopefully with more success                  */
        Net.TxState = TxReset;
      }
    } else {                                    /* Rx start of message */
      if (((Byte & AddressMask) == CheckNode(Byte)) ||
          ((Byte & AddressMask) == BroadcastAddr)   && Net.TxState == TxReset) {

        Net.RxDrCb.Destin = Byte & AddressMask;               /* NEW */
        Net.StatusPacket.Source     = Byte & AddressMask;     /* NEW */
        Net.RxDrCb.Priority = Byte & PriorityMask;
        Net.RxDrCb.Crc = InitCrc;
        Crc12(Byte, &Net.RxDrCb.Crc);
        WakeUpModeOff();
        Net.RxState = RxSource;
      }
    }
  } else {                                      /* bit 9 is '0', call NNetRx() */
    NNetRx();
  }
}
