#include "../flowlib/cond_temp_vcp_flow.h"
#include "../comm/flowctrl.h"
#include "../lib/rti_int.h"
#include "../lib/boolean.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../lib/stdlib.h"
#include "../lib/extport.h"
#include "../appl/milk.h"
#include "../key/key.h"
#include "../usx/fake_usx.h"
#include "milkmisc.h"
#include "temp.h"

extern signed short VflowTemperature;

#pragma codeseg(EEPAGE)

// struct: address, length and message  ----------------------------------------
const unsigned char EXT_TEMP_REQ[4] = {0x1f, 0x00, 0x82};


// meetwaarden NTC M891/10k  ---------------------------------------------------
#define R35         6944
#define R38_5       6165
#define R40         5831

#define U35           89
#define U40          156

#define TEMP_385     385
#define TEMP_150     150

// Waarden 'oude' sensor, worden gebruikt door SAC -----------------------------
#define R35_T4901   6569              // (4901)
#define R38_5_T4901 5746              //   volgens karakteristiek
#define R40_T4901   5372              // (b4901)

#define U35_T4901    112
#define U40_T4901    192

unsigned short  CowTemperature;
unsigned short  MaxCowTemperature;
unsigned short  ActTemperature[10];
unsigned char   TempCorr;
unsigned char   Tptr;
unsigned char   ActUadc;
unsigned char   CowTempAttention;
unsigned char   EE_U1_Copy;
unsigned char   EE_U2_Copy;
TM_ERROR_INFO   TMErrorInfo;


// =============================================================================
void InitTemperature(void)
{
  // Initieer temperature
  CowTemperature    = 0;
  MaxCowTemperature = 0;
  CowTempAttention  = 0;
  TMErrorInfo.TMErrCode = 0;
  TMErrorInfo.TMErrors  = 0;

  memory_clear( (unsigned char *)&(ActTemperature[0]), 20 );
  Tptr = 0;

  // gebruik eventueel de default waardes in de berekening
  // overschrijf de eeprom-waarden niet ze zijn anders niet terug te halen
  if ( EETempRValue > 5800 ){
    if ( (EE_U1 > (U35+7)) || (EE_U1 < (U35-7)) || (EE_U2 > (U40+9)) || (EE_U2 < (U40-9)) ){
      // waarden vallen buiten de grenzen
      EE_U1_Copy = U35;
      EE_U2_Copy = U40;
    } else {
      EE_U1_Copy = EE_U1;
      EE_U2_Copy = EE_U2;
    }
  } else {
    // andere sensor, neem de default waardes
    EE_U1_Copy = U35_T4901;
    EE_U2_Copy = U40_T4901;
  }
}

// =============================================================================
short AverageTemp(void)
{
  unsigned char i;
  short         rval;
  long          t = 0;

  for ( i = 0; i < 10; i++ ){
    t = t + (long)ActTemperature[i];
  }
  return (rval = (short)(t/10));
}

// =============================================================================
short CalcTemperature( unsigned char Um )
{
  signed short a, b, c;
         long  x, y;

  // bereken Rm = ((R1-R2)(Um-U1)+R1(U1-U2))/(U1-U2)
  if ( EETempRValue > 5800 ) {
    a = R35-R40;
    b = Um-EE_U1_Copy;
    c = EE_U1_Copy - EE_U2_Copy;
    y = (long)R35*c;
    x = ((((long)a)*((long)b))+y)/c;

    // corrigeer met eventuele offset
    x = x + ( (long)EETempRValue - (long)R38_5 );
    a = (short)(((14735-x )*50)/(R35-R40));
  } else {
    a = R35_T4901-R40_T4901;
    b = Um-EE_U1_Copy;
    c = EE_U1_Copy-EE_U2_Copy;
    y = (long)R35_T4901*c;
    x = ((((long)a)*((long)b))+y)/c;

    // corrigeer met eventuele offset
    x = x + ( (long)EETempRValue - (long)R38_5_T4901);
    a = (short)(((14948-x )*50)/(R35_T4901-R40_T4901));
  }
  return a;
}


// =============================================================================
// r_val  0: data goed ontvangen
//        1: fout in IIC protocol, rest van de fouten is van externe geleidbaarheids print
//        2: restart/start-up code
//        3: test-mode, check of calibratie nodig is
//        4: calibratie mode
//        5: schrijven eeprom na calibratie
//        6: interface ontvangt geen data van de klauw
//        7: interface ontvangt data, maar checksum is fout
// =============================================================================
unsigned char GetTemperature( unsigned char command, short *temperature, short *claw_temperature )
{
  unsigned char   rbuf[9], stat = 0;
  unsigned short *p;
           short  Tc, Tm, Td;

  if ( EEExtConMeasuring & 0x03 ){
    // lees geleidbaarheidsdata rechtstreeks van SAC klauw (externe bus)
    if ( !(stat = PwmMessage( 3, (unsigned char *)&EXT_TEMP_REQ, 8, (unsigned char *)&rbuf ))){
      if ( rbuf[3] == 0x82 ){
        if ( rbuf[2] == 4 ){
          p = (unsigned short *)&(rbuf[4]);
          Tm = *p; Tc = *(p+1);
          if ( command == TMP_MEASURE_MILK ){
            *claw_temperature = 0;

            // scale milk and claw temaperature
            Tm = Tm < TEMP_385 ? (TEMP_385-((TEMP_385-Tm)/2)) : Tm;
            Tc = Tc < TEMP_385 ? (TEMP_385-((TEMP_385-Tc)/2)) : Tc;

            if ( Tm > Tc ){
              // melktemperatuur hoger dan die van de klauw
              if ( (Td = Tm - Tc) > TEMP_150 ) Td = TEMP_150;
              *temperature = Tm + (Td * (15 -((Td*10)/100))/100);
            } else {
              // klauwtemperatuur hoger dan die van de melk
              if ( (Td = Tc - Tm) > TEMP_150 ) Td = TEMP_150;
              *temperature = Tm - (Td * (15 -((Td*10)/100))/100);
            }
          } else {
            *temperature = Tm;
            *claw_temperature = Tc;
          }
        } else {
          if ( rbuf[2] == 1 ) stat = rbuf[4]+2;  // error code ontvangen
          else                stat = 1;          // algemene error
        }
      } else stat = 1;                           // verkeerde bericht
    }
  } else {
    // interne logica gebruikt
    if ((MilkMeterType == MMT_VFLOW) || (MilkMeterType == MMT_VP4001)) {
      if ( VflowTemperature > 0){
        Tc = ((unsigned short)VflowTemperature*TempCorr)/100;
        *temperature = Tc;
      } else {
        Tc = ((unsigned short)VflowTemperature*TempCorr)/100;
        *temperature = 0-Tc;
      }
      stat = 0;
    } else {

      if ( !(stat = (HandleReadIic( PICA_ADDRESS, IIC_READ_TEMP, 2, rbuf))) ){
        if ( rbuf[0] == IIC_READ_TEMP ){
          ActUadc = rbuf[1];
          *temperature = CalcTemperature( rbuf[1] );
        } else {
          stat = 1;
        }
      }
    }
  }
  return stat;
}


// =============================================================================
void HndlTempMeasuring( void )
{
  static unsigned short  TM_time;
                  short  dummy, T;
         unsigned char   stat, tempatt;

  if ( EETempMeasuring && ( SoftKey & SK_COND_TMP_ENABLE )) {
    if ( (MlkState >= MLK_NEUTRAL && MlkState <= MLK_TAKE_OFF_WAIT ) || (MlkState == MLK_TAKE_OFF_MAN) ){

      if (((Cnt1sec % 1) == 0  && TM_time != Cnt1sec)  ){
        if ( !(stat = GetTemperature( TMP_MEASURE_MILK, &T, &dummy)) ){
          // Gemeten waarde tussen 35 en 45 graden celsius
          if ( (T > 0) && (T <= 450)) {
            ActTemperature[Tptr++] = T;
            if ( Tptr > 9 ){
              Tptr = 0;
              if ( !CowTemperature )CowTemperature = AverageTemp();
            }

            // wacht met bepalen totdat eerste 10 metingen zijn gedaan
            if ( CowTemperature )CowTemperature = AverageTemp();

            // bewaar actuele temperatuur, en de hoogste waarde
            if ( CowTemperature > MaxCowTemperature ){
              MaxCowTemperature = CowTemperature;
            }
          }
          tempatt = 0;
          // Bepaal attentie
          if (p_CowTmpRef[SideState] != NULL && (SideState == GetDisplaySide() )){
            if ( MaxCowTemperature >= (LONG_COW_GEN_NUMBER ?
            	((ExtCowTmpRef **)p_CowTmpRef)[SideState]->CowTmpMaximum :
            	((   CowTmpRef **)p_CowTmpRef)[SideState]->CowTmpMaximum )) tempatt = TRUE;
          }
          CowTempAttention =  tempatt;
          TM_time = Cnt1sec;
        } else {
          // fout in communicatie of in externe aansluiting
          // bewaar laatste fout
          if ( stat == 2 || stat == 6 || stat == 7 ){
            TMErrorInfo.TMErrCode = stat;
            TMErrorInfo.TMErrors++;
            if ( TMErrorInfo.TMErrors > 30 )TMErrorInfo.TMErrors = 30;
          }
        }
      }
    }
  }
}

