
#include "chkerr.h"
#include "milkmisc.h"
#include "tandem.h"
#include "takeoff.h"
#include "milk.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../lib/rti_int.h"
#include "../lib/extport.h"
#include "../lib/watchdog.h"
#include "../lib/error.h"
#include "../lib/time.h"
#include "../lib/text.h"
#include "../lib/delay.h"
#include "../lib/convert.h"
#include "../lib/eehc11.h"
#include "../key/disp.h"
#include "../key/key.h"
#include "../key/keyint.h"
#include "../usx/fake_usx.h"
#include "../comm/send_msg.h"
#include "../include/romver.h"
#include "../include/romeehc1.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/per_func_test_vcp_flow.h"

#pragma codeseg(EEPAGE)


const unsigned char PROG_VRS_REQ_IC09[4] = {0x01, 0x00, 0x82 };

unsigned char ParlourType;
unsigned char IicWriteErr;


unsigned char SoftKey;

unsigned char IC13PrgVrs;
unsigned char IC09PrgVrs;
unsigned char IC08PrgVrs;
unsigned char SoftwareKey;
unsigned char ExtPortK5State;
unsigned char ExtPortK4State;
unsigned char ExtPortK3State;

unsigned char ExtPortCopy;
unsigned char ActExtPortCopy;

// =============================================================================
// deze functie set meerder MPC in reiniging, of haalt ze uit reiniging
// de 'Master' wordt als laatste aangesproken
// =============================================================================
void HandleCentralMessages( void )
{
  static unsigned short time_cm_send, time_pm_send;
  static unsigned char  j = 0xff,  i = 0xff;
           signed char  idx = GetNodeAddress(SIDE_A);
         unsigned char  buf[5];


  if ( ExternMessages & SEND_CLEAN_START_MESSAGES ){
    if (i > EERowSize ) i = EERowSize;

    if ( TimeControl( Cnt10msec, time_cm_send + 10 )){
      if ( i <= 1 ) {
        ExternMessages &= ~SEND_CLEAN_START_MESSAGES;
        KeyExtSwitch = START_CLEAN_PROCESS;  // tbv de Master
        i = EERowSize;
      } else {
        buf[0] = buf[1] = START_CLEAN_PROCESS;
        if (HndlSndMsgToMasterSlave(idx+i-1, SystKeyExtSwitchRefId, 2, buf)) i--;
        time_cm_send = Cnt10msec;
      }
    }
  } else {
    if ( ExternMessages & SEND_CLEAN_STOP_MESSAGES ){
      if (i > EERowSize ) i = EERowSize;

      if ( TimeControl( Cnt10msec, time_cm_send + 10 )){
        if ( i <= 1 ){
          ExternMessages &= ~SEND_CLEAN_STOP_MESSAGES;
          KeyExtSwitch = STOP_CLEAN_PROCESS;  // tbv de Master
          i = EERowSize;
        } else {
          buf[0] = buf[1] = STOP_CLEAN_PROCESS;
          if (HndlSndMsgToMasterSlave(idx+i-1, SystKeyExtSwitchRefId, 2, buf)) i--;
          time_cm_send = Cnt10msec;
        }
      }
    } else time_cm_send = Cnt10msec;
  }


  if ( ExternMessages & SEND_SET_PULSATION_MESSAGES ){
    if (j > EERowSize ) j = EERowSize;

    if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
      if ( j <= 1 ) {
        ExternMessages &= ~SEND_SET_PULSATION_MESSAGES;
        j = EERowSize;
      } else {
        buf[0] = buf[1] = SET_PULSATION_MESSAGE;
        if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
        time_pm_send = Cnt10msec;
      }
    }
  } else {
    if ( ExternMessages & SEND_STOP_PULSATION_MESSAGES ){
      if (j > EERowSize ) j = EERowSize;

      if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
        if ( j <= 1 ){
          ExternMessages &= ~SEND_STOP_PULSATION_MESSAGES;
          j = EERowSize;
        } else {
          buf[0] = buf[1] = STOP_PULSATION_MESSAGE;
          if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
          time_pm_send = Cnt10msec;
        }
      }
    } else {
      if ( ExternMessages & SEND_WASH_MODE_4_START_MESSAGES ){
        if (j > EERowSize ) j = EERowSize;

        if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
          if ( j <= 1) {
            ExternMessages &= ~SEND_WASH_MODE_4_START_MESSAGES;
            j = EERowSize;
            KeyExtSwitch = SET_WASH_MODE_4;
          } else {
            buf[0] = buf[1] = SET_WASH_MODE_4;
            if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
            time_pm_send = Cnt10msec;
          }
        }
      } else {
        if ( ExternMessages & SEND_WASH_MODE_3_START_MESSAGES ){
          if (j > EERowSize ) j = EERowSize;

          if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
            if ( j <= 1) {
              ExternMessages &= ~SEND_WASH_MODE_3_START_MESSAGES;
              j = EERowSize;
              KeyExtSwitch = SET_WASH_MODE_3;
            } else {
              buf[0] = buf[1] = SET_WASH_MODE_3;
              if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
              time_pm_send = Cnt10msec;
            }
          }
        } else {
          if ( ExternMessages & SEND_EXTRA_DUMP_MESSAGES ){
            if (j > EERowSize ) j = EERowSize;

            if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
              if ( j <= 1) {
                ExternMessages &= ~SEND_EXTRA_DUMP_MESSAGES;
                j = EERowSize;
                KeyExtSwitch = EXTRA_DUMP;
              } else {
                buf[0] = buf[1] = EXTRA_DUMP;
                if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
                time_pm_send = Cnt10msec;
              }
            }
          } else {
            // test op centraal cluster down message
            if ( ExternMessages & SEND_PRE_WASH_START_MESSAGES ){
              if (j > EERowSize ) j = EERowSize;

              if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
                if ( j <= 1 ) {
                  ExternMessages &= ~SEND_PRE_WASH_START_MESSAGES;
                  j = EERowSize;
                  KeyExtSwitch = SET_PRE_WASH_MODE;
                } else {
                  buf[0] = buf[1] = SET_PRE_WASH_MODE;
                  if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
                  time_pm_send = Cnt10msec;
                }
              }
            } else {
              if ( ExternMessages & SEND_WASH_MODE_I2C_START_MESSAGES ){
                if (j > EERowSize ) j = EERowSize;

                if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
                  if ( j <= 1) {
                    ExternMessages &= ~SEND_WASH_MODE_I2C_START_MESSAGES;
                    j = EERowSize;
//was al                    KeyExtSwitch = START_I2C_WASH_MODE_MESSAGE;
                  } else {
                    buf[0] = buf[1] = START_I2C_WASH_MODE_MESSAGE;
                    if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
                    time_pm_send = Cnt10msec;
                  }
                }
              } else {
                if ( ExternMessages & SEND_WASH_MODE_I2C_STOP_MESSAGES ){
                  if (j > EERowSize ) j = EERowSize;

                  if ( TimeControl( Cnt10msec, time_pm_send + 10 )){
                    if ( j <= 1) {
                      ExternMessages &= ~SEND_WASH_MODE_I2C_STOP_MESSAGES;
                      j = EERowSize;
//                      KeyExtSwitch = STOP_I2C_WASH_MODE_MESSAGE;
                    } else {
                      buf[0] = buf[1] = STOP_I2C_WASH_MODE_MESSAGE;
                      if (HndlSndMsgToMasterSlave(idx+j-1, SystKeyExtSwitchRefId, 2, buf)) j--;
                      time_pm_send = Cnt10msec;
                    }
                  }
                } else time_pm_send = Cnt10msec;
              }
            }
          }
        }
      }
    }
  }
}

// =============================================================================
// Zend een bericht naar een andere peripheral
// =============================================================================
int HndlSndMsgToMasterSlave(unsigned char dest, unsigned short msg_id,
                             unsigned char length, unsigned char *message)
{
  PerFuncTest    *PerFuncTestPtr;
  unsigned short  message_length;
  unsigned char   buffer[20];

  // er kunnen geen berichten naar zich zelf gestuurd worden
  if ( dest == EEAddress )return 1;

  // bericht wordt overgestuurd als een PerFuncTest bericht
  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = msg_id;

  message_length = 2 * sizeof(unsigned short);
  memory_cpy( (unsigned char *)&(PerFuncTestPtr->Array[0]), message, length );
  message_length += length;

  return SendMessage( ConLow, EEAddressCopy, dest, message_length, buffer );
}


// =============================================================================
void HandleIicWriteErrors(void)
{
  signed char idx = GetSideIndex(SideState);

  if ( WriteErrorInfo( 6 )){
    SendPerGenErrorRes(idx);                   // Software
  }
}


// =============================================================================
void HandleIicReadErrors(void)
{
  signed char idx = GetSideIndex(SideState);

  if ( WriteErrorInfo( 5 )){                  // Hardware
    SendPerGenErrorRes(idx);
  }
}


// =============================================================================
unsigned char HandleWriteIic( unsigned char DeviceAddress, unsigned char RegAddress,
                     unsigned char nr_of_bytes, unsigned char *sbuf)
{
  unsigned char stat   = 1;
  unsigned char sum    = 0;
  unsigned char i, length = nr_of_bytes;
  unsigned char buf[11];

  if ( (DeviceAddress == KB_ADDRESS) || (DeviceAddress == PICA_ADDRESS )){

    // bepaal de checksum, plak deze achter het bericht aan
    // verstuur het alleen voor indien bestemd voor de communicatie print
    for ( i = 0; i < length && i < 10; i++ ){
      sum += (buf[i] = *(sbuf+i));
    }
    sum += RegAddress;
    buf[i] = sum;

    if (DeviceAddress == PICA_ADDRESS)length++;

    UsxGetRes(IIC_RES);
    InitIic();

    while( stat = WriteIic( DeviceAddress, RegAddress, length, buf )){
      IicWriteErrors++;
      WatchDog();
      // probeer maximaal 5 keer om de data op te halen
      if ( TimeControl( IicWriteErrors, 5)) {
        if ( DeviceAddress == KB_ADDRESS ){
          if ( !CommunicationError ) CommunicationError = COMM_ERR_IC13;
        } else {
          if ( !CommunicationError ) CommunicationError = COMM_ERR_IC09;
        }
//        HandleIicWriteErrors();
        break;
      }
    }
    UsxRelRes(IIC_RES);
  } else {
    IicWriteErrors++;
  }
    // reset eventuele errors
  if ( !stat) {
    if ( (CommunicationError == COMM_ERR_IC13) && (DeviceAddress == KB_ADDRESS)){
      CommunicationError = 0;
    }
    if ( (CommunicationError == COMM_ERR_IC08) && (DeviceAddress == PICA_ADDRESS)){
      CommunicationError = 0;
    }
  }

  return stat;
}

// =============================================================================
unsigned char HandleReadIic(unsigned char DeviceAddress, unsigned char RegAddress,
                          unsigned char nr_of_bytes, unsigned char *rbuf)
{
  unsigned char buf[12];
  unsigned char stat    = 1;
  unsigned char i, sum  = 0;
  unsigned char length = ( ((DeviceAddress == PICA_ADDRESS) && (IC08PrgVrs >= 4))                               ||
                           ((DeviceAddress == KB_ADDRESS)   && (RegAddress == IIC_GET_KEY ) && (IC13PrgVrs>=3)) ||
                            (RegAddress == IIC_GET_PROG_VERSION_IC8 )) ? nr_of_bytes+1 : nr_of_bytes;

  if ( (DeviceAddress == KB_ADDRESS) || (DeviceAddress == PICA_ADDRESS )){
    UsxGetRes(IIC_RES);

    WatchDog();
    InitIic();

    while( stat = ReadIic( DeviceAddress, RegAddress, length, (length == (nr_of_bytes+1)) ? buf : rbuf )){
      IicReadErrors++;
      WatchDog();
      // probeer maximaal 5 keer om de data op te halen
      if ( TimeControl( IicReadErrors, 5)) {
        if ( DeviceAddress == KB_ADDRESS ){
          if ( !CommunicationError ) CommunicationError = COMM_ERR_IC13;
        } else {
          if ( !CommunicationError ) CommunicationError = COMM_ERR_IC08;
        }
//        HandleIicReadErrors();
        break;
      }
    }
    UsxRelRes(IIC_RES);
  } else {
    IicWriteErrors++;
  }

  // reset eventuele errors
  if ( !stat) {
    if ( (CommunicationError == COMM_ERR_IC13) && (DeviceAddress == KB_ADDRESS)){
      CommunicationError = 0;
    }
    if ( (CommunicationError == COMM_ERR_IC08) && (DeviceAddress == PICA_ADDRESS)){
      CommunicationError = 0;
    }
  }

  // controleer op checksum voor de communicatie PIC
  if ( (!stat ) && (length == (nr_of_bytes+1)) ){
    if ( (EEExtConMeasuring & 0x03) && ( RegAddress == IIC_ONE_WIRE_READ)) {
      // in geval van error tussen control-board en klauw staat de checksum op
      // een andere plaats
      // lengte ontvangen data plus header IIC
      nr_of_bytes = ((buf[2] + 4) < nr_of_bytes ? (buf[2] + 4) : nr_of_bytes);
    }

    for ( i = 0; (i < nr_of_bytes) && (i < 11); i++ ){
      sum += (rbuf[i] = *(buf+i));
    }
    if ( buf[i] != sum ) stat = 1;
  }
  return stat;
}

// =============================================================================
// leest of verstuurd een bericht naar het PWM-kanaal, via  PICA ( communicatie )
// =============================================================================
unsigned char PwmMessage(  unsigned char slen, unsigned char *sbuf,
                           unsigned char rlen, unsigned char *rbuf )
{
  unsigned short time = Cnt10msec;
  unsigned char  stat;

  stat = HandleWriteIic(PICA_ADDRESS, IIC_ONE_WIRE_WRITE, slen, sbuf );
  if (!stat){
    // controleer of het bericht verwerkt is
    do {
      stat = HandleReadIic(PICA_ADDRESS, IIC_ONE_WIRE_READ, rlen, rbuf );

      if ( !stat ){
          // check of PWM channel nog bezig is
        if ( rbuf[1] != 0xff ) break;

        WatchDog();
      }
      // controleer op time-out
      if ( TimeControl( Cnt10msec, time+250 )) goto exit;
    } while( rbuf[3] == 0x02 );
  }

exit:
  return stat;
}


// =============================================================================
// Haal de programma versies van de diverse PIC's op
// =============================================================================
void RequestProgVersionsPIC(void )
{
  unsigned char  stat, i;
  unsigned char  rbuf[11];

  // get program-version PIC on keyboard
  stat = HandleReadIic( KB_ADDRESS, IIC_GET_PROG_VERSION_IC13, 2, rbuf);
  if ( !stat ) {
    // de eerste versie geeft commando niet terug
    if (rbuf[0] != IIC_GET_PROG_VERSION_IC13 ) IC13PrgVrs = 1;
    else                                       IC13PrgVrs = rbuf[1];
  } else {
    // communicatie mislukt
    IC13PrgVrs = 0;
  }

  // get program-version communication PIC
  // also get 'Software Key'
  stat = HandleReadIic( PICA_ADDRESS, IIC_GET_PROG_VERSION_IC8, 3, rbuf );
  if ( !stat ){
    // de eerste versie geeft 1 terug
    if ( (rbuf[0] != IIC_GET_PROG_VERSION_IC8) || (rbuf[1] == 1)) IC08PrgVrs = 2;
    else                                                          IC08PrgVrs = rbuf[1];
  } else {
    // communicatie mislukt
    IC08PrgVrs = 0;
  }

  SoftwareKey = 0;
#if !_DEBUG_
  if ( IC08PrgVrs < 4 ){
    // oude versie, enable geleidbaarheids-meting
    SoftKey = (SK_VC4_ENABLE+SK_STIM_TRCK_ARM_ENABLE+SK_GATE_CONTROL_ENABLE+SK_FEEDING_ENABLE+SK_COND_TMP_ENABLE+SK_TANDEM_ENABLE+SK_FLW_CNTRL_PLS_ENABLE);
  } else {
    // nieuwe versie, softkey enabled
#else
//    rbuf[2] = 11;                   // standalone
//    rbuf[2] = 12;                   // VC4, SwingOver, pulsatie, kalendar, centraal in/uit reiniging, handmatige koe separatie
//    rbuf[2] = 0/*2*/;               // 12+ tandem, gate control, cow separation functions, carroussel stop, feeding, low yield stop (carr)
    rbuf[2] = 14;                   // 0+ conductivity and temp.
#endif
    SoftwareKey = rbuf[2];
    switch( rbuf[2] ){
      case 12: SoftKey = (SK_VC4_ENABLE);                                                                                                                              break;
      case  0:                              // oude standaard !
      case  2:
      case 13: SoftKey = (SK_VC4_ENABLE+SK_STIM_TRCK_ARM_ENABLE+SK_GATE_CONTROL_ENABLE+SK_FEEDING_ENABLE+SK_TANDEM_ENABLE+SK_FLW_CNTRL_PLS_ENABLE);                    break;
      case  1:
      case  3:
      case 14: SoftKey = (SK_VC4_ENABLE+SK_STIM_TRCK_ARM_ENABLE+SK_GATE_CONTROL_ENABLE+SK_FEEDING_ENABLE+SK_TANDEM_ENABLE+SK_COND_TMP_ENABLE+SK_FLW_CNTRL_PLS_ENABLE); break;
      default: SoftKey = SK_STIM_TRCK_ARM_ENABLE;                 // version CNM4MP8-11
    }
#if !_DEBUG_
  }
#endif
  // get program-version Conductivity PIC
  IC09PrgVrs = 0;

  // interne geleidbaarheid
  for ( i = 0; i < 5; i++ ){
    if ( !PwmMessage( 3, (unsigned char *)&PROG_VRS_REQ_IC09, 8, (unsigned char *)&rbuf )){
      // de eerste versie geeft een vraagteken terug ipv een nummer
      if ( rbuf[7] == '?' ) IC09PrgVrs = 1;
      else                  IC09PrgVrs = rbuf[7];
      break;
    }
  }
  if ( !IC09PrgVrs ){
    // display error zolang er niet gemolken wordt
    if (!CommunicationError) CommunicationError = COMM_ERR_IC09;
  }
}


void ReportGateSwitch(unsigned char Gate, unsigned char onOff)
{
  unsigned char  buffer[MESS_LEN(DbgMessRes)+2];
  PerFuncTest    *PerFuncTestPtr;

  void          *DbgMessResPtr = (void *)(buffer + 4);

  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);
  PerFuncTestPtr->PerFuncTestCommand = DbgMessResId;
   
  ((DbgMessRes *)DbgMessResPtr)->DbgMessErrorNr = Gate;
  ((DbgMessRes *)DbgMessResPtr)->DbgMlkState = onOff;

  SendMessage( ConLow, GetNodeAddress(SideState), 0, MESS_LEN(DbgMessRes)+2, buffer);

}


// =============================================================================
// bestuur de external Poort (PICA), tevens kan het voorkomen dat sommige hekken
// door een andere MPC bestuurd worden. Gate Control kan alleen als de juiste PIC
// is geinstalleerd.
// =============================================================================
void HndlExtPort( void)
{
  unsigned char temp = (ExtPortCopy & K5);
  unsigned char buf[4];

  if ( SoftKey & SK_GATE_CONTROL_ENABLE ){
    // TANDEM: EXIT_GATE *********************************************************
    switch( ExtPortK5State ){
      case EPK5S_ON:
           if ((EEGateK5 != EEAddress) && EEGateK5 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K5;
             buf[1] = ExtPortK5State;
             if (HndlSndMsgToMasterSlave(EEGateK5, OTExtPortWriteRefId, 2, buf )){
               ExtPortK5State = EPK5S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             // tbv pulse length
             if ( EEGateK5Time )GateK5ActiveTime = Cnt10msec;

             temp |= K5;

  //           if ( ((ParlourType == PT_AUTO_TANDEM) || (ParlourType == PT_SEMI_TANDEM)) && (OTRowSide == OT_LEFT_SIDE)){
             if ( (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) &&
                  (ParlourType >= PT_SEMI_TANDEM)              &&
                   (OTRowSide == OT_LEFT_SIDE)){
               // indien koe aanwezig en GEEN manuele bediening, hek automatisch sluiten
               // als koe weg is anders hek sluiten indien manueel bediend
               if ( (MpcOtsInfo.station_state & (OT_COW_PRESENT + OT_K3_MANUAL_CONTROL) ) == OT_COW_PRESENT ){
                 MpcOtsInfo.station_state &= ~OT_EXIT_MANUAL;
               } else MpcOtsInfo.station_state |=  OT_EXIT_MANUAL;
             }
             ExtPortK5State = EPK5S_IDLE;
           }
        break;
      case EPK5S_OFF:
           if ((EEGateK5 != EEAddress) && EEGateK5 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K5;
             buf[1] = ExtPortK5State;
             if (HndlSndMsgToMasterSlave(EEGateK5, OTExtPortWriteRefId, 2, buf )){
               ExtPortK5State = EPK5S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             temp &= ~K5;
             ExtPortK5State = EPK5S_IDLE;
           }
        break;
      case EPK5S_TOGGLE:
           if ((EEGateK5 != EEAddress) && EEGateK5 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K5;
             buf[1] = ExtPortK5State;
             if (HndlSndMsgToMasterSlave(EEGateK5, OTExtPortWriteRefId, 2, buf )){
               ExtPortK5State = EPK5S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             if ( ((ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) && (ParlourType >= PT_SEMI_TANDEM)) &&
                  (OTRowSide == OT_LEFT_SIDE)){
               if ( !(ExtPortCopy & K5) ){
                 // indien koe aanwezig en GEEN manuele bediening, hek automatisch sluiten
                 // als koe weg is anders hek sluiten indien manueel bediend
                 if ( (MpcOtsInfo.station_state & (OT_COW_PRESENT + OT_K3_MANUAL_CONTROL) ) == OT_COW_PRESENT ){
                   MpcOtsInfo.station_state &= ~OT_EXIT_MANUAL;
                 } else MpcOtsInfo.station_state |=  OT_EXIT_MANUAL;
               }
             }
             if ( temp & K5 ) temp &= ~K5;
             else {
               // tbv pulse length
               if ( EEGateK5Time )GateK5ActiveTime = Cnt10msec;
                temp |=  K5;
             }
             ExtPortK5State = EPK5S_IDLE;
           }
        break;
      case EPK5S_IDLE:
      case EPK5S_ON_OFF:
      case EPK5S_OFF_ON:
      default:
        break;
    }

    if ( temp != (ExtPortCopy & K5) ){
      ReportGateSwitch(K5_GATE, temp);
      if ( temp ) ExtPortCopy |=  K5;
      else        ExtPortCopy &= ~K5;
    }

    // ENTRANCE_GATE *************************************************************
    temp = (ExtPortCopy & K4);
    switch( ExtPortK4State ){
      case EPK4S_ON:
           if ((EEGateK4 != EEAddress) && EEGateK4 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K4;
             buf[1] = ExtPortK4State;
             if (HndlSndMsgToMasterSlave(EEGateK4, OTExtPortWriteRefId, 2, buf )){
               ExtPortK4State = EPK4S_IDLE;
             }
           } else {
             // baas over zijn eigen hek

             if ( (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) &&
                  (ParlourType >= PT_SEMI_TANDEM)              &&
                  (OTRowSide == OT_RIGHT_SIDE)){
               // indien koe aanwezig en GEEN manuele bediening, hek automatisch sluiten
               // als koe weg is anders hek sluiten indien manueel bediend
               if ( (MpcOtsInfo.station_state & (OT_COW_PRESENT + OT_K3_MANUAL_CONTROL) ) == OT_COW_PRESENT ){
                 MpcOtsInfo.station_state &= ~OT_EXIT_MANUAL;
               } else MpcOtsInfo.station_state |=  OT_EXIT_MANUAL;
             }
             // tbv pulse length
             if ( EEGateK4Time )GateK4ActiveTime = Cnt10msec;
             temp |= K4;
             ExtPortK4State = EPK4S_IDLE;
           }
        break;
      case EPK4S_OFF:
           if ((EEGateK4 != EEAddress) && EEGateK4 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K4;
             buf[1] = ExtPortK4State;
             if (HndlSndMsgToMasterSlave(EEGateK4, OTExtPortWriteRefId, 2, buf )){
               ExtPortK4State = EPK4S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             temp &= ~K4;
             ExtPortK4State = EPK4S_IDLE;
           }
        break;
      case EPK4S_TOGGLE:
           if ((EEGateK4 != EEAddress) && EEGateK4 && (ParlourType == PT_HERRING_BONE)){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K4;
             buf[1] = ExtPortK4State;
             if (HndlSndMsgToMasterSlave(EEGateK4, OTExtPortWriteRefId, 2, buf )){
               ExtPortK4State = EPK4S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             if ( (ParlourType <= PT_AUTO_TANDEM_NO_CD_SENSOR) &&
                  (ParlourType >= PT_SEMI_TANDEM)              &&
                  (OTRowSide == OT_RIGHT_SIDE)){
               if ( !(ExtPortCopy & K5) ){
                 // indien koe aanwezig en GEEN manuele bediening, hek automatisch sluiten
                 // als koe weg is anders hek sluiten indien manueel bediend
                 if ( (MpcOtsInfo.station_state & (OT_COW_PRESENT + OT_K3_MANUAL_CONTROL) ) == OT_COW_PRESENT ){
                   MpcOtsInfo.station_state &= ~OT_EXIT_MANUAL;
                 } else MpcOtsInfo.station_state |=  OT_EXIT_MANUAL;
               }
             }

             if ( temp & K4 ) temp &= ~K4;
             else {
                // tbv pulse length
               if ( EEGateK4Time )GateK4ActiveTime = Cnt10msec;
               temp |=  K4;
             }
             ExtPortK4State = EPK4S_IDLE;
           }
        break;
      case EPK4S_IDLE:
      case EPK4S_ON_OFF:
      case EPK4S_OFF_ON:
      default:
        break;
    }

    if ( temp != (ExtPortCopy & K4) ){
      ReportGateSwitch(K4_GATE, temp);
      if ( temp ) ExtPortCopy |=  K4;
      else        ExtPortCopy &= ~K4;
    }

    //
    // CENTRAL_ENTRANCE **********************************************************
    // alleen bij carrousel wordt deze niet centraal aangestuurd, bij alle ander types
    // bestaat deze mogelijkheid
    //
    temp = (ExtPortCopy & K3);
    switch( ExtPortK3State ){
      case EPK3S_ON:
           if ((EEGateK3 != EEAddress) && EEGateK3 && (ParlourType != PT_CARROUSEL) ){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K3;
             buf[1] = ExtPortK3State;
             if (HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf )){
               ExtPortK3State = EPK3S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             temp |= K3;
             // tbv pulse length
             if ( EEGateK3Time )GateK3ActiveTime = Cnt10msec;
             if ( ( ParlourTandemTypeAndMilkingMode() && ( ReadI3Sensor() == SENSOR_STATE_CLOSE ))){
                ExtPortK3State = EPK3S_WAIT_IDLE;
             } else  ExtPortK3State = EPK3S_IDLE;
           }
        break;
      case EPK3S_OFF:
           if ((EEGateK3 != EEAddress) && EEGateK3 && (ParlourType != PT_CARROUSEL) ){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K3;
             buf[1] = ExtPortK3State;
             if (HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf )){
               ExtPortK3State = EPK3S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             temp &= ~K3;
             ExtPortK3State = EPK3S_IDLE;
           }
        break;
      case EPK3S_TOGGLE:
           if ((EEGateK3 != EEAddress) && EEGateK3 && (ParlourType != PT_CARROUSEL) ){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K3;
             buf[1] = ExtPortK3State;
             if (HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf )){
               ExtPortK3State = EPK3S_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             if ( temp & K3 ) temp &= ~K3;
             else {
               temp |=  K3;
               if ( EEGateK3Time )GateK3ActiveTime = Cnt10msec;
             }
             ExtPortK3State = EPK3S_IDLE;
           }
        break;
      case EPK3S_WAIT_I3_FREE:
           // deze state toegevoegd om centrale ingangshek open te kunnen houden
           // de state blijft 'EPK3S_WAIT_IDLE'. Wijziging vindt plaats in 'tandem.c'
           if ((EEGateK3 != EEAddress) && EEGateK3 && (ParlourType != PT_CARROUSEL) ){
             // de 'master' moet het hek bedienen
             buf[0] = OT_EXT_PORT_WRITE_K3;
             buf[1] = ExtPortK3State;
             if (HndlSndMsgToMasterSlave(EEGateK3, OTExtPortWriteRefId, 2, buf )){
               ExtPortK3State = EPK3S_WAIT_IDLE;
             }
           } else {
             // baas over zijn eigen hek
             temp |=  K3;
             ExtPortK3State = EPK3S_WAIT_IDLE;
           }
        break;
      case EPK3S_IDLE:
      case EPK3S_ON_OFF:
      case EPK3S_OFF_ON:
      default:
        break;
    }

    if ( temp != (ExtPortCopy & K3) ){
      ReportGateSwitch(K3_GATE, temp);
      if ( temp ) ExtPortCopy |=  K3;
      else        ExtPortCopy &= ~K3;
    }


    // controleer of het centrale ingangs-hek eventueel weer gesloten moet worden
    if ( KeyMode == KM_MILKING ) OT_CheckCentralGateSensor();
  }

  // zorgt voor het centraal ophalen van het melken of de start van de reiniging
  // zorgt ook voor centrale aansturing pulsatie tijdens reinigen ipv I2
  if ( EERowSize ) HandleCentralMessages();

  // lees status van de External Poort
  if ( !(HandleReadIic( PICA_ADDRESS, IIC_READ_PORTB, 2, buf ) )){
    if ( buf[0] == IIC_READ_PORTB ){
      ActExtPortCopy = buf[1];
      if ( (buf[1] & (K1+K2+K3+K4+K5)) != (ExtPortCopy & (K1+K2+K3+K4+K5))){
        // poortb heeft niet de gewenste status
        buf[0] = ExtPortCopy;
        HandleWriteIic( PICA_ADDRESS, IIC_WRITE_PORTB, 1, buf );
      }
    }

    if ( !(buf[1] & 0x20 )){
      // uitgangen spanningsloos, geef message op display
      WriteErrorInfo( PWR_RELAIS_ERR_NR );
    } else {
      ResetErrorInfoBit( PWR_RELAIS_ERR_NR );
    }
    // set/reset error message 
    CheckErrors();
  }
}

