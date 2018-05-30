/*****************************************************************/
/* Memolac Milkmeter                                            */
/****************************************************************/

#include "memolac.h"
#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/time.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h"
#include "../lib/intr6811.h"
#include "../lib/stdlib.h"
#include "../lib/extport.h"
#include "../lib/ascibin.h"
#include "../key/key.h"
#include "../key/keyint.h"
#include "../appl/takeoff.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../appl/pulsation.h"

#define SU_BIT_TIME        1666                    // 1200 baud
#define VFLOW_CLEAN_OK        1
#define VFLOW_TEMP_OK      0x01
#define VFLOW_COND_OK      0x02
#define VFLOW_CON_T         100
#define VFLOW_TMP_T         600
#define SU_MIN_FRAME_LENGTH  11

#define SIZEOF_SU_RBUF 120

#pragma codeseg(EEPAGE)


unsigned short suStartTime;
unsigned short suPulseLength;
unsigned char  suRBits;
unsigned char  suState;

unsigned char  suByte;
unsigned char  suBuf[61];
unsigned char  suBufPntr;
unsigned char  suDataAvailable;

unsigned char  suRBuf[SIZEOF_SU_RBUF];
unsigned short suWp;
unsigned short suRp;

unsigned short VflowMlkAmountStart;
unsigned short VflowMlkAmountRestart;
unsigned short VflowMlkAmountEnd;
  signed short VflowTemperature;
unsigned char  VflowConductivity;
unsigned char  suNrOfTICS;
unsigned short suTimeBuffer[10];
unsigned char  suTimePntr;
unsigned short suErrorCount;
unsigned short suTestErr;
unsigned char  suCleanOk;
unsigned char  suMsgType;
unsigned short suSuccess;
unsigned short suActFlow;

// =============================================================================
// InitUart
//
// Initiates a software UART of input I1
// =============================================================================
void InitUart(void)
{
  // 7 bits, even parity, 1 start/stop; 1200 Baud
  // set interrupt on rising edge; clears flags; enables IC1 interrupt
  TCTL2  = 0;
  TCTL2  = EDG1A;                         // set rising edge
  TMSK1 &= ~OC1I;                         // disable interrupt
  TFLG1  = IC1F+OC1F;                     // clear flag
  TMSK1 |= IC1I;                          // enable input capture
}


// =============================================================================
// HandleSuByte
//
// Handles byte read
// returns number of items (5bytes) available
// =============================================================================
unsigned char HandleSuByte( unsigned char b)
{
  unsigned char rval =0;

  switch ( b ){
   case  'v':
         suBufPntr = 0;
         suBuf[suBufPntr++] = b;
     break;
//   case  0x10:
   case  0x0D:
         if ( suBufPntr >= SU_MIN_FRAME_LENGTH){
           suDataAvailable = suBufPntr;
           rval = suDataAvailable;
         }
         suBufPntr=0;
     break;
   default:
         if ( suBufPntr <= 60){
           suBuf[suBufPntr++] = b;
         } else {
           // not the start of a new message
           suDataAvailable = 0;             //suBufPntr/5;
           suBufPntr=0;
         }
     break;
  } // end switch
  return rval;
}


// =============================================================================
// ChecksuData
//
// Checks the checksum of the bytes received
// =============================================================================
unsigned char ChecksuData(void)
{
  unsigned char  i, rval = 1;
  unsigned short sum, c;

  for( sum = 0, i = 0; i < (suDataAvailable-6); i++){
    sum+= suBuf[i];
  }
  if (suBuf[i] == 's'){
    c = ascbin(&suBuf[i+1], 4);

    if ( c == sum) return 0;
  } else {
    if (suBuf[i] == 'D'){
      // old string; always accept
      suMsgType = 'D';
      return 0;
    }
  }
  return rval;
}

// =============================================================================
// convertTimesToByte
//
// Convert saved times to byte
// =============================================================================
unsigned char convertTimesToByte(void)
{
  unsigned char i, byt, bits;
  unsigned short t;

  if ( suNrOfTICS < 2) return 0;

  // includes start bit
  for (bits=0,byt = 0,i = 0; i < suNrOfTICS-1; i++){
    t = suTimeBuffer[i+1] - suTimeBuffer[i];
    if (!i){
      if (t > SU_BIT_TIME) t -= SU_BIT_TIME;
      else                 t = 0;
    }
    while( t > SU_BIT_TIME ){
      t -= SU_BIT_TIME;
      if (i&0x01)byt |= (0x01<<bits);
      bits++;
    }
    if ( t > ((SU_BIT_TIME*8)/10)) {
      if (i&0x01)byt |= (0x01<<bits);
      bits++;
    }
  }
  return byt;
}

// =============================================================================
// HndlVflowM3Int
//
// Save time of input change and set next OC interrupt 
// =============================================================================
void HndlVflowM3Int (void)
{
  TFLG1  = IC1F+OC1F;                     // clear flag
  TCTL2  = EDG1A+EDG1B;                   // set rising edge
  TOC1   = TCNT+((95L*SU_BIT_TIME)/10);    // 9,5 bits;
  TMSK1 |= OC1I;                          // enable OC1 interrupt

  suTimeBuffer[suTimePntr++]= TIC1;
  if( suTimePntr > 10) suTimePntr= 0;
}



// =============================================================================
// HndlVflowOCInt
//
// Converts received times to byte
// =============================================================================
void HndlVflowOCInt(void)
{
  suNrOfTICS = suTimePntr;
  suTimePntr = 0;

  InitUart();
  if (suWp >= SIZEOF_SU_RBUF) suWp = 0;
  suRBuf[suWp++] = convertTimesToByte();

  if (suWp == suRp)suTestErr++; 	
}

// =============================================================================
// HandleVFlow
//
// Handles data received from VFlow
// input c: determins if milking is active or not
// =============================================================================
void HandleVFlow(unsigned char c)
{
  unsigned char  b;
  unsigned short x,y,i;

  while (suWp != suRp){
    if (suRp >= SIZEOF_SU_RBUF)suRp = 0;
    suDataAvailable = HandleSuByte(suRBuf[suRp++]);
    if ( suDataAvailable ){
      // check checksum received data
      if (ChecksuData()){
        suErrorCount++;
        return;
      } else {
        suSuccess++;
      }

      b = 0;
      for ( i = 0; i < suDataAvailable; i+=5 ){
        x = y = ascbin(&suBuf[i+1], 4);

        if ( x != 0xFFFF ){
          switch (suBuf[i]){
            case  'W':  // Weight; range 0..9999; resolution 10gr
                  if ( x <= 6500){
                    if ( c){
                      // test if vflow runs over
                      y = (y >= VflowMlkAmountStart) ? ( y-VflowMlkAmountStart) : ((6500-VflowMlkAmountStart)+y);
                      y = ((unsigned long)EECorVFlow*(unsigned long)y)/100;

                      if ( ((MlkState >= MLK_NEUTRAL) && (MlkState <= MLK_TAKE_OFF_WAIT)) || (MlkState == MLK_TAKE_OFF_MAN) ){
                        MlkAmount = y*10;
                        MlkAmountKg = (MlkAmount+50)/100;
                      }
                    } else {
                      VflowMlkAmountStart = x;
                    }
                  }
              break;
            case  'F':  // Flow; range 0..9999; 10gr/min
                  x = x*10;
                  if ( x > MlkMaxFlow ) MlkMaxFlow = x;
                  suActFlow = x;
              break;
            case  'T':  // Temperature external; range 0..9999
                  VflowTemperature = x;
                  if ( x > VFLOW_TMP_T ) b |= VFLOW_TEMP_OK;
              break;
            case  'P':  // Pressure; range 0..9999
              break;
            case  'C':  // Conductivity; range 0..9999
                  VflowConductivity = (x > 255) ? 255 : x;
                  if ( x > VFLOW_CON_T ) b |= VFLOW_COND_OK;
              break;
            case  'R':  // RED colour component; range 0..9999
                  x =y;
              break;
            case  'G':  // GREEN colour component; range 0..9999
                  x =y;
              break;
            case  'B':  // BLUE colour component; range 0..9999
                  x =y;
              break;
            case  'S':  // Scale value; range 0..9999
              break;
            case  'D':  // Digital inputs
              break;
            case  't':
              break;
          } // end switch (suBuf[i]

        }
      } // end for( i = 0; i < suDataAvailable; ..
      if ( b == ( VFLOW_TMP_T+VFLOW_COND_OK)) suCleanOk = VFLOW_CLEAN_OK;
    }
  }
}


// =============================================================================
// VFlowMMDone
// handles modes: MM_INIT(0)
// =============================================================================
void VFlowMMInit(void)
{
  // ResetFloatPosition();
  VflowMlkAmountRestart = VflowMlkAmountStart;
  StoredMlkAmount = 45;
  MlkAmount = 45;
  MlkAmountKg = 0;
  MlkDuration = 0;
  MlkActFlow = 0;
  MlkMaxFlow = 0;

  // Set Interrupt registers
  suDataAvailable = 0;
  suTimePntr = 0;
  suBufPntr=0;
  suErrorCount = 0;
  suTestErr = 0;
  suSuccess = 0;
  suCleanOk = 0;
  suMsgType = 's';
  InitUart();
}

// =============================================================================
// VFlowMMDone
// milk mode: MM_MEAS(1)
// handles milk states : MLK_IDLE(0) 
//                       MLK_SEND_WAIT(6)
//                       MLK_SEND(7)
//                       MLK_DONE(8)
// =============================================================================
void VFlowMMDone(void)
{
  HandleVFlow(0);
  UpdateAndSendMlkState(MlkAmount, MlkState);
}

// =============================================================================
// VFlowMMMeas
// milk mode: MM_MEAS(1)
// handles milk states: MLK_NEUTRAL(2), 
//                      MLK_MEASURING(3),
//                      MLK_TAKE_OFF_WAIT(4), 
//                      MLK_TAKE_OFF(5) 
//                      MLK_TAKE_OFF_MAN(10)
// =============================================================================
void VFlowMMMeas(void)
{
  // Milk yield calculation:
  // save milk yield every second, to determine milk flow
  HandleVFlow(1);
  UpdateAndSendMlkState(MlkAmount, MlkState);
}

// =============================================================================
// VFlowMMContinue
// milk mode: MM_MEAS(1)
// handles milk states: MLK_CONTINUE(9) 
// =============================================================================
void VFlowMMContinue(void)
{
  VflowMlkAmountStart = VflowMlkAmountRestart;
}

// =============================================================================
// VFlowMMDefault
// milk mode: MM_MEAS(1)
// handles milk states: DEFAULT states 
// =============================================================================
void VFlowMMDefault(void)
{
  HandleVFlow(0);
}

// =============================================================================
// VFlowMMCleaning
// handles milk mode: MM_CLEANING(4) 
// =============================================================================
void VFlowMMCleaning(void)
{
  HandleVFlow(0);

  // check if cleaning succeeded (default after 2 hours)
  if ( Cnt1sec > (ul_CleanStartTime + (2*60*60))){
    if ( suCleanOk != VFLOW_CLEAN_OK ){
      // send message
    }
  }
}

/*
// =============================================================================
// MEMOLAC MEASURING
// =============================================================================
void VflowMeasuring(void)
{
  static unsigned char Init     = FALSE;
  static unsigned char MilkDump = FALSE;

  switch (MlkState) {
    case MLK_START_CLEANING:                            // STATE 15
    case MLK_CLEANING:                                  // STATE 16
    case MLK_CLEANING_END:                              // STATE 18
      if (Init == FALSE) {
        MlkMode = MM_INIT;
        if ( (EEI2Control & 0x07) == 0x01) CleanPhase = CLEAN_PHASE_START;
        else                               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      } else {
        MlkMode = MM_CLEANING;
      }
    case MLK_CLEANING_END_I2:                           // STATE 19
      break;
    case MLK_DHIA:                                      // STATE 103
      // Reset melkgift
      if ((KeyA == KEY_CLEAR && ScreenNumber == SCREEN_RESET) || MlkMode ==MM_CLEANING) {
        MlkMode = MM_INIT;
      } else {
        MlkMode = MM_DHIA;
      }
    break;
    case MLK_WAIT_FOR_START:                            // STATE 11
      if (MlkMode == MM_CLEANING) {
        MlkMode = MM_INIT;
        if ( (EEI2Control & 0x07) ==  0x01) CleanPhase = CLEAN_PHASE_START;
        else                                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
      }
    break;

    case MLK_SERVICE:                                   // STATE 101
      MlkMode = MM_SERVICE;
    break;
    case MLK_RESTART_CLEANING:
      break;
    default:
      if ( (EEI2Control & 0x07) == 0x01 ) CleanPhase = CLEAN_PHASE_START;
      else                                CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
    break;
  }

  switch (MlkMode) {
    case MM_INIT    :                                   // STATE 0
      // Init memolac milk measuring

      // ResetFloatPosition();
      VflowMlkAmountRestart = VflowMlkAmountStart;
      StoredMlkAmount = 45;
      MlkAmount = 45;
      MlkAmountKg = 0;
      MlkDuration = 0;
      MlkActFlow = 0;
      MlkMaxFlow = 0;

      // Set Interrupt registers
      suDataAvailable = 0;
      suTimePntr = 0;
      suBufPntr=0;
      suErrorCount = 0;
      suTestErr = 0;
      suSuccess = 0;
      suCleanOk = 0;
      suMsgType = 's';
      InitUart();

      if (Init != FALSE )   MlkMode = MM_MEAS;
      Init    = TRUE;
      MilkDump = TRUE;                // extra dump bij versturen van melk result
    break;


    case MM_MEAS:                                       // STATE 1
      switch(MlkState) {
        //Milking??
        case  MLK_IDLE:                                  // STATE 0
        case  MLK_SEND_WAIT:                             // STATE 6
        case  MLK_SEND:                                  // STATE 7
        case  MLK_DONE:                                  // STATE 8
              HandleVFlow(0);
              UpdateAndSendMlkState(MlkAmount, MlkState);
          break;
        case  MLK_NEUTRAL:                               // STATE 2
        case  MLK_MEASURING:                             // STATE 3
        case  MLK_TAKE_OFF_WAIT:                         // STATE 4
        case  MLK_TAKE_OFF:                              // STATE 5
        case  MLK_TAKE_OFF_MAN:                          // STATE 10
              // Milk yield calculation:
              // save milk yield every second, to determine milk flow
              HandleVFlow(1);

              UpdateAndSendMlkState(MlkAmount, MlkState);
          break;
        case  MLK_CONTINUE:                              // STATE 10
              VflowMlkAmountStart = VflowMlkAmountRestart;
          break;
        default:
              HandleVFlow(0);
        break;
      }

    break;

    case MM_DHIA:                                       // STATE 3
    case MM_SERVICE:                                    // STATE 5
    break;

    case MM_CLEANING:                                   // STATE 4
      HandleVFlow(0);

      // check if cleaning succeeded (default after 2 hours)
      if ( Cnt1sec > (ul_CleanStartTime + (2*60*60))){
        if ( suCleanOk != VFLOW_CLEAN_OK ){
          // send message
        }
      }

      switch (CleanPhase) {
        case CLEAN_PHASE_START:                         // STATE 0
          CleanPhase       = CLEAN_PHASE_0;
          StartCleanPhase1 = Cnt1sec;

          if ( (EEI2Control & 0x04) || (EEI2Control == 0x00) ) CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
        break;

        case CLEAN_PHASE_0:                             // STATE 3
          // Tien seconden wachten na opstarten (anders start reiniging direct omdat
          // klep tijdens initialiseren wordt bekrachtigd
          if (Cnt1sec > 10  && Cnt1sec > (StartCleanPhase1 + 1)) {

            CleanPhase       = CLEAN_PHASE_1;
            StartCleanPhase1 = Cnt1sec;
            ClnPhsStartTime  = Cnt1sec;
          }

            // controleer of de pulsatie AAN of UIT moet
          CheckPulsation();
        break;

        case CLEAN_PHASE_1:                             // STATE 4
          // Reinigingsfase 1 verstreken
          if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase1 + (60 * EEClnPhs1))) {
            CleanPhase       = CLEAN_PHASE_2;
            StartCleanPhase2 = Cnt1sec;
          } else {
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          }
        break;
        case CLEAN_PHASE_2:                             // STATE 5
          // Reinigingsfase 2 verstreken, zoja stop de pulsatie
          if ( TimeControl( (unsigned short)Cnt1sec, StartCleanPhase2 + (60 * EEClnPhs2))) {
            PlsState    = PLS_STOP_PULSATION;
            if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
            else                                      VacuumState = VACUUM_ON;
            CleanPhase  = CLEAN_PHASE_END;
          } else{
            // controleer of de pulsatie AAN of UIT moet
            CheckPulsation();
          }
        break;

        case CLEAN_PHASE_END:                           // STATE 7
          // finish extra dump
          if ( ((EEI2Control & 0x03) != 1) && EERowSize ){
            SetStartSwitchState();
            if ( StartSwitchState == SWITCH_CLOSED ){
              Cnt1sec          = 0;
              StartCleanPhase4 = Cnt1sec;
              if ( EEOutputK2 == VACUUM_CONTROL_INVERS) VacuumState = VACUUM_OFF;
              else                                      VacuumState = VACUUM_ON;
              CleanPhase       = CLEAN_PHASE_PRE_CLEAN;

              // set MPC's in pre-wash mode, centraal
              if ( EERowSize ){
                 ExternMessages |= SEND_WASH_MODE_4_START_MESSAGES;
              }
            }
          }

        break;

        case CLEAN_PHASE_PRE_CLEAN:                     // STATE 8
             // Reinigingsfase 2 verstreken
             if (TimeControl( (unsigned short)Cnt1sec, StartCleanPhase4 + (60 * EEClnPhs4))) {
               PlsState    = PLS_STOP_PULSATION;
               CleanPhase  = CLEAN_PHASE_END;
             } else{
               // controleer of de pulsatie AAN of UIT moet
               CheckPulsation();
             }
          break;
        case CLEAN_PHASE_I2_CONTROLLED_START:                // state 9
             SetStartSwitchState();
             //
             //   2: afhankelijk van de communicatie; de 'Master' moet I2 in de gaten houden
             //   1: onafhankelijk van I2
             //   0: afhankelijk van I2;
             //

            if (Cnt1sec > 10 ) {
              if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)) {
                // set MPC's in I2C wash mode, centraal
                if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
                  ExternMessages |= SEND_WASH_MODE_I2C_START_MESSAGES;
                }
                CleanPhase  = CLEAN_PHASE_I2_CONTROLLED_0;
                if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_ON;
              }
            }
          break;

        case CLEAN_PHASE_I2_CONTROLLED_0:                    // state 10
             // Tien seconden wachten na opstarten (anders start reiniging direct omdat
             // klep tijdens initialiseren wordt bekrachtigd
             if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
               // wassen afhankelijk van I2 of afhankelijk van Master
               SetStartSwitchState();

               if (Cnt1sec > 10 ) {
                 if ( ((StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD))){
                   CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
                 } else {
                   if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)) {
                     if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
                       ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
                       CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                       if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
                     } else {
                       if ( ((EEI2Control & 0x03) == 0x00)){
                         CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                         if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
                       }
                     }
                   }
                 }
               }
             } else {
               // slave; start
               CleanPhase = CLEAN_PHASE_I2_CONTROLLED_1;
             }
             // controleer of de pulsatie AAN of UIT moet
             CheckPulsation();
          break;

        case CLEAN_PHASE_I2_CONTROLLED_1:                  // state 11
             ClnPhsStartTime  = Cnt1sec;
             StartCleanPhase1 = Cnt1sec;
             CleanPhase = CLEAN_PHASE_I2_CONTROLLED;

             CheckPulsation();
          break;

        case CLEAN_PHASE_I2_CONTROLLED:                  // state 11
             // controleer of de pulsatie AAN of UIT moet
             CheckPulsation();
             SetStartSwitchState();

             if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){
               // MASTER
               if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
                 CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                 PlsState   = PLS_STOP_PULSATION;

                 // set MPC's in I2C wash mode, centraal
                 ExternMessages |= SEND_WASH_MODE_I2C_STOP_MESSAGES;
                 if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
               }
             } else {
               if ((EEI2Control & 0x03) == 0x00){
                  // afhankelijk van I2
                 if ( (StartSwitchState == SWITCH_OPEN) || (StartSwitchState == SWITCH_OPEN_HOLD)){
                   CleanPhase = CLEAN_PHASE_I2_CONTROLLED_END;
                   PlsState   = PLS_STOP_PULSATION;
                   if( EEOutputK2 == VACUUM_CONTROL_INVERS ) VacuumState = VACUUM_OFF;
                 }
               }
             }
          break;
        case CLEAN_PHASE_I2_CONTROLLED_END:                   // state 12
             // MASTER en afhankelijk van I2; vanuit CONTROLLED_START worden de slaves opgestart
             SetStartSwitchState();
             if ( (((EEI2Control & 0x03) == 0x02) && EERowSize) ){

               if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
                 CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
               }
             } else {
               if ((EEI2Control & 0x03) == 0x00){
                  // afhankelijk van I2
                 if ( (StartSwitchState == SWITCH_CLOSED) || (StartSwitchState == SWITCH_CLOSED_HOLD)){
                   CleanPhase = CLEAN_PHASE_I2_CONTROLLED_START;
                 }
               }
             }
          break;
      } // switch CleanPhase
      break;

    default :
      break;
  } // switch MlkMode
}
*/

