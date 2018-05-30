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

#pragma codeseg(ACOMMON)

non_banked void NetTxNonBanked(void) {

  NetTx();
} 

non_banked void NetRxNonBanked(void) {

  NetRx();
}   


non_banked void NetTimerTickNonBanked(void) {

  NetTimerTick();
}  
