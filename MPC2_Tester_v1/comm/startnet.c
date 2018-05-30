/*******************************************************************/
/*                                                                 */
/*    startnet.c                                                   */
/*                                                                 */
/*******************************************************************/

/* include files --------------------------------------------------*/

#include "startnet.h"
#include "../net/ext_net.h"
#include "../net/net_def.h"
#include "../net/local_net.h"
#include "../usx/fake_usx.h"
#include "../lib/eeprom.h"
#include "../lib/eehc11.h"
#include "../lib/burnin.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../include/romeehc1.h"

#pragma codeseg(EEPAGE)

#define         Cntrllr         0


//PerGenRef *p_PerGenRef;

/* Peripheral Address ------------------------------------------------*/


signed short PeripheralAddress;


/* initialisatie netwerk ---------------------------------------------*/

const signed char NodeList1[2] = { Cntrllr, -1 };



/*-----------------------------------------------------------------------------*/
void InitStartNet(void)
{

  if (TestMode == BURN_IN) {
    GetBurnInAddress(); 
  }   
  EEAddressCopy = EEAddress;
  PeripheralAddress = EEAddressCopy;

  NetIni();
  InitUsx();
  InitNetw();
}



/*******************************************************************/
/*                                                                 */
/*    InitNetw:                                                    */
/*    This function initialises the 68hc11 network for all channels*/
/*    If on of the ininitialisations or openings fail the alarm    */
/*    is set.                                                      */
/*                                                                 */
/*******************************************************************/

void InitNetw(void)
{
signed char NodeList[NodeListLengthMax];

/*NodeList[0] = PeripheralAddress;              */

  NodeList[0] = -1;
  if ((EEAddressCopy > 0) && (EEAddressCopy <= 50)) {
    NodeList[0] = EEAddressCopy;
    NodeList[1] = -1;
  }

  if (EEAddressCopy == EEAddress2) {WriteEEpromByte(0, &EEAddress2);}

  if ((EEAddress2 > 0) && (EEAddress2 <= 50)) {
    NodeList[1] = EEAddress2;
    NodeList[2] = -1;
  }


// speciale node voor de burnin kar. ( invullen van deze node is het enigste dat
// moet gebeuren). In item.c moeten de maxima worden aangepast -----------------
if (EEAddress == 61) {

  NodeList[0] = EEAddress;                         // tbv burnin
  NodeList[1] = -1;

  NetClose(CHANNEL);
  NetInit( CHANNEL,                             /* Channel           */
           BUFFERS,                             /* Number of buffers */
           BUFSIZE,                             /* Buffer size       */
           Cntrllr,                             /* Node number       */
           ArbModeVCP,                          /* Bus slave         */
           &NodeList[0] );

  NetInitStat(CHANNEL);

  if (NodeList[0] != -1) {
    while(NetOpen(CHANNEL) == -1) {
      NetClose(CHANNEL);
      NetInit( CHANNEL,                         /* Channel           */
           BUFFERS,                             /* Number of buffers */
           BUFSIZE,                             /* Buffer size       */
           Cntrllr,                             /* Node number       */
           ArbModeVCP,                          /* Bus slave         */
           &NodeList[0] );

      NetInitStat(CHANNEL);
    }
  }
} else {
  NetClose(CHANNEL);
  NetInit( CHANNEL,                             /* Channel           */
           BUFFERS,                             /* Number of buffers */
           BUFSIZE,                             /* Buffer size       */
           Cntrllr,                             /* Node number       */
           ArbModeVCP,                          /* Bus slave         */
           &NodeList[0] );

  NetInitStat(CHANNEL);

  if (NodeList[0] != -1) {
    while(NetOpen(CHANNEL) == -1) {
      NetClose(CHANNEL);
      NetInit( CHANNEL,                         /* Channel           */
           BUFFERS,                             /* Number of buffers */
           BUFSIZE,                             /* Buffer size       */
           Cntrllr,                             /* Node number       */
           ArbModeVCP,                          /* Bus slave         */
           &NodeList[0] );

      NetInitStat(CHANNEL);
    }
  }

}
}

