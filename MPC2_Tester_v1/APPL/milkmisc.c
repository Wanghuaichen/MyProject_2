// =============================================================================
// MODULE MILKMESS
// =============================================================================

#include "milkmisc.h"
#include "milk.h"
#include "conduct.h"
#include "temp.h"
#include "pulsation.h"
#include "../lib/stdlib.h"
#include "../lib/boolean.h"
#include "../lib/reg_flags.h"
#include "../lib/eehc11.h"
#include "../lib/extport.h"
#include "../lib/io6811.h"
#include "../lib/time.h"
#include "../lib/rti_int.h"
#include "../key/keyint.h"
#include "../key/key.h"
#include "../key/disp.h"
#include "../comm/flowctrl.h"


#pragma codeseg(EEPAGE)

extern unsigned char msgTimeoutTime;


unsigned char GateState, SideState, StartSwitchState = SWITCH_OPEN;
unsigned char GateStateNew, EmptyGlassAuto;

// =============================================================================
unsigned char ReadI1Sensor( void )
{
  if (      Sensor1Cntr > SENSOR1_CNTR_ON  ) return SENSOR_STATE_CLOSE;
  else if ( Sensor1Cntr < SENSOR1_CNTR_OFF ) return SENSOR_STATE_OPEN;
  else                                       return 0;
}


// =============================================================================
unsigned char ReadI2Sensor( void )
{
  if (      Sensor2Cntr > SENSOR2_CNTR_ON  ) return SENSOR_STATE_CLOSE;
  else if ( Sensor2Cntr < SENSOR2_CNTR_OFF ) return SENSOR_STATE_OPEN;
  else                                       return 0;
}

// =============================================================================
unsigned char ReadI3Sensor( void )
{
  if (      Sensor3Cntr > SENSOR3_CNTR_ON  ) return SENSOR_STATE_CLOSE;
  else if ( Sensor3Cntr < SENSOR3_CNTR_OFF ) return SENSOR_STATE_OPEN;
  else                                       return 0;
}


// =============================================================================
unsigned char ReadI4Sensor( void )
{
  if (      Sensor4Cntr > SENSOR4_CNTR_ON  ) return SENSOR_STATE_CLOSE;
  else if ( Sensor4Cntr < SENSOR4_CNTR_OFF ) return SENSOR_STATE_OPEN;
  else                                       return 0;
}

// =============================================================================
unsigned char ReadM1Sensor( void )
{
  if (      SensorM1Cntr > SENSOR1_CNTR_ON  ) return SENSOR_STATE_CLOSE;
  else if ( SensorM1Cntr < SENSOR1_CNTR_OFF ) return SENSOR_STATE_OPEN;
  else                                        return 0;
}

// =============================================================================
// Get Node Index
// =============================================================================
signed char GetNodeIndex(signed char Node)
{
  if (Node == EEAddressCopy) return(0);
  if (Node == EEAddress2)    return(1);
  return(-1);
}

// =============================================================================
// Get Node Address
// =============================================================================
signed char GetNodeAddress( signed char Side)
{
  if (Side == 0) return(EEAddressCopy);
  if (Side == 1) return(EEAddress2);
  return(-1);
}


// =============================================================================
// Get Node to Send
// =============================================================================
signed char GetSideIndex( signed char Side)
{
  if (Side == SIDE_A) return(0);
  if (Side == SIDE_B) return(1);
  return(-1);
}

// =============================================================================
// Get Other Node to Send
// =============================================================================
signed char GetOtherSideIndex(signed char Side)
{
  if (Side == SIDE_A) return(1);
  if (Side == SIDE_B) return(0);
  return(-1);
}

// =============================================================================
// Get active display side
// =============================================================================
signed char GetDisplaySide(void)
{
  if ( EEAddress2 && DisplayOtherSide ){
      // controleer of er terug geschakeld moet worden
      if ( TimeControl( Cnt10msec, OtherDisplayTime + 1000 )){
        DisplayOtherSide = FALSE;
        return SideState;
      } else {
      return SideState == SIDE_A ? SIDE_B : SIDE_A;
      }
  } else {
    // kan zowel SIDE_A of SIDE_B zijn
    return SideState;
  }
}

// =============================================================================
// Get cow attentions, before begin milking
// =============================================================================
unsigned short GetDisplayAtt(signed char side)
{
  NETIO_MAIL    *MailPtr;
  void          *CowDiseaseRefPtr;
  unsigned short Attention, r_val = 0;
  unsigned char  i;

  nr_of_disease_items = 0;


  if (MlkState == MLK_INIT || MeasState[side] == MEAS_IDLE || MlkState == MLK_WAIT_FOR_START) {
    if ( p_CowDiseaseMail[side] != NULL ){
      MailPtr = (NETIO_MAIL *)p_CowDiseaseMail[side];
      if (LONG_COW_GEN_NUMBER ){
        nr_of_disease_items = ((MailPtr->message_length-4)/sizeof((((ExtCowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
      } else {
        nr_of_disease_items = ((MailPtr->message_length-2)/sizeof((((CowDiseaseRef *)CowDiseaseRefPtr)->Array)[0]));
      }
    }

    // TOT NEUTRALE MELKFASE
    Attention = GetCowAtt(side);
    if (Attention & CA_COLOST)                                   r_val |= DPL_COLOST_ATT;
    if (Attention & CA_SEP)                                      r_val |= DPL_SEP_ATT;
    if (Attention & CA_COND)                                     r_val |= DPL_COND_ATT;
    if (Attention & CA_ACT_COND)                                 r_val |= DPL_ACT_COND_ATT;
    if (Attention & CA_TEMP)                                     r_val |= DPL_TMP_ATT;
    if ((p_MlkRef[side] != NULL) &&
        (LONG_COW_GEN_NUMBER ?
        ((ExtMlkRef **)p_MlkRef)[side]->CowGenDryOffDate :
        ((   MlkRef **)p_MlkRef)[side]->CowGenDryOffDate ))      r_val |= DPL_DRY_ATT;
    if ((p_MlkRef[side] != NULL) &&
        (LONG_COW_GEN_NUMBER ?
        (!(((ExtMlkRef **)p_MlkRef)[side]->CowGenCalvingDate)) :
        (!(((   MlkRef **)p_MlkRef)[side]->CowGenCalvingDate)))) r_val |= DPL_DRY_ATT;
    if ((p_MlkRef[side] != NULL) && nr_of_disease_items ){
      for( i = 0; i < nr_of_disease_items; i++){
        r_val |= (DPL_ILL_ATT1<<i);
      }
    }
  }
  r_val |= GetExtCondAtt(side);
  
  // check for communication error
  if (msgTimeoutTime== 0) r_val |= DPL_COMM_ATT;

  return r_val;
}


// =============================================================================
// check for errors in external conductivity module
// =============================================================================
unsigned short GetExtCondAtt( signed char side )
{
  unsigned short r_val = 0;

  if ( (EEExtConMeasuring & 0x03) ){
    // TOT NEUTRALE MELKFASE
    if (MlkState == MLK_INIT || MeasState[side] != MEAS_MEAS || MlkState == MLK_WAIT_FOR_START || MlkState == MLK_DONE) {
//      if      ( CMErrorInfo.CMErrors > 3 ) r_val |= CMErrorInfo.CMErrCode;
//      else if ( TMErrorInfo.TMErrors > 3 ) r_val |= TMErrorInfo.TMErrCode;
      if (CMErrorInfo.CMErrors > 3 ) r_val |= DPL_CM_ERR;
      if (TMErrorInfo.TMErrors > 3 ) r_val |= DPL_TM_ERR;
    }
  }
  return r_val;
}

// =============================================================================
// Get cow attentions
// =============================================================================
unsigned short GetCowAtt(unsigned char idx)
{
  unsigned short att = 0;

  if (p_MlkRef[idx] != NULL) {
    if ( LONG_COW_GEN_NUMBER ){
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )           att |= CA_SEP;
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost &&
          ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )           att |= CA_COLOST;
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent )       att |= CA_DRY;
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowGenCalvingDate == 0)    att |= CA_DRY_BLCK; // blokkeer onderhangen
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowGenDryOffDate)          att |= CA_DRY_BLCK;
      if (((ExtMlkRef **)p_MlkRef)[idx]->CowGenHeatAttent )         att |= CA_HEAT;
    } else {
      if (((MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )              att |= CA_SEP;
      if (((MlkRef **)p_MlkRef)[idx]->CowGenColost &&
          ((MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )              att |= CA_COLOST;
      if (((MlkRef **)p_MlkRef)[idx]->CowGenDryOffAttent )          att |= CA_DRY;
      if (((MlkRef **)p_MlkRef)[idx]->CowGenCalvingDate == 0)       att |= CA_DRY_BLCK; // blokkeer onderhangen
      if (((MlkRef **)p_MlkRef)[idx]->CowGenDryOffDate)             att |= CA_DRY_BLCK;
      if (((MlkRef **)p_MlkRef)[idx]->CowGenHeatAttent )            att |= CA_HEAT;
    }
  } else {
    if ( EEAddressCopy != 0)                                        att |= CA_NOCOW;
  }
  if ( p_CowConRef[idx] != NULL &&
       (LONG_COW_GEN_NUMBER ?
       ((ExtCowConRef **)p_CowConRef)[idx]->CowConDataAttentions :
       ((   CowConRef **)p_CowConRef)[idx]->CowConDataAttentions )) att |= CA_COND;
  if ( idx == GetDisplaySide() ){
    // alleen als juiste zijde gedisplayed wordt
    if ( ConDataAttentions & 0x0f )                                 att |= CA_ACT_COND;
    if ( CowTempAttention )                                         att |= CA_TEMP;
  }
  return(att);
}



// =============================================================================
// Get transfer attentions
// =============================================================================
unsigned char GetTransferAtt(void)
{
    signed char  idx = GetSideIndex(SideState);
  unsigned char  att = 0;

  if ( SoftKey & SK_STIM_TRCK_ARM_ENABLE ){
    if (p_MlkRef[idx] != NULL) {
      if ( MilkMeterType < MMT_NO_MILK_METER ){
        if (((MlkAmount+50) / 100) <
           (LONG_COW_GEN_NUMBER ?
           (((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10) :
           (((   MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10)))    att |= TA_MINYIELD;
      }
      if (LONG_COW_GEN_NUMBER ?
           ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps :
           ((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )             att |= TA_SEP;
      if ((LONG_COW_GEN_NUMBER ?
           ((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost :
           ((   MlkRef **)p_MlkRef)[idx]->CowGenColost ) &&
          (LONG_COW_GEN_NUMBER ?
           ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps :
           ((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps))             att |= TA_COLOST;
    } else {                                                           att |= TA_NOCOW;
      if ( ConDataAttentions & 0x0f )                                  att |= TA_ACT_COND;
    }
  }
  return(att);
}



// =============================================================================
// Get cow Glas attentions, at the end of the milking
// =============================================================================
unsigned short  GetCowGls(void)
{
    signed char  idx = GetSideIndex(SideState);
  unsigned char  att = 0;

  if (p_MlkRef[idx] != NULL) {
    if ( LONG_COW_GEN_NUMBER ?
        ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps :
        ((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps )     att |= CG_SEP;
    if ((LONG_COW_GEN_NUMBER ?
         ((ExtMlkRef **)p_MlkRef)[idx]->CowGenColost :
         ((   MlkRef **)p_MlkRef)[idx]->CowGenColost ) &&
        (LONG_COW_GEN_NUMBER ?
         ((ExtMlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps :
         ((   MlkRef **)p_MlkRef)[idx]->CowMlkNrOfSeps))    att |= CG_COLOST;
    if (MlkAmount == 0)                                     att |= CG_YIELD;
    if (((MlkAmount+50) / 100) <
         (LONG_COW_GEN_NUMBER ?
         (((ExtMlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10) :
         (((   MlkRef **)p_MlkRef)[idx]->CowMlkExpMinAmount/10))) att |= CG_MINYIELD;
    if (LONG_COW_GEN_NUMBER ?
        ((ExtMlkRef **)p_MlkRef)[idx]->HerdMlkEmptyGlassAuto :
        ((   MlkRef **)p_MlkRef)[idx]->HerdMlkEmptyGlassAuto == 0) {
      att |= CG_EMPTY;
      // Als tijdens een melksessie het melkglas niet geledigd mag worden geldt dit
      // ook voor alle opvolgende melksessie van het desbetreffende melkmaal
      EmptyGlassAuto = FALSE;
    }
  } else {
    if (EEGoatMilkGlass == FALSE) att |= CG_NOCOW;
  }
  return(att);
}

// =============================================================================
// Get Dhis attentions
// =============================================================================
unsigned char GetDhiaAtt(unsigned char Reset)
{
  static unsigned short PrevMilkYield, StartTime;
         unsigned char  byte;
         unsigned short MilkTest;

  if (Reset == TRUE)  {
    StartTime = Cnt1sec;
  }
  byte = FALSE;

  switch (MilkMeterType) {
    case MMT_NIVEAU:
      MilkTest = 90;
      if (EEGoatMilkGlass == TRUE) {
        MilkTest = 40;
      }
      // Test of melkgift constant is
      if (MlkAmountKg >= (PrevMilkYield - 1) && MlkAmountKg <= (PrevMilkYield+1)
          && MlkAmountKg > MilkTest) {
        if (Cnt1sec > (StartTime + 60)) {
          byte = TRUE;
        }
      } else {
        StartTime = Cnt1sec;
      }
      PrevMilkYield = MlkAmountKg;
    break;
  }
  return(byte);
}


// =============================================================================
// Initieer de hekschakelaar + melkplaats side ( I1 )
// =============================================================================
void InitGateState(void)
{
  SideState = SIDE_A;

  if (PORTE & P_3) {
    // Schakelaar open
    GateState = GATE_CLOSED;
    GateStateNew = GATE_CLOSED;
    Sensor1Cntr = 0;
  } else {
    // Schakelaar dicht
    GateState = GATE_OPEN;
    GateStateNew = GATE_OPEN;
    Sensor1Cntr = SENSOR1_CNTR_MAX;;
    if (EEAddress2 != 0) {
      SideState = SIDE_B;
    }
  }
}


// =============================================================================
// Bepaal toestand van de hekschakelaar
// =============================================================================
void SetGateState(void)
{
  static unsigned long GateTime;
         unsigned char sensor_state;

  // bepaal toestand schakelaar
  sensor_state = ReadI1Sensor();

  // Hek Dicht ?
  if ( sensor_state == SENSOR_STATE_OPEN ){
    if (GateStateNew == GATE_OPEN) {
      GateTime = Cnt10msec+EEGateSwitchTime*10;         // time in 100msec
      GateStateNew = GATE_CLOSED;
    }
    if (TimeControl(Cnt10msec, GateTime)){
      GateState = GATE_CLOSED;
    }
  } else {
    // Hek Open ?
    if ( sensor_state == SENSOR_STATE_CLOSE ){
      if (GateStateNew == GATE_CLOSED) {
        GateTime = Cnt10msec+(EEGateSwitchTime*10);
        GateStateNew = GATE_OPEN;
      }
      if (TimeControl(Cnt10msec, GateTime)){
        GateState = GATE_OPEN;
      }
    }
  }
}

// =============================================================================
// Bepaal toestand MelkStart Schakelaar, (niet via key-board)
// =============================================================================
void SetStartSwitchState(void)
{
  static unsigned char  StartSwitchStateNew = SWITCH_OPEN;
  static unsigned short SwitchTime;
         unsigned char  sensor_state;

  // bepaal toestand schakelaar
  if ( (EEI1Control == 2) && (KeyMode == KM_MILKING)) sensor_state = ReadI1Sensor();
  else                                                sensor_state = ReadI2Sensor();

  // Schakelaar open ?
  if ( sensor_state == SENSOR_STATE_OPEN ){
    if (StartSwitchStateNew == SWITCH_CLOSED) {
      SwitchTime = Cnt10msec;
      StartSwitchStateNew = SWITCH_OPEN;
      StartSwitchState = SWITCH_OPEN;
    }

    // key-hold, 900 msec.
    if ( TimeControl( Cnt10msec, SwitchTime+400)){
      StartSwitchState = SWITCH_OPEN_HOLD;
    }
  } else {
    // Schakelaar gesloten ?
    if ( sensor_state == SENSOR_STATE_CLOSE ){
      if (StartSwitchStateNew == SWITCH_OPEN) {
        SwitchTime = Cnt10msec;
        StartSwitchStateNew = SWITCH_CLOSED;
        StartSwitchState = SWITCH_CLOSED;
      }

      if ( TimeControl( Cnt10msec, SwitchTime+400)){
        StartSwitchState = SWITCH_CLOSED_HOLD;
      }
    }
  }
}


// =============================================================================
// Controleer of koe aanwezig is
// =============================================================================
unsigned char MlkCowAvailable(unsigned char idx)
{
  if (p_MlkRef[idx] != NULL) return(1);
  else                       return(0);
}


// =============================================================================
unsigned long GetCowNumber(unsigned char idx)
{
  if (p_MlkRef[idx] != NULL)
    return(LONG_COW_GEN_NUMBER ?
      ((ExtMlkRef **)p_MlkRef)[idx]->CowGenNumber :
      ((MlkRef **)p_MlkRef)[idx]->CowGenNumber);
  else  return(0);
}


// =============================================================================
// Controleer of er een nieuwe koe is
// =============================================================================
unsigned char DifferentCowAvailable( unsigned char idx )
{
  unsigned long cow;

  cow = GetCowNumber(idx);

  if ( (cow != CowMilked[idx] ) || (cow == 0) ) return 1;  // versie 101: cow == 0 ipv cow != 0
  else                                          return 0;
}
