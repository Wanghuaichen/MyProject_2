#include "../flowlib/cond_temp_vcp_flow.h"
#include "../comm/flowctrl.h"
#include "../lib/rti_int.h"
#include "../lib/boolean.h"
#include "../lib/eehc11.h"
#include "../lib/typedef.h"
#include "../lib/extport.h"
#include "../lib/eeprom.h"
#include "../lib/time.h"
#include "../lib/mem.h"
#include "../lib/iic.h"
#include "../key/key.h"
#include "../key/key_input.h"
#include "../key/keyint.h"
#include "../key/disp.h"
#include "../usx/fake_usx.h"
#include "milk.h"
#include "take_off_sensor.h"
#include "takeoff.h"
#include "conduct.h"

extern unsigned char VflowConductivity;

#pragma codeseg(EEPAGE)



#define NR_OF_TABLE_ENTRIES 10
#define COND_MIN_VALUE      30

// struct: address, length and message  ----------------------------------------
const unsigned char EXT_COND_1_REQ[4] = {0x1f, 0x00, 0x80};
const unsigned char EXT_COND_2_REQ[4] = {0x1f, 0x00, 0x81};


const unsigned char ConvTable[10][2] = {
  {195, 60},
  {130, 51},
  {120, 42},
  {110, 39},
  {100, 32},
  { 90, 27},
  { 80, 23},
  { 70, 17},
  { 60, 12},
  { 40,  8}
};


CONDUCTIVITY_INFO ConductivityBuffer[144];
HVC_BUFFER_INFO   HVC_Buffer[4];
CM_ERROR_INFO     CMErrorInfo;
unsigned char     ConDataAttentions;
unsigned short    CBWptr;
TAKE_OFF_SENSOR   TakeOffSensor;


// =============================================================================
void InitConductivity(void)
{
  // Clear geleidbaarheids tabellen
  memory_clear( (unsigned char *)ConductivityBuffer, sizeof(CONDUCTIVITY_INFO) * 144 );
  memory_clear( (unsigned char *)HVC_Buffer, sizeof(HVC_BUFFER_INFO) * 4);
  CMErrorInfo.CMErrCode = 0;
  CMErrorInfo.CMErrors  = 0;
  ConDataAttentions     = 0;
  CBWptr                = 0;

  TakeOffSensor.h_cntr = 0;
  TakeOffSensor.l_cntr = 0;

  if ( MilkMeterType == MMT_VFLOW ){
    /*
    // EEExtConMeasuring:
    // 0x01: external claw
    // 0x02: switch values 180 degrees
    // 0x04: one channel only
    */

    if (EEConMeasuring){
      WriteEEpromByte(4, &EEExtConMeasuring);
    } else {
      WriteEEpromByte(0, &EEExtConMeasuring);
    }
  }
}

// =============================================================================
void ConConMpkMpc( unsigned char *cv )
{
  unsigned char i, j;

  // conversie tbv MPK -> MPC2
  for ( j = 0; j < 4; j++ ){
    for( i = 0; i < (NR_OF_TABLE_ENTRIES-1); i++ ){
      if ( ( *(cv+j) <= ConvTable[i][0]) && (*(cv+j) > ConvTable[i+1][0]) ){
        *(cv+j) += ConvTable[i][1];
        continue;
      }
    }
  }
}


// =============================================================================
void CheckForAttention( unsigned char side )
{
  unsigned char    la_cond = 255;         // lowest average ( actual)
  unsigned char    ra_cond = 255;         // rolling average
  unsigned char    pa_cond = 255;         // previous average
  unsigned char    pa, lra, la, i;
  unsigned char    teat_3_test = 0xff;
    signed short   perc;
  HVC_BUFFER_INFO *p    = HVC_Buffer;
  unsigned char   *p_pa  = LONG_COW_GEN_NUMBER ?
                   (unsigned char *)&(((ExtCowConRef **)p_CowConRef)[side]->CowConDataValue1) :
                   (unsigned char *)&(((   CowConRef **)p_CowConRef)[side]->CowConDataValue1);
  unsigned char   *p_pra = LONG_COW_GEN_NUMBER ?
                   (unsigned char *)&(((ExtCowConRef **)p_CowConRef)[side]->CowConRollingAverage1) :
                   (unsigned char *)&(((   CowConRef **)p_CowConRef)[side]->CowConRollingAverage1);


  if ( p_CowConRef[side] != NULL ){
    if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
      // MENGMELK; controleer rolling average
      if ( ((p[0].r_average) < p_HerdConRef->HerdConMinValue ) ||
           ((p[0].r_average) > p_HerdConRef->HerdConMaxValue ) ) return;

      // MENGMELK; controleer average
      if ( ((p[0].average) < p_HerdConRef->HerdConMinValue ) ||
           ((p[0].average) > p_HerdConRef->HerdConMaxValue ) ) return;

      // MENGMELK; controleer previous average
      if ( (*p_pa < p_HerdConRef->HerdConMinValue ) ||
           (*p_pa > p_HerdConRef->HerdConMaxValue ) ) return;

      if ( p[0].average < *p_pra ) return;

      perc = ((((p[0].average) - (*p_pra))*100)/ (*p_pra));
      if ( perc >= p_HerdConRef->HerdConThresholdValue1 ){
        if ( !ConDataAttentions ){
          // nog geen attentie, stuur eventueel automatische separatie flow
          if ( EESepMsgCntrl & SEND_SEP_MSG_COND ){
            // deze functie handelt verder alles af
            HandleKeySave(EESepCondFuncNr ,SM_COW_INFO, SCREEN_COW_INFO_1, 1, side, 1);
          }
        }
        ConDataAttentions |= 0x01;
      }
    } else {
      // Check for lowest rolling_average conductivity
      for ( i = 0; i < 4; i++ ){
        if ( (p[i].r_average) < ra_cond ) {
          if ( (p[i].r_average) < p_HerdConRef->HerdConMinValue ){
            if ( teat_3_test == 0xff ){
              // could be a cow with three teats
              teat_3_test = i;
              continue;
            } else return;
          } else {
            ra_cond = (p[i].r_average); lra = i;    // lowest rolling average
          }
        }
      }
      // check maximum
      if ( ra_cond > p_HerdConRef->HerdConMaxValue )return;

      // Check for lowest average conductivity
      for ( i = 0; i < 4; i++ ){
        if ( (p[i].average) < la_cond ) {
          if ( (p[i].average) < p_HerdConRef->HerdConMinValue ){
            // could be a cow with three teats, must be the same teat missing
            if ( teat_3_test == i ) continue;
            else return;
          } else {
            la_cond = (p[i].average); la = i;       // lowest average
          }
        }
      }
      // check maximum
      if ( la_cond > p_HerdConRef->HerdConMaxValue )return;

      // check average previous milking
      for ( i = 0; i < 4; i++ ){
        if ( (p_pa[i]) < pa_cond ) {
          if ( (p_pa[i]) < p_HerdConRef->HerdConMinValue ){
            // could be a cow with three teats, must be the same teat missing
            if ( teat_3_test == i ) continue;
            else                    return;
          } else {
            pa_cond = (p_pa[i]); pa = i;       // lowest previous average
          }
        }
      }
      // check maximum
      if ( pa_cond > p_HerdConRef->HerdConMaxValue )return;

      // check for every nipple the following
      for ( i = 0; i < 4; i++ ){
        // do not check lowest value itself, or missing teat
        if ( (i == lra) || (i == teat_3_test)) continue;

        // check if percentage rolling-average exceeds ThresholdValue1
        perc = (((p[i].r_average) - (p[lra].r_average))*100)/ (p[lra].r_average);
        if ( perc < p_HerdConRef->HerdConThresholdValue1 )continue;

        // check if average exceeds minimum attention-value
        if ( (p[i].average) <= p_HerdConRef->HerdConAttValue )continue;
        perc = (((p[i].average) - (p[la].average) )*100)/ (p[la].average);
        if ( perc < p_HerdConRef->HerdConThresholdValue2 )continue;

        // check if average previous milking exceeds attention-value
        if ( p_pa[i] <= p_HerdConRef->HerdConAttValue )continue;

        // calculate differance between average and lowest previous milking
        perc = (( (p_pa[i]) - (p_pa[pa]) )*100)/ (p_pa[pa]);
        if ( perc < p_HerdConRef->HerdConThresholdValue2 )continue;

        if ( !ConDataAttentions ){
          // nog geen attentie, stuur eventueel automatische separatie flow
          if ( EESepMsgCntrl & SEND_SEP_MSG_COND ){
            // deze functie handelt verder alles af
            HandleKeySave(EESepCondFuncNr ,SM_COW_INFO, SCREEN_COW_INFO_1, 1, side, 1);
          }
        }
        ConDataAttentions |= (0x01 << i );
      }
    }
  }
}


// =============================================================================
unsigned char CheckAndStoreHVCBuffer( CONDUCTIVITY_INFO *cv, unsigned char side )
{
  unsigned char    i, j, x, NrOfValues;
  unsigned char   *p_cv = (unsigned char *)cv;
  HVC_BUFFER_INFO *p    = HVC_Buffer;
  unsigned char   *p_ra = LONG_COW_GEN_NUMBER ?
                   (unsigned char *)&(((ExtCowConRef **)p_CowConRef)[side]->CowConRollingAverage1) :
                   (unsigned char *)&(((   CowConRef **)p_CowConRef)[side]->CowConRollingAverage1);
  unsigned short   s;
  unsigned char    r_val = 0;

  if (p_HerdConRef != NULL ) {
    if ((p_HerdConRef->HerdConNrOfValues < 40) && (p_HerdConRef->HerdConNrOfValues != 0) ) {
      NrOfValues =  p_HerdConRef->HerdConNrOfValues;
    } else {
      NrOfValues = 20;
    }
  }

  for( j = 0; j < ((EEExtConMeasuring & CM_ONE_CHANNEL/*4*/) ? 1 : 4); j++ ){
    // check if the new value shoud be saved, higher than the minimum value
    // tabel blijft gesorteerd van hoog naar laag
    if ( *(p_cv+j) > p[j].HVC_Values[39] ){
      // store and sort buffer
      for ( i = 0; i < NrOfValues; i++ ){
        if ( *(p_cv+j) > p[j].HVC_Values[i] ){
          memory_move( (unsigned char *)&(p[j].HVC_Values[i+1]),
                       (unsigned char *)&(p[j].HVC_Values[i]), 39 - i );
          (p[j].HVC_Values[i]) = *(p_cv+j);
          break;
        }
      }
    }

    // start calculating averages when mimimum number of values collected
    // zero's are not handled
    for ( x= 0, s = 0, i = 0; i < CBWptr && i < NrOfValues; i++ ){
      if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
        if( (p[j].HVC_Values[i]) > COND_MIN_VALUE ) { s += (p[j].HVC_Values[i]); x++; }
      } else {
        if( (p[j].HVC_Values[i])) { s += (p[j].HVC_Values[i]); x++; }
      }
    }

    if (x) (p[j].average) = (unsigned char)(s/x );
    else    p[j].average  = 0;

    // berekening rolling-average begint pas naar een minimum aantal metingen
    if (  x >= NrOfValues && p_CowConRef[side] != NULL ){
      // controleer of gemiddelde wel binnen de grenzen ligt
      if ( (p[j].average) >= p_HerdConRef->HerdConMinValue &&
           (p[j].average) <= p_HerdConRef->HerdConMaxValue ){

        // bereken nieuw RollingAverige, indien deze 0 is zal gelijk
        // het gemiddelde worden overgenomen, zij het binnen grenzen
        if ( *(p_ra+j) == 0 ) s = (p[j].average);
        else {
          if ( EEExtConMeasuring & CM_ONE_CHANNEL ){
            // afronding op een half
            s = (10 * (( *(p_ra+j) * 4 ) + (p[j].average)))/5;
            if ( (s % 10) >= 5 ) s = (s/10)+1;
            else                 s = (s/10);
          } else  {
            s = (( *(p_ra+j) * 2 ) + (p[j].average) )/3;
          }
        }

        if ( s < p_HerdConRef->HerdConMinValue ) s = p_HerdConRef->HerdConMinValue;
        if ( s > p_HerdConRef->HerdConMaxValue ) s = p_HerdConRef->HerdConMaxValue;

        (p[j].r_average) = (unsigned char)s;
        r_val = 1;
      }
    }
  }
  return r_val;
}

// =============================================================================
// r_val   0: data goed ontvangen
//    1: fout in IIC protocol, rest van de fouten is van externe geleidbaarheids print
//    2: restart/start-up code
//    3: test-mode, check of calibratie nodig is
//    4: calibratie mode
//    5: schrijven eeprom na calibratie
//    6: interface ontvangt geen data van de klauw
//    7: interface ontvangt data, maar checksum is fout
// =============================================================================
unsigned char GetConductivityValues( unsigned char *buf)
{
  unsigned char   stat = 0;
  unsigned char   rbuf[20];
  unsigned short *p;

  memory_clear( (unsigned char *)buf, 5);

  // buf[0] -- buf[4]: property, RF, RR, LR, LF
  // haal de geidbaarheids waarde op, intern of extern
  if ( EEExtConMeasuring & 0x03){
    // lees geleidbaarheidsdata rechtstreeks van SAC klauw (externe bus)
    if ( !(stat = PwmMessage( 3, (unsigned char *)&EXT_COND_1_REQ, 8, (unsigned char *)&rbuf ))){
      if ( !(stat = PwmMessage( 3, (unsigned char *)&EXT_COND_2_REQ, 8, (unsigned char *)&(rbuf[10]) ))){
        if ( rbuf[3] == 0x80 && rbuf[13] == 0x81 ){
          if ( rbuf[2] == 4 && rbuf[12] == 4){
            // data goed gelezen, converteer naar milli-siemens (* 25)
            buf[0] = IIC_READ_COND; stat = 0;
            p = (unsigned short *)&(rbuf[4]);

            buf[1] = (*(p+0)) < 100 ? 255 : (25000 / (*(p+0)));
            buf[2] = (*(p+1)) < 100 ? 255 : (25000 / (*(p+1)));

            // LF and LR are switched
            p = (unsigned short *)&(rbuf[14]);
            buf[4] = (*(p+0)) < 100 ? 255 : (25000 / (*(p+0)));
            buf[3] = (*(p+1)) < 100 ? 255 : (25000 / (*(p+1)));


            if ( (EEExtConMeasuring & 0x03) == 2 ){
              // waarden draaien over 180 graden
              buf[5] = buf[1];         // een waarde even opslaan
              buf[1] = buf[3];
              buf[3] = buf[5];
              buf[5] = buf[2];         // een waarde even opslaan
              buf[2] = buf[4];
              buf[4] = buf[5];
            }
          } else {
              stat = 1;                               // algemene error
            if ( rbuf[ 2] == 1 )stat = rbuf[ 4]+2;  // error code ontvangen
            if ( rbuf[12] == 1 )stat = rbuf[14]+2;  // error code ontvangen
          }
        } else {
          stat = 1;                                 // algemene error
          if ( rbuf[3] == 0x80 && rbuf[13] == 0x80 ){
            if ( rbuf[ 2] == 1 )stat = rbuf[ 4]+2;  // error code ontvangen
            if ( rbuf[12] == 1 )stat = rbuf[14]+2;  // error code ontvangen
          }
        }
      }
    }
  } else {
    if ( MilkMeterType == MMT_VFLOW ){
      buf[0] = IIC_READ_COND;
      buf[1] = buf[2] = buf[3] = buf[4] = VflowConductivity;
    } else {
      if ( !(stat = HandleReadIic( PICA_ADDRESS, IIC_READ_COND, 5, (unsigned char *)buf ))){
        // controleer of het juiste item is binnen gekomen
        if (buf[0] != IIC_READ_COND ) stat = 1;
      }
    }
  }
  return stat;
}


// =============================================================================
void HndlConductMeasuring( unsigned char side )
{
  static unsigned short  HCM_time;
  static unsigned char   cm_meas;
         unsigned char   cv[5];
         unsigned short  i,j;
         unsigned char   stat = 0;
         unsigned char   *tp = (unsigned char *)(ConductivityBuffer);

  if (((EEConMeasuring & CM_MEASURE_ENABLE) && ( SoftKey & SK_COND_TMP_ENABLE)) ||
       (MilkMeterType == MMT_TAKE_OFF_SENSOR) ) {
    if ( (MlkState >= MLK_NEUTRAL && MlkState <= MLK_TAKE_OFF_WAIT ) || (MlkState == MLK_TAKE_OFF_MAN ) ){
      if (MilkMeterType == MMT_TAKE_OFF_SENSOR){
        if ( TimeControl( Cnt10msec,  HCM_time + 5)){
          HCM_time = Cnt10msec;
          if ( !(stat = GetConductivityValues(cv) )){
          // takeof sensor
            CBWptr++;
            if ( (CBWptr > (10*EEMlkFlwCalcTime)) || (CBWptr > 600))CBWptr = 0;

            if ( cv[CV_RA] > EEDetectLevel ){
              *(tp+CBWptr) = 1;
              take_off_sensor_level = 1;
            } else {
              *(tp+CBWptr) = 0;
              take_off_sensor_level = 0;
            }
            *(tp+601) = cv[CV_RA];                   // fixed; to display

            // determine ratio no_milk/milk
            for ( j = 0, i = 0; ((i < (10*EEMlkFlwCalcTime)) && (i < 600)); i++ ){
              if ( *(tp+i) == 1 )j++;
            }

            MlkActFlow = take_off_sensor_ratio = (j *100)/ (EEMlkFlwCalcTime*10);
            // ratio smaller takeof treshold?
            if ( take_off_sensor_ratio < EETakeOffThreshold) FlowState = 0;
            else                                             FlowState = 1;
          }
        }
      } else {
        //  read every 5 seconds the conductivity
        if (((Cnt1sec % 5) == 0)  && (HCM_time != Cnt1sec) ){
          cm_meas = TRUE;
          if ( !(stat = GetConductivityValues(cv) )){
            // correct the values read with the sensor-correction-values
            cv[1] = (unsigned char)(((unsigned short)cv[1] * ConCor1)/100);
            cv[2] = (unsigned char)(((unsigned short)cv[2] * ConCor2)/100);
            cv[3] = (unsigned char)(((unsigned short)cv[3] * ConCor3)/100);
            cv[4] = (unsigned char)(((unsigned short)cv[4] * ConCor4)/100);

            // store values
            memory_cpy( (unsigned char *)&(ConductivityBuffer[CBWptr]), (unsigned char *)&(cv[1]), 4 );
            if ( CBWptr < 143 ) CBWptr++;              // 144 is de laatste entry

            // calculate average of the X highest values, maximal 40
            if ( CheckAndStoreHVCBuffer( (CONDUCTIVITY_INFO *)&(cv[1]), side) ){
              // look if an attention has to be given
              CheckForAttention(side);
            }
            // set tijd van laatste meting
            HCM_time = Cnt1sec;
            cm_meas = FALSE;
          }
        } else {
          if ( cm_meas ){
            cm_meas = FALSE;
            // meting niet gelukt, voeg nullen toe aan de tabel door de pointer
            // te verhogen (waardes zijn al nul)
            if ( CBWptr < 143 ) CBWptr++;              // 144 is de laatste entry
          }
          return;
        }
      }
    } else {
      // er wordt elke 2 seconden contact gezocht met PIC of externe module om
      // bij het begin van het melken eventuele foutmeldingen te hebben
      if (MilkMeterType == MMT_TAKE_OFF_SENSOR){
        if ( TimeControl( Cnt10msec,  HCM_time + 200)){
          HCM_time = Cnt10msec;
          stat = GetConductivityValues(cv);
        } else {
          return;
        }
      } else {
        if ((Cnt1sec % 2) == 0  && HCM_time != Cnt1sec ){
          HCM_time = Cnt1sec;
          stat = GetConductivityValues(cv);
        } else {
          return;
        }
      }
    }

    // fout in communicatie of in externe aansluiting
    // bewaar laatste fout, ze worden gereset bij elke nieuwe koe
    if ( stat == 1 || stat == 2 || stat == 6 || stat == 7 ){
      CMErrorInfo.CMErrCode = stat;
      CMErrorInfo.CMErrors++;
      if ( CMErrorInfo.CMErrors > 30 ) CMErrorInfo.CMErrors = 30;
    } else {
      if ( stat == 0 ){
       CMErrorInfo.CMErrCode = 0;
        CMErrorInfo.CMErrors  = 0;
      }
    }
  }
}

