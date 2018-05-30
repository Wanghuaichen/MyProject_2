#include "milkmisc.h"
#include "tandem.h"    
#include "takeoff.h"    
#include "milk.h"    
#include "../lib/mem.h" 
#include "../lib/time.h" 
#include "../lib/schedule.h" 
#include "../lib/extport.h"
#include "../lib/rti_int.h"
#include "../lib/string.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/watchdog.h"
#include "../lib/text.h"
#include "../lib/convert.h"
#include "../key/disp.h"
#include "../key/key.h"
#include "../key/keyint.h"
#include "../include/romstnet.h"
#include "../include/romeehc1.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/per_func_test_vcp_flow.h"
 
#pragma codeseg(EEPAGE)

MPC_OTS_INFO         MpcOtsInfo;
MPC_OTM_SLAVE_INFO   MpcOtmSlaveInfo[10];
AT_MASTER_STATE      ATMasterState;
MPC_OTM_PARLOUR_INFO MpcOtmParlourInfo;
BOOLEAN              OTStartup;

unsigned short       OpenTandemStartTime;
unsigned char        OTRowMaster;
unsigned char        OTRowSide;

extern unsigned char msgTimeoutTime;

// =============================================================================
// bepaal of stal-type een open-tandem is
// =============================================================================
unsigned char ParlourTandemTypeAndMilkingMode(void) 
{
  if (((ParlourType == PT_SEMI_TANDEM)  ||
       (ParlourType == PT_AUTO_TANDEM)  ||
       (ParlourType == PT_AUTO_TANDEM_NO_CD_SENSOR) ) &&  
        KeyMode       == KM_MILKING       ) return 1;
  else                                      return 0;  
}

// =============================================================================
void OTStartFromCleaning( void)
{
  InitOpenTandem();
  
  ExtPortK4State = EPK4S_OFF;
  ExtPortK5State = EPK5S_OFF;
}


// =============================================================================
unsigned char OTGetRowSide( unsigned char address )
{
  unsigned char OtherRowMaster = EEOTRowMaster / 10;
  
  if ( ((address > EEGateK3) && EEGateK3 ) || 
       ((OTRowMaster > OtherRowMaster) && OtherRowMaster )) return OT_LEFT_SIDE;
  else                                                      return OT_RIGHT_SIDE;
}

// =============================================================================
// unsigned char SingleMaster( void)
// indien een Master return 1 else 0
// =============================================================================
unsigned char SingleMaster( void)
{
  if ( (EEOTRowMaster/10) == 0 ) return 1;
  else                           return 0;
}

  
// =============================================================================
void InitOpenTandem( void )
{
  ATMasterState       = ATM_INIT;
  OTStartup           = TRUE;
  OpenTandemStartTime = Cnt10msec;
  Sensor4Cntr         = 8;      // versnel het opstarten
  ParlourType         = EEParlourType;
  OTRowMaster         = EEOTRowMaster % 10;
  
  // bepaal aan welke zijde de MPC zich bevind
  OTRowSide = OTGetRowSide(EEAddress);


  // initieer 'station-state', hef blokkade's op 
  MpcOtsInfo.station_state     = 0;
  MpcOtsInfo.station_state_old = 0xff;

  if ( OTRowMaster == EEAddress ){
    memory_clear( (unsigned char *)(&MpcOtmSlaveInfo),   sizeof(MPC_OTM_SLAVE_INFO) *10);
    memory_clear( (unsigned char *)(&MpcOtmParlourInfo), sizeof(MPC_OTM_PARLOUR_INFO));
    MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty = OT_STALL_FREE;
  }     
}

// =============================================================================
// Sluit 'CentralGate' indien I3 gezien wordt
// =============================================================================
void OT_CheckCentralGateSensor( void )
{      
  unsigned char sensor_state = ReadI3Sensor();
  
    
  // alleen de MPC die het centrale-ingangshek bediend kijkt naar de koe sensor (I3)
  if ( ( ParlourTandemTypeAndMilkingMode() || (ParlourType == PT_OPEN_TANDEM) )  && 
       ((EEGateK3 == EEAddress)            || !EEGateK3 )                        && 
        (ExtPortCopy & K3 )                                                      ){
    
    if ( ExtPortK3State  == EPK3S_CLOSE_DELAYED ){
      if ( TimeControl( Cnt10msec, GateK3ActiveTime + (EEGateK3Time * 10))){
        ExtPortK3State = EPK3S_OFF;
      }
    } else {	
      // indien hek wordt geopend met de sensor bediend, dan moet het hek gesloten 
      // worden als de sensor vrij komt. Het kan echter ook zo zijn dat het hek 
      // handmatig geopend wordt zonder dat de sensor bediend is, in dit geval
      // moet het hek open blijven
      if ( (ExtPortK3State == EPK3S_WAIT_I3_FREE) || (ExtPortK3State == EPK3S_WAIT_IDLE)){
        if ( sensor_state == SENSOR_STATE_OPEN ){  
          if ( !(MpcOtsInfo.station_state & OT_K3_MANUAL )){
            ExtPortK3State = EPK3S_OFF;
          }
        } else {
          if ( sensor_state == SENSOR_STATE_CLOSE ){  
            if ( MpcOtsInfo.station_state & OT_K3_MANUAL ){
              OTSndMsgToMaster( OT_K3_CLEAR_MANUAL, OT_K3_CLEAR_MANUAL, OT_K3_CLEAR_MANUAL );
            }
          }    
        }       
      } else {    
        if ( sensor_state == SENSOR_STATE_CLOSE ){
        	if ( EEGateK3Time ){
        	  // wacht bepaalde tijd met sluiten van het hek ivm klemmen koe
        	  GateK3ActiveTime = Cnt10msec;
        	  ExtPortK3State   = EPK3S_CLOSE_DELAYED; 
        	} else {	
        	  ExtPortK3State = EPK3S_OFF;  
          }
        }
      }
    }
  }	
}

// =============================================================================
// stuur de MASTER_STATE naar alle SLAVE'S
// =============================================================================
void HndlSendMasterStateToAllSlaves( unsigned long state, unsigned char parl_info )
{
  unsigned char   i;
  unsigned char   buf[10];
  unsigned long  *p;
  
  p      = (unsigned long *)(&buf);
  *p     = state;
  buf[4] = parl_info;
  buf[5] = parl_info;

  // neem master state over
  MpcOtsInfo.otm_prl_info = parl_info;
  
  for ( i = OTRowMaster+1; i < (OTRowMaster+EERowSize); ){
    if (HndlSndMsgToMasterSlave(i, OTMasterSlaveStatusRefId, 6, buf ))i++;
    else TaskExit();            
  }
}

// =============================================================================
// OTCowInBox: kijk of het programma denkt dat er een koe aanwezig is
// =============================================================================
unsigned char OTCowInBox( void)
{
  if (MpcOtsInfo.station_state & OT_COW_PRESENT ) return 1;
  else                                            return 0;
}    

// =============================================================================
// OTCowInSystem: kijk of er nog een koe door exit sensor gezien moet worden
// =============================================================================
unsigned char OTCowInSystem( void)
{
  if ( MpcOtsInfo.otm_prl_info != OT_STALL_FREE ) return 1;
  else                                            return 0;
}    

// =============================================================================
// OTGateStateBlocked: returns 1 if output gate is blocked
// =============================================================================
unsigned char OTGateStateBlocked(void)
{
  if (((ParlourType == PT_AUTO_TANDEM) || (ParlourType == PT_AUTO_TANDEM_NO_CD_SENSOR) )  &&
       ScreenMode    == SM_MILKING       &&
      (IC13PrgVrs    < 3 )               &&
       MpcOtsInfo.station_state & OT_EXIT_GATE_BLOCKED ) {
         return 1;
  } else return 0;
}

// =============================================================================
// Open 'central-gate'  
// =============================================================================
void OpenCentralGate( void)
{ 
  unsigned char   buf[4], r_val = 0;
  BOOLEAN         open_gate = FALSE;

  if ( !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE)){
    if ( !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & (OT_K3_GATE_BLOCKED+OTM_COW_TRAFFIC_STOPPED)) ) {
      buf[0] = OT_EXT_PORT_WRITE_K3;
      buf[1] = EPK3S_ON; 
      while(HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf)==0)TaskExit();            
    }
  } 
}

// =============================================================================
// Controleer of 'central gate' gesloten is  
// =============================================================================
unsigned char CentralGateOpened(void)
{
  unsigned char r_val = 0;
  unsigned char time;

  time = Cnt10msec + 100;
  do {  
    if( (MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE )) { r_val = 1; break; }
  } while ( TimeControl( Cnt10msec, time )); 

  return r_val;
}


// =============================================================================
// Close 'central-gate'  
// =============================================================================
void CloseCentralGate(void)
{ 
  unsigned char   buf[4];
  
  if ( !(MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL_CONTROL )){
    buf[0] = OT_EXT_PORT_WRITE_K3;
    buf[1] = EPK3S_OFF; 
    while(HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf)==0)TaskExit();
  }
}

// =============================================================================
// Controleer of 'central gate' gesloten is  
// =============================================================================
unsigned char CentralGateClosed(void)
{
  unsigned char  r_val = 0; 
  unsigned short time; 

  time = Cnt10msec + 100;
  do {
    if( !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE )) { r_val = 1; break; }
  } while ( TimeControl( Cnt10msec, time )); 

  return r_val;
}

// =============================================================================
// kijk of betreffende ingangs-hek gesloten is, hou rekening met tandem zijde
// =============================================================================
unsigned char EntranceGateClosed( unsigned char station )
{
  unsigned char side  = OTGetRowSide(station);
 
  if (  !(MpcOtmSlaveInfo[station-OTRowMaster].station_state & 
         (side == OT_LEFT_SIDE ? OT_K4_GATE_STATE : OT_K5_GATE_STATE) )) return 1;
  else                                                                   return 0;
}  

// =============================================================================
// kijk of betreffende uitgangs-hek gesloten is, hou rekening met tandem zijde
// =============================================================================
unsigned char ExitGateClosed( unsigned char station )
{
  unsigned char side  = OTGetRowSide(station);

  if ( !(MpcOtmSlaveInfo[station-OTRowMaster].station_state & 
        (side == OT_LEFT_SIDE ? OT_K5_GATE_STATE : OT_K4_GATE_STATE) )) return 1;
  else                                                                  return 0;      
}


// =============================================================================
// HandleK4/K5Gate: Openen en sluiten van K4 en K5  
// =============================================================================
void HandleK4K5Gate( unsigned char station, unsigned char command, unsigned char control, unsigned char gate )
{
  unsigned char   buf[4];
  unsigned short  time;
  unsigned char   mask;
  unsigned char   side = OTGetRowSide(station);
  
  if ( station == OTRowMaster ){
    if ( (( side == OT_LEFT_SIDE)  && (gate == OT_ENTR_GATE )) ||
         (( side == OT_RIGHT_SIDE) && (gate == OT_EXIT_GATE )) ){
      ExtPortK4State = control == OT_OPEN_GATE ? EPK4S_ON : EPK4S_OFF; 
      mask = K4; 
    } else { 
      ExtPortK5State = control == OT_OPEN_GATE ? EPK5S_ON : EPK5S_OFF; 
      mask = K5; 
    }

    if ( !(MpcOtsInfo.station_state & OT_K3_MANUAL_CONTROL )){
      time  = Cnt10msec + OT_MSG_CONFIRM_TIME;
      while ( control == OT_OPEN_GATE ? (!(ExtPortCopy & mask)) : (ExtPortCopy & mask) ) {
        WatchDog();
        TaskExit();
        if ( TimeControl( Cnt10msec, time ))break;            // wacht maximaal 1,5 sec       
      }    
    }
  } else {
      // stuur bericht naar desbetreffende slave
    if ( (( side == OT_LEFT_SIDE)  && (gate == OT_ENTR_GATE )) ||
         (( side == OT_RIGHT_SIDE) && (gate == OT_EXIT_GATE )) ){
      buf[0] = OT_EXT_PORT_WRITE_K4;
      buf[1] = control == OT_OPEN_GATE ? EPK4S_ON : EPK4S_OFF; 
      mask   = OT_K4_GATE_STATE;
    } else {
      buf[0] = OT_EXT_PORT_WRITE_K5;
      buf[1] = control == OT_OPEN_GATE ? EPK5S_ON : EPK5S_OFF; 
      mask   = OT_K5_GATE_STATE;
    }    

    while(HndlSndMsgToMasterSlave(station, OTExtPortWriteRefId, 2, buf) == 0)TaskExit();            

    // wacht tot hek geopend is
    time  = Cnt10msec + OT_MSG_CONFIRM_TIME;
    if ( (ATMasterState > ATM_WAIT_FOR_START) && (command == OT_MSG_CONFIRM) ){
      while( control == OT_OPEN_GATE ? (!(MpcOtmSlaveInfo[station-OTRowMaster].station_state & mask )) :
                                         (MpcOtmSlaveInfo[station-OTRowMaster].station_state & mask )) {
        WatchDog();
        TaskExit();
        if ( TimeControl( Cnt10msec, time ))break;        // wacht maximaal 1,5 sec       
      }  
    }
  }
}    


    
// =============================================================================
// kijk of er geen ingangs-hek open staat na het te openen uitgangshek
// =============================================================================
unsigned char EntranceGateOpenTowardsEnd( unsigned char station )
{
  unsigned char i;

  unsigned char side = OTGetRowSide ( station + OTRowMaster );
  
  // indien er twee row-master zijn moet er altijd van 0 - station gecheckt worden.
  // In geval van een row-master moet de linker-zijde anders behandeld worden 
  if ( side == OT_LEFT_SIDE && SingleMaster() ){
    for ( i = (EEGateK3+1-OTRowMaster); i < station;  i++ ){  
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV) ) continue;
      if (   MpcOtmSlaveInfo[i].station_state & OT_K4_GATE_STATE )       return i+OTRowMaster;
    } 
  } else {
    // Still check for left or right side	
    for ( i = 0; i <= station;  i++ ){  
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV) ) continue;
      if (   MpcOtmSlaveInfo[i].station_state & (side == OT_LEFT_SIDE ? OT_K4_GATE_STATE : OT_K5_GATE_STATE) )  return i+OTRowMaster;
    }
  }        
  return 0;
}

// =============================================================================
// kijk of er geen uitgangshek open staat voor het te openen ingangshek
// =============================================================================
unsigned char ExitGateOpenTowardsBegin( unsigned char station )
{
  signed char i;

  unsigned char side = OTGetRowSide ( station + OTRowMaster );

  // indien er twee row-master zijn moet er altijd van 0 - station gecheckt worden.
  // In geval van een row-master moet de linker-zijde anders behandeld worden 
  if ( side == OT_LEFT_SIDE && SingleMaster() ){
    for ( i = EERowSize-1; i > (EEGateK3+1-OTRowMaster);  i-- ){  
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV) ) continue;
      if (   MpcOtmSlaveInfo[i].station_state & OT_K5_GATE_STATE )       return i+OTRowMaster;
    }
  } else {
    for ( i = (EEGateK3-OTRowMaster); i >= 0;  i-- ){  
      // Still check for left or right side	
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV) ) continue;
      if (   MpcOtmSlaveInfo[i].station_state & (side == OT_LEFT_SIDE ? OT_K5_GATE_STATE : OT_K4_GATE_STATE) )  return i+OTRowMaster;
//      if (   MpcOtmSlaveInfo[i].station_state & OT_K4_GATE_STATE )       return i+OTRowMaster;
    }
  }        
  return 0;
}
    


// =============================================================================
// status van een slave, nodig voor MASTER 
// =============================================================================
void HndlRcvOTSStateRes(NETIO_MAIL *MailPtr) 
{
  unsigned char i;
  
  OTStateRes *OTStateResPtr = ( OTStateRes *)(&(MailPtr->message_start)+2);

  if (MailPtr->message_length < (sizeof(OTStateRes)+sizeof(unsigned short))) return;
  
  // bewaar data, bewaar tevens de tijd van binnenkomst 
  // sommige bits van 'station_state' worden door de master gecontroleerd 
  i = (MailPtr->address_src) - OTRowMaster;
  MpcOtmSlaveInfo[i].station_state  = OTStateResPtr->station_state;
  MpcOtmSlaveInfo[i].ot_milk_state  = OTStateResPtr->ot_milk_state;
  MpcOtmSlaveInfo[i].ot_trf_att     = OTStateResPtr->ot_trf_att;
  MpcOtmSlaveInfo[i].msg_time       = Cnt1sec;
  
  // reset 'WaitForStationState' bit 
  if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV) ){
    MpcOtmSlaveInfo[i].station_state |= OTM_STATION_STATE_RCV;
  }  
}  


// =============================================================================
// bepaal eerst mogelijke stand waar de koe terecht kan ------------------------
// =============================================================================
unsigned char GetEntranceGateToWaitFor( void )
{
    signed char i;
  unsigned char side;

  // zoek eerste open 'EntranceGate', start vanaf de SLAVE met het 'CentralGate' 
  for ( i = EERowSize-1; i >= 0;  i-- ){  
    side = OTGetRowSide(i+OTRowMaster);
    if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV )) continue;
    if (   MpcOtmSlaveInfo[i].station_state & (side == OT_LEFT_SIDE ? OT_K4_GATE_STATE : OT_K5_GATE_STATE) ) return i+OTRowMaster;
  } 
  return 0;
}

// =============================================================================
// bepaal eerst mogelijke vrije stand ------------------------------------------
// =============================================================================
unsigned char GetFirstFreeStand( void )
{
  signed char i;

  // de vul richting is afhankelijk van op welke MPC het proces gestart is
  // stations waar geen status van binnen is worden niet behandeld
  if ( MpcOtmSlaveInfo[0].station_state & OTM_MASTER_START ){
    for ( i = 0; i < EERowSize;  i++ ){
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV ))                   continue;
      if ((!(MpcOtmSlaveInfo[i].station_state & (OT_COW_PRESENT+OT_ENTR_GATE_BLOCKED)) )){
        if ( ExitGateOpenTowardsBegin(i)) break;
        else return i+OTRowMaster;
      }
    }
  } else {
    for ( i = EERowSize-1; i >= 0;  i-- ){  
      if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV ))                   continue;
      if ((!(MpcOtmSlaveInfo[i].station_state & (OT_COW_PRESENT+OT_ENTR_GATE_BLOCKED)) )){
        if ( ExitGateOpenTowardsBegin(i)) break;
        else return i+OTRowMaster;
      }
    }
  }  
  return 0;
}



// =============================================================================
// bepaal of alle standen hun status hebben opgestuurd
// =============================================================================
unsigned char NotAllStationStatesReceived ( void )
{
  signed char i;
 
  for ( i = (EERowSize-OTRowMaster); i >= 0;  i-- ){  
    if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV ))return i+OTRowMaster;
  }
  return 0;
}


// =============================================================================
// kijk of er een exit-gate geopened is  
// =============================================================================
unsigned char CheckOnAnyExitGateOpened( void)
{
  unsigned char i;
  unsigned char side;
  
  for ( i = 0; i <= (EERowSize-1); i++ ){
    side = i > ( EEGateK3-OTRowMaster) ? OT_LEFT_SIDE : OT_RIGHT_SIDE;
    if ( side == OT_LEFT_SIDE ){
      if ( MpcOtmSlaveInfo[i].station_state & OT_K5_GATE_STATE ) return i+OTRowMaster;
    } else {
      if ( MpcOtmSlaveInfo[i].station_state & OT_K4_GATE_STATE ) return i+OTRowMaster;
    }
  }
  return 0;
}

// =============================================================================
// stuur de status naar de Master
// =============================================================================
void HndlSendOTSStateRes( void )
{
  unsigned char   buf[8];
  unsigned long  *p;
  
  p      = (unsigned long *)(&buf);
  *p     = MpcOtsInfo.station_state;
  buf[4] = MpcOtsInfo.ot_milk_state;
  buf[5] = MpcOtsInfo.ot_trf_att;
    
  while(HndlSndMsgToMasterSlave(OTRowMaster, OTStateResId, 6, buf) == 0)TaskExit();            
}   


// =============================================================================
// MASTER_STATE ontvangen, enkele bits zijn van belang
// =============================================================================
void HndlRcvMasterState( NETIO_MAIL *MailPtr )
{
  OTMasterSlaveStatusRef *OTMasterSlaveStatusRefPtr = ( OTMasterSlaveStatusRef *)(&(MailPtr->message_start)+2);

  if (MailPtr->message_length < (sizeof(OTMasterSlaveStatusRef)+sizeof(unsigned short))) return;

  MpcOtsInfo.station_state &= ~OTMS_MASTER_MASK;
  MpcOtsInfo.station_state |= ((OTMasterSlaveStatusRefPtr->OTM_station_state) & OTMS_MASTER_MASK);
  MpcOtsInfo.otm_prl_info   =   OTMasterSlaveStatusRefPtr->OTM_parl_info;
}


// =============================================================================
// Send bericht naar een Slave
// =============================================================================
void OTSndMsgToSlave(  unsigned char dest, unsigned char message )
{
  unsigned char buf[4];

  if ( dest == EEAddress ){
    // eigen address, zet message op display
    switch( message){
      case OT_MSG_EXIT_GATE_BLOCKED: DisplayMessage(TXT_EXIT_GATE_LOCKED);  break;
      case OT_MSG_ENTR_GATE_BLOCKED: DisplayMessage(TXT_ENTR_GATE_LOCKED);  break;
      case OT_MSG_EXIT_OPEN:         DisplayMessage(TXT_EXIT_GATE_OPEN);    break;
      case OT_MSG_ENTR_OPEN:         DisplayMessage(TXT_ENTR_GATE_OPEN);    break;
      case OT_MSG_NO_ENTR_OPEN:      DisplayMessage(TXT_NO_ENTR_OPEN);      break;
    }
  } else {    
    buf[0] = message;
    buf[1] = message;
    while(HndlSndMsgToMasterSlave(dest, OTMessageRefId, 2, buf)==0)TaskExit();            
  }
}

// =============================================================================
unsigned char OTCowToLeave( unsigned char station )
{
  unsigned char side = OTGetRowSide( station+OTRowSide );
  
  if (((MpcOtmSlaveInfo[station].station_state  & 
       ( side == OT_LEFT_SIDE ? ( OT_COW_PRESENT+OT_EXIT_GATE_BLOCKED+OT_WAIT_COW_MILKING+OT_K5_GATE_STATE):
                                ( OT_COW_PRESENT+OT_EXIT_GATE_BLOCKED+OT_WAIT_COW_MILKING+OT_K4_GATE_STATE))) == OT_COW_PRESENT ) && 
              (  MpcOtmSlaveInfo[station].ot_milk_state == MLK_DONE )             &&
              ( (MpcOtmSlaveInfo[station].ot_trf_att & EEOutputBlockBits )== 0)   ){
         return 1;
  } else return 0;
}

// =============================================================================
// Kijk of op een bepaalde stand de koe er uitgelaten mag/moet worden
// =============================================================================
unsigned char CheckForCowToLeave( void )
{
  unsigned char r_val = 0;
  signed char i;
  
  if ( (ParlourType == PT_AUTO_TANDEM) || ParlourType == (PT_AUTO_TANDEM_NO_CD_SENSOR) ) {
    if ( MpcOtmParlourInfo.leave_way_empty == OT_STALL_FREE ){
      if ( MpcOtmSlaveInfo[0].station_state & OTM_MASTER_START ){
        for ( i = 0; i <= (EEGateK3-OTRowMaster);  i++ ){
          if ( OTCowToLeave( i )) { 
            if ( EntranceGateOpenTowardsEnd(i) ) break;
            else { r_val = OTRowMaster+i;        break; }
          }
        }
        // Leftside && SingleMaster, gaat ook goed bij twee Masters
        if ( !r_val ){
          for ( i = (EEGateK3+1-OTRowMaster); i < EERowSize;  i++ ){
            if ( OTCowToLeave( i )) { 
              if ( EntranceGateOpenTowardsEnd(i) ) break;
              else { r_val = OTRowMaster+i;        break; }
            }
          }
        }    
      } else {
        // slave start
        for ( i = (EEGateK3-OTRowMaster); i >= 0;  i-- ){  
          if ( OTCowToLeave( i )) { 
            if ( EntranceGateOpenTowardsEnd(i) ) break;
            else { r_val = OTRowMaster+i;        break; }
          }
        } 
        if ( !r_val ){
          for ( i = EERowSize-1; i >= (EEGateK3+1-OTRowMaster);  i-- ){  
            if ( OTCowToLeave( i )) { 
              if ( EntranceGateOpenTowardsEnd(i) ) break;
              else { r_val = OTRowMaster+i;        break; }
            }
          } 
        }
      }
    }    
  }
  return r_val;
}  

// =============================================================================
// Send bericht naar de Master
// called from KeyTask
// =============================================================================
void OTSndMsgToMaster( unsigned char command, unsigned char dest, unsigned char source )
{
  unsigned char buf[4];
  
  if ( EEAddress == OTRowMaster ) {
    // OpenTandem Master, bericht hoeft niet verstuurd te worden, moet
    // wel worden afgehandeld!!
    switch (command){
      case OT_OPEN_EXIT_GATE:                     // K5_GATE
           // exit mag niet geopend worden terwijl koe nog gemolken wordt
           if ( ((!(MpcOtmSlaveInfo[dest-OTRowMaster].station_state & OT_EXIT_GATE_BLOCKED )) &&
                  (MpcOtmSlaveInfo[dest-OTRowMaster].ot_milk_state == MLK_DONE ))             ||
                  (MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL_CONTROL )                  ){
             if ( EntranceGateOpenTowardsEnd(dest-OTRowMaster) ){
               // geef status weer op display van de source
               OTSndMsgToSlave( source, OT_MSG_ENTR_OPEN );
             } else {
               if ( ParlourType == PT_AUTO_TANDEM ){
                 if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
                   MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
                   HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
                 }
               }
               // open EXIT_GATE remote MPC
               HandleK4K5Gate( dest, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_EXIT_GATE );
             }  
           } else {
             // geef status weer op display
             OTSndMsgToSlave( source, OT_MSG_EXIT_GATE_BLOCKED );
           }         
        break;
      case OT_OPEN_ENTRANCE_GATE:                 // K4_GATE
           if ( !(MpcOtmSlaveInfo[dest-OTRowMaster].station_state & OT_ENTR_GATE_BLOCKED )){
             if ( ExitGateOpenTowardsBegin(dest-OTRowMaster) ){
               // geef status weer op display van de source
               OTSndMsgToSlave( source, OT_MSG_EXIT_OPEN );
             } else {          
               // Open ENTRANCE_GATE remote MPC
               HandleK4K5Gate( dest, OT_NO_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
             }
           } else {
             if ( MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL_CONTROL ){
               HandleK4K5Gate( dest, OT_NO_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
             } else { 
               // geef status weer op display
               OTSndMsgToSlave( source, OT_MSG_ENTR_GATE_BLOCKED );
             }
           }    
        break;  
      case OT_COW_TRAFFIC_INIT:
           MpcOtsInfo.station_state |= OTM_COW_TRAFFIC_INIT;

           // kijk of koeverkeer al gestart is, zoniet start deze dan vanaf de MASTER
           if ( !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT) ){
             // koeverkeer gestart op de MASTER, laat alle salve's dit weten      
             MpcOtmSlaveInfo[0].station_state |= OTM_MASTER_START;
             MpcOtsInfo.station_state         |= OTM_MASTER_START;
             MpcOtmSlaveInfo[0].station_state |= OTM_COW_TRAFFIC_INIT;
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty);
           }    
        break;
      case OT_COW_TRAFFIC_STOP:
           MpcOtsInfo.station_state |= OTM_COW_TRAFFIC_STOPPED;

           if ( !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)){
             // koe verkeer wordt gestopt
             MpcOtmSlaveInfo[0].station_state |= OTM_COW_TRAFFIC_STOPPED;
            
             // stuur de state van de MASTER naar alle slave's aan de zelfde kant
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
           }          
        break;
      case OT_COW_TRAFFIC_START:
           MpcOtsInfo.station_state &= ~OTM_COW_TRAFFIC_STOPPED;

           if ( (MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)){
             // koe verkeer kan weer worden gestart
             MpcOtmSlaveInfo[0].station_state &= ~OTM_COW_TRAFFIC_STOPPED;
            
             // stuur de state van de MASTER naar alle slave's aan de zelfde kant
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
           }
           // open 'central gate' indien er een ingangshek open
           if ( GetEntranceGateToWaitFor()) ExtPortK3State = EPK3S_ON;                          
        break;  
      case OT_K3_SET_MANUAL:
           MpcOtsInfo.station_state |= OT_K3_MANUAL;

           if ( !(MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL)){
             MpcOtmSlaveInfo[0].station_state |= OT_K3_MANUAL;
            
             // stuur de state van de MASTER naar alle slave's aan de zelfde kant
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
             ExtPortK3State = EPK3S_WAIT_I3_FREE;  // sluit hek nadat I3 vrij is
           }
        break;  
      case OT_K3_CLEAR_MANUAL:
           MpcOtsInfo.station_state &= ~OT_K3_MANUAL;

           if ( (MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL)){
             MpcOtmSlaveInfo[0].station_state &= ~OT_K3_MANUAL;
            
             // stuur de state van de MASTER naar alle slave's aan de zelfde kant
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
           }
        break;  
      case OT_TOGGLE_K3:
           if ( !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE  )){
             // CentralGate dicht kijk of die open mag
             if ( GetEntranceGateToWaitFor() ){
               ExtPortK3State = EPK3S_ON;
             } else { 
               // zend bericht naar afzender dat er geen ingang open staat
               OTSndMsgToSlave( OTRowMaster, OT_MSG_NO_ENTR_OPEN );
             }   
           } else {
             // sluiten mag altijd
             ExtPortK3State = EPK3S_OFF;
           }        
        break;
      case OT_SET_COW_EXPECTED:
           if ( ParlourType == PT_AUTO_TANDEM ){
             if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
               MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
               HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
             }
           }
        break;
    } // end switch (command)         
  } else {
    buf[0] = command;
    buf[1] = dest;
    buf[2] = source;
    while(HndlSndMsgToMasterSlave( OTRowMaster, OTCommandRefId, 4, buf)==0)TaskExit();            
  }
}

// =============================================================================
// Bericht voor remote MPC via de Master
// called from FlowControl task, should not call HndlSendMasterStateToAllSlaves directly
// because StareRes messages from slaves are not handle while sending 'MasterState'
// Set BOOLEAN instead, TANDEM_TASK will handle it 
// =============================================================================
void HndlRcvOTCommandRef(NETIO_MAIL *MailPtr)
{
   
  OTCommandRef *OTCommandRefPtr = ( OTCommandRef *)(&(MailPtr->message_start)+2);
  
  // alleen de Master moet met dit bericht iets doen, en de lengte moet goed zijn
  // deze  routine wordt vanuit FlowControl aangeroepen, bij het terugsturen afkomstig van 
  // een Slave mag er niet gewacht worden op een bevestiging!! (HANG_UP) 
  if ( !OTRowMaster || (EEAddress != OTRowMaster)) return;
  if (MailPtr->message_length < (sizeof(OTCommandRef)+sizeof(unsigned short))) return;
 
  switch( OTCommandRefPtr->OTCommand ){
    case OT_OPEN_EXIT_GATE:                     // K5_GATE
         // exit-gate mag alleen geopend worden als de navolgende ingangs-hekken
         // gesloten zijn, dus ook als alle hekken gesloten zijn! 
         if ( (OTCommandRefPtr->OTCommandDest - OTRowMaster) > (EERowSize-1) )   return;
         if ( OTCommandRefPtr->OTCommandDest < OTRowMaster ) return;
 
         // bestemd voor deze master
         if( MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT ){
           if ( ((!(MpcOtmSlaveInfo[OTCommandRefPtr->OTCommandDest-OTRowMaster].station_state & OT_EXIT_GATE_BLOCKED )) &&
                  (MpcOtmSlaveInfo[OTCommandRefPtr->OTCommandDest-OTRowMaster].ot_milk_state == MLK_DONE ))             ||
                  MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL_CONTROL ){
             
             if ( EntranceGateOpenTowardsEnd(OTCommandRefPtr->OTCommandDest-OTRowMaster) ){ 
               // geef status weer op display
               OTSndMsgToSlave( OTCommandRefPtr->OTCommandSource, OT_MSG_ENTR_OPEN );
             } else {
               // open EXIT_GATE remote MPC
               if ( ParlourType == PT_AUTO_TANDEM ){
                 if ( MpcOtmSlaveInfo[OTCommandRefPtr->OTCommandDest-OTRowMaster].station_state & OT_COW_PRESENT ){
                   if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
                     MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
                     HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
                   }
                 }
               } 
               HandleK4K5Gate( OTCommandRefPtr->OTCommandDest, OT_NO_MSG_CONFIRM, OT_OPEN_GATE, OT_EXIT_GATE );
             }
           } else {
             // geef status weer op display: ExitGate is geblokkeerd
             OTSndMsgToSlave( OTCommandRefPtr->OTCommandSource, OT_MSG_EXIT_GATE_BLOCKED );
           }
         }            
      break;
    case OT_OPEN_ENTRANCE_GATE:
         // entrance-gate mag alleen geopened worden als voorgaande uitgangen
         // gesloten zijn
         if ( (OTCommandRefPtr->OTCommandDest - OTRowMaster) > (EERowSize-1) )   return;
         if ( OTCommandRefPtr->OTCommandDest < OTRowMaster ) return;

         if( MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT ){
           if ( !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)){
             if ( !(MpcOtmSlaveInfo[OTCommandRefPtr->OTCommandDest-OTRowMaster].station_state & OT_ENTR_GATE_BLOCKED )){
               if (ExitGateOpenTowardsBegin(OTCommandRefPtr->OTCommandDest-OTRowMaster) ){
                 // geef status weer op display: Er is nog een eerdere uitgang open
                 OTSndMsgToSlave( OTCommandRefPtr->OTCommandDest, OT_MSG_EXIT_OPEN );
               } else {
                 // Open ENTRANCE_GATE remote MPC
                 HandleK4K5Gate( OTCommandRefPtr->OTCommandDest, OT_NO_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
               }    
             } else {
               // geef status weer op display: EntranceGate is geblokkeerd
               OTSndMsgToSlave( OTCommandRefPtr->OTCommandDest, OT_MSG_ENTR_GATE_BLOCKED );
             }    
           } else {
             if ( MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL_CONTROL ){
               // Open ENTRANCE_GATE remote MPC
               HandleK4K5Gate( OTCommandRefPtr->OTCommandDest, OT_NO_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
             } else {            
               // CowTraffic stopped, geef boodschap op display 
               OTSndMsgToSlave( OTCommandRefPtr->OTCommandDest, TXT_COW_TRAFFIC_STOPPED );  
             }
           }        
         }           
      break;  
    case OT_COW_TRAFFIC_INIT:
         // check op het 'OTM_COW_TRAFFIC_INIT" bit, copieer naar MASTER 
         // zodat de MASTER weet dat koe verkeer gestart is
         if ( !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT) ){
           MpcOtmSlaveInfo[0].station_state |= OTM_COW_TRAFFIC_INIT;
           MpcOtsInfo.station_state         |= OTM_COW_TRAFFIC_INIT;
           
           // stuur nieuwe status naar alle slave's
           HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty);
         }
      break;
    case OT_COW_TRAFFIC_STOP:
         if( MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT ){
           if ( !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)){
             MpcOtmSlaveInfo[0].station_state |= OTM_COW_TRAFFIC_STOPPED;
             MpcOtsInfo.station_state         |= OTM_COW_TRAFFIC_STOPPED;
             
             // stuur nieuwe status naar alle slave's
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty);
           }    
         }    
      break;         
    case OT_COW_TRAFFIC_START:
         if( MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT ){
           if ( MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED ){
             MpcOtmSlaveInfo[0].station_state &= ~OTM_COW_TRAFFIC_STOPPED;
             MpcOtsInfo.station_state         &= ~OTM_COW_TRAFFIC_STOPPED;
             
             // stuur nieuwe status naar alle slave's
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty);
           }    
           // open 'central gate' indien er een ingangshek open
           if ( GetEntranceGateToWaitFor()) ExtPortK3State = EPK3S_ON;                          
         }    
      break;         
    case OT_K3_SET_MANUAL:
         MpcOtsInfo.station_state |= OT_K3_MANUAL;

         if ( !(MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL)){
           // central-gate moet met hand bediend worden, open hek
           MpcOtmSlaveInfo[0].station_state |= OT_K3_MANUAL;
             
           // stuur de state van de MASTER naar alle slave's aan de zelfde kant
           HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
           ExtPortK3State = EPK3S_WAIT_I3_FREE;  // sluit hek nadat I3 vrij is
         }
      break;  
    case OT_K3_CLEAR_MANUAL:
         MpcOtsInfo.station_state &= ~OT_K3_MANUAL;

         if ( (MpcOtmSlaveInfo[0].station_state & OT_K3_MANUAL)){
           //
           MpcOtmSlaveInfo[0].station_state &= ~OT_K3_MANUAL;
            
           // stuur de state van de MASTER naar alle slave's aan de zelfde kant
           HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
         }
      break;  
    case OT_TOGGLE_K3:
         if ( !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE  )){
           // CentralGate dicht kijk of die open mag
           if ( GetEntranceGateToWaitFor() ){
             ExtPortK3State = EPK3S_ON;
           } else { 
             // zend bericht naar afzender dat er geen ingang open staat
             OTSndMsgToSlave( OTCommandRefPtr->OTCommandSource, OT_MSG_NO_ENTR_OPEN );
           }
         } else {
           // sluiten mag altijd
           ExtPortK3State = EPK3S_OFF;
         }        
      break;
    case OT_SET_COW_EXPECTED:
         if ( ParlourType == PT_AUTO_TANDEM ){
           if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
             MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
             HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
           }
         }
      break;

  } // end switch( OTCommandRefPtr->OTCommand )
}  

// =============================================================================
// handel ingedrukte toets af
// =============================================================================
void HandleKeyOpenTandem( unsigned char key, unsigned char key_hold )
{

  switch (key) {
    case '6':
         // omschakelen kan alleen maar als het een OpenTandem stal is en tevens de Master
         if ( key_hold ){ 
           if ( OTRowMaster == EEAddress ){
             ParlourType = PT_AUTO_TANDEM_NO_CD_SENSOR;
             DisplayMessage(TXT_MSG_FULL_AUTO_NO_SENSOR);
             goto ot_send_state;
           }
         }      
      break;   
    case '5':
         // omschakelen kan alleen maar als het een OpenTandem stal is en tevens de Master
         if ( key_hold ){ 
           if ( OTRowMaster == EEAddress ){
             ParlourType = PT_AUTO_TANDEM;
             DisplayMessage( TXT_MSG_FULL_AUTO );
             goto ot_send_state;
           }
         }      
      break;   
    case '4':
         if ( key_hold ){ 
           if ( OTRowMaster == EEAddress ){
             ParlourType = PT_SEMI_TANDEM;
             DisplayMessage( TXT_MSG_SEMI_AUTO );
         
ot_send_state:
             // master heeft weer volledige controle over CentralGate
             MpcOtsInfo.station_state &= ~(OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED);
             if ( MpcOtmSlaveInfo[0].station_state & (OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED) ){
               MpcOtmSlaveInfo[0].station_state &= ~(OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED);
               HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty = OT_STALL_FREE);
             }    
           }
         }  
      break;      
    case '3':
         if ( key_hold ){ 
           if ( OTRowMaster == EEAddress ){
             ParlourType = PT_SEMI_TANDEM;  
             DisplayMessage( TXT_MSG_MANUAL_CONTROL );
         
             // master heeft weer volledige controle over CentralGate
             if ( (MpcOtmSlaveInfo[0].station_state & (OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED)) !=
                                                      (OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED)  ){
               MpcOtsInfo.station_state         |= (OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED);
               MpcOtmSlaveInfo[0].station_state |= (OT_K3_MANUAL_CONTROL+OTM_COW_TRAFFIC_STOPPED);
               HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty = OT_STALL_FREE);
             }
           }
         }    
      break;   

    case KEY_SHIFT_0:
         // noodoplssing gooi ingangshek open
         // open ENTRANCE_GATE op aandringen van de boer 
         MpcOtsInfo.station_state |= OT_EMERGENCY_STOP;
         MpcOtsInfo.station_state &= ~OT_ENTR_GATE_BLOCKED;
         if ( OTRowSide == OT_LEFT_SIDE ){
           ExtPortK4State = EPK4S_ON;             // open entrance gate              
         } else {
           ExtPortK5State = EPK5S_ON;             // open entrance gate              
         }    
      break;
      
    case KEY_SHIFT_1:
    case KEY_SHIFT_2:
    case KEY_SHIFT_3:
    case KEY_SHIFT_4:
    case KEY_SHIFT_5:
    case KEY_SHIFT_6:
    case KEY_SHIFT_7:
    case KEY_SHIFT_8:
    case KEY_SHIFT_9:
         // Remote CowRelease from any MPC (OPEN_TANDEM)
         if ( (ParlourType >= PT_SEMI_TANDEM) && (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) ){
           // stuur een bericht via de Master naar een ander slave, eventueel
           // komt er een message terug, indien actie niet lukt
            OTSndMsgToMaster( OT_OPEN_EXIT_GATE, key-KEY_SHIFT_0, EEAddress);
         }
      break;

    case KEY_SHIFT_K3_GATE:        // CENTRAL_GATE ****************
         // dmv deze toetsen combinatie wordt het hek gesloten en
         // geblokkeerd voor 'normale' bediening.  'OpenTandem' 
         // stop het koe-verkeer, sluit tevens de 'CentralGate'
         ExtPortK3State = EPK3S_OFF;           // sluit 'central gate'
         OTSndMsgToMaster( OT_COW_TRAFFIC_STOP, OT_COW_TRAFFIC_STOP, OT_COW_TRAFFIC_STOP );
         
         // blocked, geef boodschap op display 
         DisplayMessage( TXT_COW_TRAFFIC_STOPPED ); 
      break;

    case KEY_SHIFT_K4_GATE :      
         // sluiten van een hek gaat altijd, hoeft niet via de MASTER
         if ( OTRowSide == OT_LEFT_SIDE ){
           // ENTRANCE_GATE ****************
           // indien hek al dicht, zet hem dan op slot
           if ( !(ExtPortCopy & K4) ){
             MpcOtsInfo.station_state |= OT_ENTR_GATE_BLOCKED;
         
             // blocked, geef boodschap op display 
             DisplayMessage( TXT_ENTR_GATE_LOCKED ); 
           }
           MpcOtsInfo.station_state &= ~OT_EMERGENCY_STOP;
           ExtPortK4State = EPK4S_OFF;            // sluit entrance gate              
         } else {
           // EXIT_GATE ********************
           // indien hek al dicht, zet hem dan op slot
           if ( !(ExtPortCopy & K4) ){
             MpcOtsInfo.station_state |= OT_EXIT_GATE_BLOCKED;
         
             // blocked, geef boodschap op display 
             DisplayMessage( TXT_EXIT_GATE_LOCKED ); 
           }
           ExtPortK4State = EPK4S_OFF;            // sluit exit gate              
         }    
      break;

    case KEY_SHIFT_K5_GATE :
         // sluiten van een hek gaat altijd, hoeft niet via de MASTER
         // EXIT_GATE **********************
         if ( OTRowSide == OT_LEFT_SIDE ){
           if ( !(ExtPortCopy & K5) ){
             MpcOtsInfo.station_state |= OT_EXIT_GATE_BLOCKED;
         
             // blocked, geef boodschap op display 
             DisplayMessage( TXT_EXIT_GATE_LOCKED ); 
           }
           ExtPortK5State = EPK5S_OFF;            // sluit exit gate              
         } else {
           // ENTRANCE_GATE ****************
           if ( !(ExtPortCopy & K5) ){
             MpcOtsInfo.station_state |= OT_ENTR_GATE_BLOCKED;
         
             // blocked, geef boodschap op display 
             DisplayMessage( TXT_ENTR_GATE_LOCKED ); 
           }
           MpcOtsInfo.station_state &= ~OT_EMERGENCY_STOP;
           ExtPortK5State = EPK5S_OFF;            // sluit entrance gate              
         }     
      break;

    case KEY_K3_GATE:                // CENTRAL GATE *******************  
         if (msgTimeoutTime == 0){
           // comm error; owner can switch gate
           if (EEGateK3 == EEAddress){
           	 ExtPortK3State = EPK3S_TOGGLE;
           }	
         } else {
           if ( MpcOtsInfo.station_state & OT_K3_MANUAL_CONTROL ){
             OTSndMsgToMaster( OT_TOGGLE_K3, EEAddress, EEAddress );
           } else {    
             if( key_hold ) { 
               if ( MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED ){
                 // start koeverkeer opnieuw, MASTER moet eventueel central-gate openen
                 OTSndMsgToMaster( OT_COW_TRAFFIC_START, OT_COW_TRAFFIC_START, OT_COW_TRAFFIC_START );
               } else {
                 // koe klem in central-gate of handmatig openen van het centrale
                 // ingangshek. Werking afhankelijk of I3 actief is of niet    
                 OTSndMsgToMaster( OT_K3_SET_MANUAL, OT_K3_SET_MANUAL, OT_K3_SET_MANUAL );
               }    
             } else {
               // start cow traffic, kan alleen vanaf MASTER of eerste SLAVE  
               if ( EEGateK3 && (!(MpcOtsInfo.station_state & OTM_COW_TRAFFIC_INIT ))){    
                 OTSndMsgToMaster( OT_COW_TRAFFIC_INIT, OT_COW_TRAFFIC_INIT, OT_COW_TRAFFIC_INIT );
               } else {
                 // reset eventuele handbediening 
                 // 'MASTER' sluit het hek indien nodig
                 if ( MpcOtsInfo.station_state & OT_K3_MANUAL ){
                   OTSndMsgToMaster( OT_K3_CLEAR_MANUAL, OT_K3_CLEAR_MANUAL, OT_K3_CLEAR_MANUAL );
                 }  
               }    
             }    
           }
         }    
      break;

    case KEY_K4_GATE :               
         if ( OTRowSide == OT_LEFT_SIDE ){
           // ENTRANCE GATE ******************
           if (!key_hold){
             if (  (MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED) &&
                 (!(MpcOtsInfo.station_state & OT_K3_MANUAL_CONTROL))  ){
               // CowTraffic stopped, geef boodschap op display 
               DisplayMessage( TXT_COW_TRAFFIC_STOPPED );  
             } else {         
               // openen ingangshek moet via de MASTER 
               OTSndMsgToMaster( OT_OPEN_ENTRANCE_GATE, EEAddress, EEAddress );
             }
           } else {
             // open ENTRANCE_GATE op aandringen van de boer 
             MpcOtsInfo.station_state &= ~OT_ENTR_GATE_BLOCKED;
             ExtPortK4State = EPK4S_ON;             // open entrance gate              
           }              
         } else {
           // EXIT GATE  ********************
           if (!key_hold){
             // openen EXIT_GATE moet via de MASTER 
             OTSndMsgToMaster( OT_OPEN_EXIT_GATE, EEAddress, EEAddress );
           } else {
             // laat koe eruit, op aandringen van de boer
             // reset eventueel manueel bediend
             MpcOtsInfo.station_state &= ~(OT_EXIT_GATE_BLOCKED+OT_WAIT_COW_MILKING);
             ExtPortK4State = EPK4S_ON;             // open exit gate              
             OTSndMsgToMaster( OT_SET_COW_EXPECTED, OT_SET_COW_EXPECTED, OT_SET_COW_EXPECTED );
           }  
         }    
      break;
    case KEY_K5_GATE :                
         if ( OTRowSide == OT_LEFT_SIDE ){
           // EXIT GATE  ******************** 
           if (!key_hold){
             // openen EXIT_GATE moet via de MASTER 
             OTSndMsgToMaster( OT_OPEN_EXIT_GATE, EEAddress, EEAddress );
           } else {
             // laat koe eruit, op aandringen van de boer
             // reset eventueel manueel bediend
             MpcOtsInfo.station_state &= ~(OT_EXIT_GATE_BLOCKED+OT_WAIT_COW_MILKING);
             ExtPortK5State = EPK5S_ON;             // open exit gate              
             OTSndMsgToMaster( OT_SET_COW_EXPECTED, OT_SET_COW_EXPECTED, OT_SET_COW_EXPECTED );
           }  
         } else {
           // ENTRANCE GATE ******************
           if (!key_hold){
             if (  (MpcOtsInfo.station_state & OTM_COW_TRAFFIC_STOPPED) &&
                 (!(MpcOtsInfo.station_state & OT_K3_MANUAL_CONTROL))  ){
               // CowTraffic stopped, geef boodschap op display 
               DisplayMessage( TXT_COW_TRAFFIC_STOPPED );  
             } else {         
               // openen ingangshek moet via de MASTER 
               OTSndMsgToMaster( OT_OPEN_ENTRANCE_GATE, EEAddress, EEAddress );
             }
           } else {
             // open ENTRANCE_GATE op aandringen van de boer 
             MpcOtsInfo.station_state &= ~OT_ENTR_GATE_BLOCKED;
             ExtPortK5State = EPK5S_ON;             // open entrance gate              
           }              
         }    
      break;

  }
}        

// =============================================================================
// Open tandem afhandeling, SEMI en FULL AUTOMATIC
// =============================================================================
void HndlTandem(void)
{

  unsigned short cow_detec_time;
  BOOLEAN        time_set = FALSE;
  unsigned char  i;
  unsigned char  buf[4];
  unsigned char  sensor_state;
  
  while (1)
  {
    if ( KeyMode == KM_MILKING ){

      // -----------------------------------------------------------------------      
      // AUTO_TANDEM, SLAVE afhandeling 
      // -----------------------------------------------------------------------
  
      // SLAVE: stuur veranderingen in de status door naar de master 
      sensor_state = ReadI4Sensor();
      
      switch ( ActExtPortCopy & (K4+K5) ){
        case K5:
             if ( OTRowSide == OT_LEFT_SIDE ){  
               // Uitgangshek open, sluiten indien koe niet meer gezien wordt 
               // er zit wel een delay op 
               if ( sensor_state == SENSOR_STATE_OPEN ){  
                 if ( time_set ){
                   if ( TimeControl( Cnt10msec, cow_detec_time + (EEGateSwitchTime * 10))){  
                     MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                 
                     // indien geen koe aanwezig ten tijde het hek werd geopend, dan
                     // hek manueel bedienen 
                     if ( !(MpcOtsInfo.station_state & OT_EXIT_MANUAL)){
                       ExtPortCopy &= ~K5;
                     }  
                   }
                 } else {
                   time_set = TRUE;
                   cow_detec_time = Cnt10msec;
                 }         
               } else  time_set = FALSE;
             } else {
               // Ingangshek open, sluiten indien koe gezien wordt. Behalve bij 
               // manuele bediening of nood stop
               if ( sensor_state == SENSOR_STATE_CLOSE ){  
                 if ( time_set ){
                   if ( TimeControl( Cnt10msec, cow_detec_time + (EEGateSwitchTime * 10))){  
                     MpcOtsInfo.station_state |= OT_COW_PRESENT;
                     MpcOtsInfo.station_state |= OT_WAIT_COW_MILKING;
                     if ( !(MpcOtsInfo.station_state & (OT_K3_MANUAL_CONTROL+OT_EMERGENCY_STOP)))ExtPortCopy &= ~K5;
                   }
                 } else {
                   time_set = TRUE;
                   cow_detec_time = Cnt10msec;
                 }         
               } else {
                 if ( sensor_state == SENSOR_STATE_OPEN ){  
                   MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                 }
                 time_set = FALSE;           
               }     
             }  
          break;     
        case K4:                         
             if ( OTRowSide == OT_LEFT_SIDE ){  
               // Ingangshek open, sluiten indien koe gezien wordt 
               if ( sensor_state == SENSOR_STATE_CLOSE ){  
                 if ( time_set ){
                   if ( TimeControl( Cnt10msec, cow_detec_time + (EEGateSwitchTime * 10))){  
                     MpcOtsInfo.station_state |= OT_COW_PRESENT;
                     MpcOtsInfo.station_state |= OT_WAIT_COW_MILKING;
                     if ( !(MpcOtsInfo.station_state & (OT_K3_MANUAL_CONTROL+OT_EMERGENCY_STOP)))ExtPortCopy &= ~K4;
                   }
                 } else {
                   time_set = TRUE;
                   cow_detec_time = Cnt10msec;
                 }         
               } else {
                 if ( sensor_state == SENSOR_STATE_OPEN ){  
                   MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                 }
                 time_set = FALSE;           
               }
             } else {
               // Uitgangshek open, sluiten indien koe niet meer gezien wordt 
               // er zit wel een delay op 
               if ( sensor_state == SENSOR_STATE_OPEN ){  
                 if ( time_set ){
                   if ( TimeControl( Cnt10msec, cow_detec_time + (EEGateSwitchTime * 10))){  
                     MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                 
                     // indien geen koe aanwezig ten tijde het hek werd geopend, dan
                     // hek manueel bedienen 
                     if ( !(MpcOtsInfo.station_state & OT_EXIT_MANUAL)){
                       ExtPortCopy &= ~K4;
                     }  
                   }
                 } else {
                   time_set = TRUE;
                   cow_detec_time = Cnt10msec;
                 }         
               } else  time_set = FALSE;
             }           
          break;
        case (K4+K5):
             // In- en uitgangshek open 
             time_set = FALSE;
          break;     
        default:
             // bepaal of er een koe staat bij het opstarten, warme of koude start
             if ( OTStartup == TRUE ){
               if ( !TimeControl( Cnt10msec,  OpenTandemStartTime+300) ){
                 if ( sensor_state == SENSOR_STATE_CLOSE ){  
                   MpcOtsInfo.station_state |= OT_COW_PRESENT;
                 } else {
                   if ( sensor_state == SENSOR_STATE_OPEN ){  
                     MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                   }
                 }            
               } else {
                 OTStartup = FALSE;
               }
             }     
             time_set = FALSE;    
          break;     
      } // end switch( ExtPortCopy .. ) 

      // kijk of er een koe de stal verlaat 
      sensor_state = ReadI3Sensor();
      if ( ( sensor_state == SENSOR_STATE_CLOSE) &&
           (OTRowMaster == EEAddress)            &&
            MpcOtmParlourInfo.leave_way_empty == OT_COW_EXPECT ){
        MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty = OT_COW_SEEN;
      } else {
        if ( (sensor_state == SENSOR_STATE_OPEN ) &&  
           (OTRowMaster == EEAddress)             &&
            MpcOtmParlourInfo.leave_way_empty == OT_COW_SEEN ){
          MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty = OT_STALL_FREE;
          HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
        }
      }
                    
      // indien de status van de standplaats wijzigd stuur dit dan naar de master 
      // stuur minimaal elke 5 minuten de actuele status op 
      MpcOtsInfo.station_state &= ~(K3+K4+K5);
      MpcOtsInfo.station_state |= (ExtPortCopy & (K3+K4+K5));
   
      if ( (MpcOtsInfo.station_state != MpcOtsInfo.station_state_old) ||
           (MpcOtsInfo.ot_milk_state != MlkState)                     ||
           (MpcOtsInfo.ot_trf_att    != GetTransferAtt() )            ||     
            TimeControl( Cnt1sec, MpcOtsInfo.req_send_time + 300 )){

        // controleer of de aanwezige koe gemolken wordt, indien het geval reset
        // desbetreffende bit
        if ( (MlkState == MLK_MEASURING) && (MpcOtsInfo.station_state & OT_WAIT_COW_MILKING)){
          MpcOtsInfo.station_state &= ~OT_WAIT_COW_MILKING;
        }    
        
        // stuur open-tandem status en de melk status 
        MpcOtsInfo.station_state_old  = MpcOtsInfo.station_state;
        MpcOtsInfo.ot_milk_state      = MlkState;
        MpcOtsInfo.req_send_time      = Cnt1sec;
        MpcOtsInfo.ot_trf_att         = GetTransferAtt();
      
        
        // stuur data naar de MASTER
        if ( EEAddress == OTRowMaster ){
          // enkele bits worden door het 'master-process' beheerd 
          MpcOtmSlaveInfo[0].station_state  = MpcOtsInfo.station_state;
          MpcOtmSlaveInfo[0].ot_milk_state  = MpcOtsInfo.ot_milk_state;
          MpcOtmSlaveInfo[0].ot_trf_att     = MpcOtsInfo.ot_trf_att;
        } else {  
          if ( MpcOtsInfo.station_state & OTM_POWER_UP ){
            // stuur status op naar de MASTER 
            HndlSendOTSStateRes();
          }
        }    
      }


      // -----------------------------------------------------------------------
      // (SEMI)AUTO_TANDEM, MASTER afhandeling 
      // -----------------------------------------------------------------------
      if ( OTRowMaster == EEAddress ){

        // set 'StationStateRcv' bit van de MASTER
        if ( !(MpcOtmSlaveInfo[0].station_state & OTM_STATION_STATE_RCV )){
          MpcOtmSlaveInfo[0].station_state |= OTM_STATION_STATE_RCV;
        }  
         
      
        switch ( ATMasterState ){
          case ATM_INIT:                                  // ***** STATE 1  ****
               // wacht even een 2 seconden voordat alle MPC's worden afgevraagd 
               if ( !( MpcOtsInfo.station_state & OTM_POWER_UP )){
                 // stuur een bericht naar de slave's als indicatie dat de 
                 // MASTER on-line is
                 MpcOtsInfo.station_state         |= OTM_POWER_UP;
                 MpcOtmSlaveInfo[0].station_state |= OTM_POWER_UP;

                 HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
               }    
               
               if ( TimeControl( Cnt10msec,  OpenTandemStartTime+300) ){
                 for ( i = 0; i <= (EEGateK3-OTRowMaster); ){
                   // vraag van alle slave's de status op
                   if ( !(MpcOtmSlaveInfo[i].station_state & OTM_STATION_STATE_RCV )){
                     if (HndlSndMsgToMasterSlave(i+OTRowMaster, OTStateRefReqId, 0, buf )) i++;
                     else TaskExit();            
                   } else {
                     i++;
                   }
                 }
                
                 MpcOtsInfo.station_state &= ~OT_COW_PRESENT;
                 MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty = OT_STALL_FREE;
                 ATMasterState = ATM_CHECK_MPC_STATES; 
               } else {  
                 if ( !(NotAllStationStatesReceived()) ){ 
                   ATMasterState = ATM_CHECK_MPC_STATES; 
                 }
               } 
            break;
  
          case ATM_CHECK_MPC_STATES:                      // ***** STATE 2  ****
               if ( !(NotAllStationStatesReceived()) ){ 
                 // states from all slave's received 
                 // altijd koude start: sluit alle K4/K5 hekken
                 // sluit ook het centrale ingangshek
                 CloseCentralGate();
                   
                 for ( i = EEGateK3; i >= OTRowMaster; i-- ) HandleK4K5Gate(i, OT_NO_MSG_CONFIRM, OT_CLOSE_GATE, OT_ENTR_GATE);
                 for ( i = EEGateK3; i >= OTRowMaster; i-- ) HandleK4K5Gate(i, OT_NO_MSG_CONFIRM, OT_CLOSE_GATE, OT_EXIT_GATE);
                 ATMasterState = ATM_WAIT_FOR_START;
               } else {
                 // process is gestart
                 if (  MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT) {
                   ATMasterState = ATM_WAIT_FOR_START;
                 }                 
               }           
            break;

          case ATM_WAIT_FOR_START:                        // ***** STATE 3  ****
               // wacht tot het koeverkeer gestart wordt, controleer of alle hekken dicht zijn,
               // open vervolgens een ingangshek en tenslotte het centrale ingangshek
               if (  (MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_INIT) && 
                   (!(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)) ){ 

                 // bepaal van af welke zijde er begonnen moet worden
                 if ( MpcOtmSlaveInfo[0].station_state & OTM_MASTER_START ){
                   // begin bij de MASTER, dus alle hekken van de voorgaande stands
                   // moeten gesloten blijven
                   HandleK4K5Gate(OTRowMaster, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE);
                 } else {     
                   // begin bij de MPC het dichtst bij het centrale ingangshek omdat
                   // deze als eerste de juiste status moet terug leveren
                   HandleK4K5Gate( OTRowMaster+EERowSize-1, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
                 }     
                 OpenCentralGate();
                 if ( CentralGateOpened() ) ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED;
               }              
            break;       

          case ATM_WAIT_LEAVE_WAY_EMPTY:                  // ***** STATE 4  ****
               // wacht tot gang vrij is, of centrale ingangshek manueel geopend 
               
               if ( NotAllStationStatesReceived() != EEGateK3 ){ 
                 if ( MpcOtmParlourInfo.leave_way_empty == OT_STALL_FREE ){  
                   // bepaal op welke stand de koe verwacht wordt 
                   if ( MpcOtmParlourInfo.gate_to_wait_for = GetEntranceGateToWaitFor() ){
                     OpenCentralGate();
                     if ( CentralGateOpened() )  ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED;
                     else                        ATMasterState = ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE;
                   } else {
                     // geen ingangs-hekken open: kijk of er een stand vrij is  
                     // en open vervolgens dit hek mits koeverkeer niet gestopt is  
                     if ( (MpcOtmParlourInfo.gate_to_wait_for = GetFirstFreeStand()) &&
                          (!(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)) ){ 
                       HandleK4K5Gate( MpcOtmParlourInfo.gate_to_wait_for, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
                       OpenCentralGate();
                       if ( CentralGateOpened() ) ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED;
                       else                       ATMasterState = ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE;
                     } else {
                       // alle standen bezet en hekken dicht, wacht tot er een koe
                       // uitgemolken is 
                       ATMasterState = ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE;
                     }  
                   }    
                 }    
               }   
            break;
          
          case ATM_WAIT_ENTRANCE_GATE_CLOSED:             // ***** STATE 5  ****
               // hou in de gate of er tussentijds een hek gesloten is, handmatig
            
               if (MpcOtmParlourInfo.gate_to_wait_for == 0){
                  MpcOtmParlourInfo.gate_to_wait_for = GetEntranceGateToWaitFor();
               }
               
               if ( EntranceGateClosed( MpcOtmParlourInfo.gate_to_wait_for ) || 
                    (MpcOtmParlourInfo.gate_to_wait_for == 0) ){
                 // kijk of er nog een stand vrij is
                 if ( MpcOtmParlourInfo.gate_to_wait_for = GetEntranceGateToWaitFor() ){
                   // nog steeds een stand vrij, open centale ingangs hek 
                   OpenCentralGate();
                 } else {
                   // geen open standen meer (alle ingangs-hekken dicht) wacht tot er 
                   // weer een koe uitgelaten wordt
                   if ( CheckOnAnyExitGateOpened() ) ATMasterState = ATM_WAIT_FOR_EXIT_GATE_CLOSED;
                   else                              ATMasterState = ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE;
                 }  
               } else {
                 // ingang is nog open, uitgemolken koeien, achter deze, mogen eruit
                 // gelaten worden (OF ANDER RIJ)
                 if( i = CheckForCowToLeave() ){
                   HandleK4K5Gate(i, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_EXIT_GATE);
                   if ( ParlourType == PT_AUTO_TANDEM ){
                        // full automatic with sensor
                     if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
                       MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
                       HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
                     }
                   }
                 }    
               }      
            break;
           
          case ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE:         // ***** STATE 6  ******
               // kijk of er een hek geopened is (manueel)
               if ( MpcOtmParlourInfo.gate_to_wait_for = CheckOnAnyExitGateOpened() ){
                 ATMasterState = ATM_WAIT_FOR_EXIT_GATE_CLOSED;
               } else {
                 if ( (MpcOtmParlourInfo.gate_to_wait_for = GetFirstFreeStand()) &&
                      (!(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED)) ){ 
                   HandleK4K5Gate( MpcOtmParlourInfo.gate_to_wait_for, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
                   OpenCentralGate();
                   ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED;
                 } else {
                   // controleer of 'central-gate' gesloten is
                   if (   MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_GATE_STATE &&
                        !(MpcOtmSlaveInfo[EEGateK3-OTRowMaster].station_state & OT_K3_MANUAL )   ){  
                     CloseCentralGate(); 
                   }                 
                   // controleer of de Master een uitgang moet openen, gebeurt aan de
                   // hand van de MelkStatus en StationStatus (AUTO_TANDEM) 
                   if ( i = CheckForCowToLeave() ){
                     // laat de koe eruit, volgende koe eerst melken voor ze eruit
                     // gelaten wordt. MelkStatus blijft MILK_DONE
                     HandleK4K5Gate(i, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_EXIT_GATE);
                     
                     if ( ParlourType == PT_AUTO_TANDEM ){
                       // full automatic with sensor    
                       if ( MpcOtmParlourInfo.leave_way_empty != OT_COW_EXPECT ){
                         MpcOtsInfo.otm_prl_info = MpcOtmParlourInfo.leave_way_empty  = OT_COW_EXPECT;
                         HndlSendMasterStateToAllSlaves(MpcOtmSlaveInfo[0].station_state, MpcOtmParlourInfo.leave_way_empty ); 
                       }
                     }
                     
                     MpcOtmParlourInfo.gate_to_wait_for = i;
                     ATMasterState = ATM_WAIT_FOR_EXIT_GATE_CLOSED;
                   }
                 }     
               }  
            break;
          
          case ATM_WAIT_FOR_EXIT_GATE_CLOSED:             // ***** STATE 7  ****
               if ( MpcOtmParlourInfo.gate_to_wait_for = GetEntranceGateToWaitFor() ) {
                 ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED;
               } else {
                 // wacht tot desbetreffende uitgangshek gesloten is 
                 if ( MpcOtmParlourInfo.gate_to_wait_for = CheckOnAnyExitGateOpened() ){
                   if ( ExitGateClosed( MpcOtmParlourInfo.gate_to_wait_for )){
                     // de entrance van deze stand kan in ieder geval geopend worden
                     if ( !GetEntranceGateToWaitFor()                                   && 
                          !(MpcOtmSlaveInfo[0].station_state & OTM_COW_TRAFFIC_STOPPED) &&
                          !(MpcOtmSlaveInfo[MpcOtmParlourInfo.gate_to_wait_for-OTRowMaster].station_state & OT_ENTR_GATE_BLOCKED) ){
                       HandleK4K5Gate( MpcOtmParlourInfo.gate_to_wait_for, OT_MSG_CONFIRM, OT_OPEN_GATE, OT_ENTR_GATE );
                       OpenCentralGate();
                     } 
                     ATMasterState = ATM_WAIT_ENTRANCE_GATE_CLOSED; 
                   }
                 } else {
                   ATMasterState = ATM_CHECK_SLAVES_FOR_COW_TO_LEAVE;
                 }       
               }  
            break;
          default:
               ATMasterState = ATM_INIT;
            break;   
        } // end switch ( ATMasterState )
      } // MASTER AFHANDELING
    } else {
      // zet station in default stand
      if ( KeyMode == KM_CLEANING ){
        switch ( ATMasterState ){
          case ATM_CLEANING: break;
          default:
               // zet hekken in default positie
               ExtPortK4State = EPK4S_OFF;
               ExtPortK5State = EPK5S_OFF;
               if ((EEGateK3 == EEAddress) && EEGateK3 ) ExtPortK3State = EPK3S_OFF;
             
               ATMasterState = ATM_CLEANING;
            break;
        }
      }
    }     
    TaskExit();
  } // end while(1)
}


 
