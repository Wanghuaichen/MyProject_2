

#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/intr6811.h"
#include "../lib/eehc11.h"
#include "../lib/time.h"
#include "milkmisc.h"
#include "pulsation.h"
#include "niveau.h"
#include "memolac.h"
#include "milk.h"

unsigned short Cnt10msec = 0;
unsigned char  SensorM1Cntr;
unsigned char  Sensor1Cntr;
unsigned char  Sensor2Cntr;
unsigned char  Sensor3Cntr;
unsigned char  Sensor4Cntr;

extern void HndlVflowOCInt(void);
extern void HndlVflowM3Int(void);
#pragma codeseg(COMMON)

// =============================================================================
// Interrupt on MilkInput M1
// =============================================================================
non_banked interrupt void MilkInputM1(void)
{
  switch(MilkMeterType) {
    case MMT_MEMOLAC:   HndlMemolacM1Int();         break;
    case MMT_NIVEAU:    HndlMeasurePulseWidth();    break;
    default:            TFLG1 = IC3F;               break;
  }
}

// =============================================================================
// Interrupt on MilkInput M2
// =============================================================================
non_banked interrupt void MilkInputM2(void)
{

  switch(MilkMeterType) {
    case MMT_MEMOLAC:    HndlMemolacM2Int();        break;
    default:             TFLG1 = IC2F;              break;
  }
}

// =============================================================================
// Interrupt on MilkInput M3
// =============================================================================
non_banked interrupt void MilkInputM3(void)
{
  switch(MilkMeterType) {
//    case MMT_SURGE:
//      HndlSurgeTakeOffInt();
//    break;
    case MMT_VFLOW:       HndlVflowM3Int();         break;
    case MMT_MEMOLAC:     HndlMemolacM3Int();       break;
    default:              TFLG1 = IC1F;             break;
  }
}


// =============================================================================
// TOC Interrupt
// =============================================================================
non_banked interrupt void  OC2_int(void)
{
  Cnt10msec++;

  TMSK1 &= ~OC2I;
  enable_interrupt();


  // filter dender van M1 schakelaar
  if (PORTA & P_0) {
    if (SensorM1Cntr > 0) SensorM1Cntr--;                      // Schakelaar open
  } else  {
    if (SensorM1Cntr < SENSORM1_CNTR_MAX ) SensorM1Cntr++;     // Schakelaar dicht
  }

  // filter dender schakelaar I2
  if (PORTE & P_3) {
    if (Sensor1Cntr > 0) Sensor1Cntr--;                        // Schakelaar open
  } else  {
    if (Sensor1Cntr < SENSOR1_CNTR_MAX ) Sensor1Cntr++;        // Schakelaar dicht
  }

  // filter dender schakelaar I2
  if (PORTE & P_2) {
    if (Sensor2Cntr > 0) Sensor2Cntr--;                        // Schakelaar open
  } else  {
    if (Sensor2Cntr < SENSOR2_CNTR_MAX ) Sensor2Cntr++;        // Schakelaar dicht
  }

  // filter dender schakelaar I3
  if (PORTE & P_1 ){
    if (Sensor3Cntr > 0) Sensor3Cntr--;                        // Schakelaar open
  } else  {
    if (Sensor3Cntr < SENSOR3_CNTR_MAX ) Sensor3Cntr++;        // Schakelaar dicht
  }

  // filter dender schakelaar I4
  if (PORTE & P_0 ){
    if (Sensor4Cntr > 0) Sensor4Cntr--;                        // Schakelaar open
  } else  {
    if (Sensor4Cntr < SENSOR4_CNTR_MAX ) Sensor4Cntr++;        // Schakelaar dicht
  }

  // handle eventueel de pulsatie
  if ( EEPulsation ) HndlPulsation();

  disable_interrupt();
  // het is mogelijk dat er al 10 milliseconden verstreken zijn sinds de interrupt
  // kwam. Indien dit het geval is zetten we TOC2 aan de hand van TCNT.
  if ( TimeControl(TCNT, TOC2+20300))  TOC2 = TCNT+300;
  else                                 TOC2 += 20000;

  TFLG1  = OC2F;
  TMSK1 |= OC2I;
}

non_banked interrupt void OC1_int(void )
{
  HndlVflowOCInt();
}
