// =============================================================================
// Niveau Meter                                             
// =============================================================================


#include "../lib/typedef.h"
#include "../lib/reg_flags.h"
#include "../lib/io6811.h"
#include "../lib/eehc11.h"
#include "../lib/eeprom.h"
#include "../lib/rti_int.h"
#include "../lib/convert.h" 
#include "../lib/intr6811.h"
#include "../key/key.h" 
#include "../appl/takeoff.h" 
#include "../appl/milkmisc.h" 
#include "../comm/flowctrl.h" 
#include "pulsation.h"
#include "milk.h"
#include "niveau.h"

#pragma codeseg(EEPAGE) 

 

/***************************************************************************************/
/*                                                                                     */
/* YKPROCEDURE NIVEAU METER                                                            */
/*                                                                                     */
/* Voor het yken van de niveau meter moeten we eerst een paar handelingen              */
/* bij het maatglas in de melkstal verrichten, hierbij maken we ook gebruik            */
/* van het toetsenbord in de stal. De P0x, P5x en de P15x moeten gemeten               */
/* worden.                                                                             */
/*                                                                                     */
/*       P0x  = gemeten pulsbreedte wanneer de vlotter onderin ligt bij temperatuur x  */
/*       P5x  = gemeten pulsbreedte bij 5 kg melk en temperatuur x                     */
/*       P15x = gemeten pulsbreedte bij 15 kg melk en temperatuur x                    */
/*                                                                                     */
/* Bij het yken kunnen we als volgt te werk gaan. Meetglas vullen met                  */
/* meer dan 15 kg water, water aftappen tot precies 15 kg. Gemeten                     */
/* pulsbreedte vastleggen.                                                             */
/* Water verder laten zakken tot 5 kg. Gemeten pulsbreedte vastleggen                  */
/* Tenslotte het melkglas leeg laten lopen en de pulsbreedte weer vast-                */
/* leggen.                                                                             */
/*                                                                                     */
/*  1) berekening vlotter onderin waarde bij 38,5 'C                                   */
/*                                                                                     */
/*                P1m * (pijpl + 214)                                                  */
/*       Pvltr = --------------------- pulsbreedte                                     */
/*                       10000                                                         */
/*                                                                                     */
/*            P1m = pulsbreedte voor 1 meter pijp, dit is een konstante (1625)         */
/*            pijpl = lengte van de niveau meter pijp in mm.                           */
/*                                                                                     */
/*  2) berekenen d10ltr verschilwaarde bij temp. x                                     */
/*                                                                                     */
/*       Pd10x = Pd5x - Pd15x                                                          */
/*                                                                                     */
/*  3) nulliter waarde bij temp. x                                                     */
/*                                                                                     */
/*       P0x = P5x + Pd10x/2                                                           */
/*                                                                                     */
/*  4) Ykw2 nulliterwaarde bij 38,5 'C  (Ykw1)                                         */
/*     ***************************************                                         */
/*                                                                                     */
/*       P0 = P0x * Pvltr/Pvltrx                                                       */
/*                                                                                     */
/*            Pvltr  = vlotter onderin waarde bij 38,5 'C                              */
/*            Pvltrx = vlotter onderin waarde bij temp. x                              */
/*                                                                                     */
/*  5) Ykw1 d10ltr verschilwaarde bij 38,5 'C                                          */
/*     **************************************                                          */
/*                                                                                     */
/*       Pd10 = Pd10x * Pvltr/Pvltrx  (Ykw0)                                           */
/*                                                                                     */
/*            Pvltr  = vlotter onderin waarde bij 38,5 'C                              */
/*            Pvltrx = vlotter onderin waarde bij temp. x                              */
/*                                                                                     */
/*                                                                                     */
/***************************************************************************************/



/**********************************************************************/
/*                                                                    */
/* TABEL VOOR BEGIN TEMPERATUREN DEZE WAARDEN ZIJN VAN EEN 5415 PIJP  */
/*                                                                    */
/**********************************************************************/


const unsigned short TempTable[80] = {
             31150,          /* 38.5 'C        1494             */
             31060,          /*                919              */
             30980,          /*                920              */
             30900,          /*                921              */
             30800,          /* 35 'C          922              */
             30700,          /*                923              */
             30600,          /*                924              */
             30500,          /*                925              */
             30433,          /*                926              */
             30366,          /*                927              */
             30300,          /* 30 'C          928              */
             30233,          /*                929              */
             30166,          /*                930              */
             30100,          /*                931              */
             30033,          /*                932              */
             29966,          /*                933              */
             29900,          /*                934              */
             29833,          /* 25.33 'C       935              */
             29766,          /*                936              */
             29700,          /*                937              */
             29650,          /*                938              */
             29600,          /*                939              */
             29550,          /*                940              */
             29500,          /*                941              */
             29450,          /*                942              */
             29400,          /*                943              */
             29350,          /*                944              */
             29300,          /*                945              */
             29250,          /*                946              */
             29200,          /*                947              */
             29150,          /*                948              */
             29100,          /*                949              */
             29050,          /*                950              */
             29000,          /*                951              */
             28950,          /*                952              */
             28900,          /*                953              */
             28850,          /*                954              */
             28800,          /* 15 'C          955              */
             28750,          /*                956              */
             28700,          /*                957              */
             28660,          /*                958              */
             28620,          /*                959              */
             28580,          /*                960              */
             28540,          /*                961              */
             28500,          /*                962              */
             28460,          /*                963              */
             28420,          /*                964              */
             28380,          /*                965              */
             28340,          /*                966              */
             28300,          /* 10 'C          967              */
             28260,          /*                968              */
             28220,          /*                969              */
             28180,          /*                970              */
             28140,          /*                971              */
             28100,          /*                972              */
             28066,          /*                973              */
             28033,          /*                974              */
             28000,          /*                975              */
             27966,          /*                976              */
             27933,          /*                977              */
             27900,          /*                978              */
             27866,          /*                979              */
             27833,          /*                980              */
             27800,          /* 5 'C           981              */
             27766,          /*                982              */
             27733,          /*                983              */
             27700,          /*                984              */
             27666,          /*                985              */
             27633,          /*                986              */
             27600,          /*                987              */
             27566,          /*                988              */
             27533,          /*                989              */
             27500,          /*                990              */
             27471,          /*                991              */
             27449,          /*                992              */
             27414,          /*                993              */
             27386,          /*                994              */
             27358,          /*                995              */
             27329,          /*                996              */
             27300           /* 0 'C           997              */
};


unsigned short PulsWidthMeas, PulsWidthCor, PulsWidthAve;
unsigned char  CalState;
unsigned short Pulse15Ltr, Pulse5Ltr, Pulse0Ltr;


// =============================================================================
// NIVEAU MEASURING INTERRUPT ROUTINE                           
// =============================================================================
void HndlMeasurePulseWidth(void) 
{
  static unsigned short StartValue = 0;
  unsigned short PulseW;
  unsigned char Control, MaskOC4;
  unsigned long Pulse;

  Control = TCTL2 & (EDG3A + EDG3B);

  // interrupt falling edge ?? -------------------------------------------------
  if (Control == EDG3B ) {
    StartValue = TIC3;
    //set interrupt on rising edge
    TCTL2 &= (0xff - EDG3B);
    TCTL2 |= EDG3A;
  } else {
    // interrupt rising edge ---------------------------------------------------
    if (Control == EDG3A) {
      if (TIC3 > StartValue) {
        PulseW = TIC3 - StartValue;
      } else {
        PulseW = TIC3 + (0xffff - StartValue);
      }
    }
    
    if (EEMmCorNiv2 != 0) { 
      // Enable interrupts -----------------------------------------------------
      MaskOC4 = TMSK1 & OC4I;
      TMSK1 &= (0xff-OC4I);         // disable scheduler -----------------------
      TMSK1 &= (0xff-IC3I);
      enable_interrupt();
 
      // Adjust pulsewidth in case of using other liquid -----------------------
      Pulse = (unsigned long)EEMmCorNiv2 * (unsigned long)PulseW;
      Pulse /= 100L;
      PulsWidthMeas = (unsigned short) Pulse; 
      
      disable_interrupt();
      TMSK1 |= MaskOC4;
      TMSK1|= IC3I;
 
    } else {
      PulsWidthMeas = PulseW;
    }

//    PulsWidthMeas = PulseW;

    // set interrupt on falling edge -------------------------------------------
    TCTL2 &= (0xff - EDG3A);
    TCTL2 |= EDG3B;
  }
  TFLG1 = IC3F;
}


// =============================================================================
// CALCULATE PulsFloatBottem                                    
// =============================================================================
unsigned short CalcPulsFloatBottom(void)
{
  unsigned long PFloatBottom;

  // Calculate pulswidth when float is at bottom at 38,5'C
  // This value will be calculate using fixed constants which
  // have been measured in the past.
  //
  //                P1m * (Pypl +214)
  // PfloatBottom = ------------------- [clockcycles]
  //                   10000
  //
  //              P1m   = pulswidth for 1 meter pipe, this constant is 2669
  //              Pypl  = pipelength of the levelmeter in mm.
  //
  //

  PFloatBottom = ((2669L * ((unsigned long)EECal4 + 214L))/ 10000L);

  return((unsigned short) PFloatBottom);
}

// =============================================================================
// CALCULATE Calibration Values                                  
// =============================================================================
void CalcCalValues(unsigned short Pulse5Ltr, unsigned short Pulse15Ltr,
                   unsigned short Pulse0Ltr, unsigned short PFloat) 
{
  unsigned short Pd10x, P0x, P0, Pd10;

  // ---------------------------------------------------------------------------
  // Pd10x = Pd5x -Pd15x
  // ---------------------------------------------------------------------------
  Pd10x = Pulse5Ltr - Pulse15Ltr;


  // ---------------------------------------------------------------------------
  // P0x = P5x + Pd10x/2
  // ---------------------------------------------------------------------------
  P0x = Pulse5Ltr + (Pd10x / 2);

  // ---------------------------------------------------------------------------
  // Bereken P0 waarde, Ykw2
  // P0 = P0x * Pvltr/Pvltrx
  // ---------------------------------------------------------------------------
  P0 = ((unsigned long)P0x * (unsigned long)PFloat)/ (unsigned long)Pulse0Ltr ;

  // ---------------------------------------------------------------------------
  // Bereken d10ltr waarde, Ykw1
  // Pd10 = Pd10x * Pvltr/Pvltrx
  // ---------------------------------------------------------------------------
  Pd10 = ((unsigned long)Pd10x * (unsigned long)PFloat) / (unsigned long)Pulse0Ltr;

  WriteEEpromShort(Pd10 , &EECal1);
  WriteEEpromShort(P0 , &EECal2);
} 

 
// =============================================================================
// CALCULATE Calibration Values                                  
// =============================================================================
void CalibrateMilkmeter(void)  
{
  static unsigned char KeyOld;
  unsigned short PFloat;

  if (KeyA != KeyOld) {
    KeyOld = KeyA;
    if (KeyA == KEY_SHIFT || KeyA == KEY_ENTER ) { 
      while (KeyA != KEY_REL);
    }
  }


  switch (CalState) {

    case CAL_START:
      // print start screen ----------------------------------------------------
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          CalState = CAL_15LTR;
        break;
        case KEY_SHIFT:
          CalState = CAL_RDY;
        break;
      }
    break;
    case CAL_15LTR:
      // print pulsewidth 15 ltr; ----------------------------------------------
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          Pulse15Ltr = PulsWidthCor;
          CalState = CAL_5LTR;
        break;

        case KEY_SHIFT:
          CalState = CAL_START;
        break;

      }
    break;

    case CAL_5LTR:
      // print pulsewidth 5 ltr; -----------------------------------------------
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          Pulse5Ltr = PulsWidthCor;
          CalState = CAL_0LTR;
        break;

        // Prev Step -----------------------------------------------------------
        case KEY_SHIFT:
          CalState=CAL_15LTR;
        break;
      }
    break;

    case CAL_0LTR:
      // print pulsewidth 0 ltr; -----------------------------------------------
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          Pulse0Ltr = PulsWidthCor;
          CalState = CAL_VAL1;
          PFloat = CalcPulsFloatBottom();
          CalcCalValues(Pulse5Ltr, Pulse15Ltr, Pulse0Ltr, PFloat); 
          HndlPerGenRes();
        break;
        // Prev Step -----------------------------------------------------------
        case KEY_SHIFT:
          CalState=CAL_5LTR;
        break;
      }
    break;

    
    case CAL_VAL1:
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          CalState = CAL_VAL2;
        break;
      }
    break;

    case CAL_VAL2:
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          CalState = CAL_RDY;
        break;
        case KEY_SHIFT:
          CalState = CAL_VAL1;
        break;
      }
    break;
    
    case CAL_RDY:
      switch (KeyOld) {
        // Next step -----------------------------------------------------------
        case KEY_ENTER:
          CalState = CAL_START;
        break;  
        case KEY_SHIFT:
          DispClear();
          CalState = CAL_VAL2;
        break;
      }
    break;

    
  }
}


// =============================================================================
// AvePulsWidth                                                 
//                                                              
// Bewaar laatste 10 meetpulsen en bepaal gemiddelde over de    
// middelste 8, hoogste en laagste waarde niet meenemen         
// =============================================================================
void CalcAvePulsWidth(void)
{
  static unsigned short Puls[10];
  unsigned long PulsWidthMeasAve;
  unsigned short PulsMeas;
  signed char i,j;

  if (MlkMode == MM_INIT) {
    for (i = 0; i < 10; i++ ) {
      Puls[i] = 0;
    }
  }


  if (PulsWidthMeas > 100 && PulsWidthMeas < 2000) {
    PulsMeas = PulsWidthMeas;

    for (i= 0; i<=9; i++) {
      if (Puls[i] == 0 ) {
        Puls[i] = PulsMeas;
        i = 10;
      } else {
        if (PulsMeas <= Puls[i] ) {
          if (i < 9 ) {
            for (j = 9; j > i; j--) {
              Puls[j] = Puls[j-1];
            }
          }
          Puls[i] = PulsMeas;
          i = 10;
        }
      }
    }

    if ((PulsMeas >= Puls[9]) && i != 11){
      for (i= 0; i < 9; i++) {
        Puls[i] = Puls[i+1];
      }
      Puls[9] = PulsMeas;
    }

    PulsWidthMeasAve = 0;
    j= 0;
    for(i=1; i<9; i++) {
      if (Puls[i] != 0) {
        PulsWidthMeasAve += Puls[i];
        j++;
      }
    }
    PulsWidthMeasAve /= j;
    PulsWidthAve = (unsigned short)PulsWidthMeasAve;
  }
}


// =============================================================================
// Bepaal Begin Temperatuur nivometer buis                    
// =============================================================================
unsigned short CalcTBegin(unsigned short PulsWidth)
{
  unsigned short PulsFloatBottom;
  unsigned long Pulse;
  unsigned short TBegin;

  PulsFloatBottom = CalcPulsFloatBottom();

  // determined start temperature ----------------------------------------------
  if (PulsWidth <= PulsFloatBottom) {
    TBegin = 31150;        // Max temp. 38.5 'C --------------------------------
  } else {
    Pulse = (unsigned long)PulsWidth * 5629L;
    // Adjust pulse width (pipelength + distance between pipe and sensor) ------
    Pulse /= ((unsigned long)EECal4 + 214L);
    if (Pulse >  1623L) {
      TBegin = 27300;            // Min. temp 0'C
    } else {
      if (Pulse < 1494L) {       // Min. pulse witdt by 38,5'C
        TBegin = 31150;          // Max temp 38,5'C
      } else {
        Pulse -= 1494L;
        Pulse *= 1000L;
        Pulse /= 1628L;          // adjust to frequency of 4.9152 MHz
        TBegin = TempTable[Pulse];
      }
    }
  }
  return(TBegin);
}


// =============================================================================
// Bereken Melk gift                                           
// =============================================================================
unsigned long CalcMilkYield(unsigned short PulsWidth, unsigned short KWarm,
                             unsigned short TBegin)
{
  unsigned long Pulse, Tmp, MilkAct;
  unsigned char CorConNiv;
  unsigned short Tend;

  CorConNiv = EEMmCorNiv1;
  MilkAct = 0;

  // Milk yield calculation:   -------------------------------------------------
  // Milk = (Po - Px)/Pdo x 1000
  // Po = nul litre value at 311,5 'K (Cal_2)
  // Pdo = d 10 litre value at 311,5 'K (Cal_1)
  // Px = measured pulswidth

  Pulse = PulsWidth; 
  if (EEGoatMilkGlass != TRUE) { 
    // Pulse width correct? ----------------------------------------------------
    if (Pulse <= (unsigned long)EECal2) {
      MilkAct = ( (((unsigned long)EECal2 - Pulse) * 1000L) / (unsigned long)EECal1) ;

      // Calculate end temperature after milking
      // Tend = ("MilkAct"*31150 + Tbegin*KWarm)/KWarm+"MilkAct")
      // MilkAct = actual milk yield

      Tend = (unsigned short)(((MilkAct * 31150L) + ((unsigned long)TBegin*(unsigned long)KWarm))/((unsigned long)KWarm+MilkAct));

      // Calculating actual milkyield
      //
      // MilkActual = (Pox - Px) * 1000 /Pd10x in xx.xx kg
      //   Pox = nul litre valua at temperature x
      //   Pd10x = d 10 litre difference value at temperature x
      //   Px = measured puls width
      //
      //        Pox = Po + (31150-Teind)/75
      //        Po  = Nul litre value at 311,5 'K
      //
      //        Pd10x = Pd10 + ((31150 - Teind)/75)/4
      //        Pd10 = 10 litre difference value at 311,5 'K
      //
      //             (Po + (31150 - Teind)/75 - Px) * 1000
      //   MilkAct = -------------------------------------  in xx.xx kg
      //               Pd10 + ((31150 - Teind) /75)/4
      //

      Tmp = ((31150L - (unsigned long)Tend) * 100L)/ 7500L ;
      MilkAct = ((unsigned long)EECal2 +Tmp - Pulse) * 1000L / ((unsigned long)EECal1 + (Tmp/4L));

      // Milkyield binairy in grams --------------------------------------------
      MilkAct *= 10L;
     
      // Correctie constante ingevoerd ? ---------------------------------------
      if (CorConNiv) {
        if ((CorConNiv >= 1) && (CorConNiv <=10)) {
          MilkAct = MilkAct - ((unsigned long)CorConNiv * 100L);
        } else {
          if ((CorConNiv >= 11) && (CorConNiv <=20)) {
            MilkAct = MilkAct + (((unsigned long)CorConNiv - 10L) *100L);
          }
        }
      }
    }  
  } else {    
/*
    // Geiten melkglas
    // Formule: Aantal tienden kg = -0.105 x puls + 110,9 
    // Deze formule is gebaseerd op de meetwaarden van Henk.
    // Hieruit bleek dat de grafiek nagenoeg een rechte lijn is.
    // De formule is berekend uit de volgende gegevens.
    // 1.0 kg  P=960, 5.0 kg P=580 (verschil 380)
    
    // Meetwaarden bij Aarts 1.0 kg P=1027, 5.0 kg  P=639 (verschil 388), 0.3 kg P = 1133
    // Lineare gelijk maar ofset is verschillend
    // Nieuwe formule Puls < 1030: Aantal tiende kg = -0.105 x Puls + 117,8
    // Nieuwe formule Puls >= 1030: Aantal tiende kg = - 0.066 x Puls +77,8
        
//    MilkAct = (((105 * Pulse) * -1) + 110900)/10; 
    if (Pulse < 1030) {
      MilkAct = (((105 * Pulse) * -1) + 117800)/10; 
    } else {
      MilkAct = (((66 * Pulse) * -1) + 77800)/10; 
    }
*/  
  
   
//  Geiten melkglas
//  Calibratie waarde 1 = tellerwaarde 0.3 kg  (1133)
//  Calibratie waarde 2 = tellerwaarde flotter onder in (1300)
//  Calibratie waarde 3 = tellerwaarde 1.0 kg   (1027)
//  Calibratie waarde 4 = tellerwaarde 5.0 kg   (639)
    
//  De melkgift wordt berekend aan de hand van twee formules welke een
//  rechte lijn zijn in de bereiken van 0-1.0 kg en groter dan 1.0 kg
//  De lijnen worden berkend a.h.v. van de formule voor een rechte lijn
//   
//  y - y1 / y2 - y1 = x - x1 / x2 - x1
//  y = (y2 - y1) * (x - x1)/(x2-x1) + y1
//
//  De formule wordt voor bereik > 1.0 kg 
//  Y1 = 5000   X1 = Cal3
//  Y2 = 1000   X2 = Cal4
//  y = -400 * (x - Cal4)/(Cal3 - Cal4) + 5000 
//
//
//  De formule wordt voor bereik < 1.0 kg 
//  Y1 = 1000   X1 = CAL1
//  Y2 = 300    X2 = CAL3
//  y = -700 * (x - Cal3)/(Cal1 - Cal3) + 1000 
//
    //Berekening melkgift in grammen
    if (Pulse < (unsigned long)EECal3) {
      MilkAct = (-1 * ((4000L *((signed long)Pulse - (signed long)EECal4))/((signed long)EECal3 - (signed long)EECal4))) + 5000L;
    } else {
      MilkAct = (-1 * ((700L * ((signed long)Pulse - (signed long)EECal3))/((signed long)EECal1 - (signed long)EECal3))) + 1000L;
    }


     
    //Correctie constante ingevoerd ?
    //Bij geiten glas alleen corrigeren bij melkgift > 1kg 
    if (CorConNiv && (MilkAct > 1000)) {
      if ((CorConNiv >= 1) && (CorConNiv <=10)) {
        MilkAct = MilkAct - ((unsigned long)CorConNiv * 100L);
      } else {
        if ((CorConNiv >= 11) && (CorConNiv <=20)) {
          MilkAct = MilkAct + (((unsigned long)CorConNiv - 10L) *100L);
        }
      }
    }
    
    //Geen goede meting als melkgift is groter dan 99 kg
    if (MilkAct > 99000) {
      MilkAct = 0;
    }  
  }            
  return(MilkAct);
}

// =============================================================================
// NIVEAU MEASURING                                           
// =============================================================================
void NiveauMeasuring(void) 
{
  unsigned char TimerControl;
  unsigned long MilkAct;
  unsigned short PulsWidth;
  static unsigned char TempMeasuring, InitBeginTemp;
  static unsigned short KWarm, TBegin, TBeginDhia, TBeginDhiaPrev,
                      PulsFloatBottom, PulsWidthAvePrev, Cnt1secLast;
                      

  switch (MlkState) {
    case MLK_CAL:
      MlkMode =  MM_CAL;
    break;
    case MLK_CLEANING: 
         CheckPulsation();
         if ( (EECleanTime == 0) && (EEOutputP1Inv == 1)){
           // deactivate P1
           ExtOutputP1State = P1_ON;          // PortP1Off();
         }  
         // NO BREAK
    case MLK_CLEANING_END:
         if ( ((EEI2Control & 0x03) == 0) || (((EEI2Control & 0x03) == 0x02) && EERowSize) && (MlkState == MLK_CLEANING_END)){
//         if ( ((EEI2Control == 0) || ((EEI2Control == 2) && EERowSize)) && MlkState == MLK_CLEANING_END){
           SetStartSwitchState(); 
           if ( StartSwitchState == SWITCH_CLOSED ){
             ul_CleanStartTime = Cnt1sec;
             TakeOffState      = TO_START_CLEAN;
             MlkState          = MLK_CLEANING;
           }  
         }
         // NO BREAK                           
    case MLK_WAIT_FOR_START:
      CheckPulsation();
      MlkMode = MM_INIT;
      
      //DHIA - Initialisatie
      TempMeasuring = TRUE;
      Cnt1secLast = Cnt1sec;
      TBeginDhia = 31150;
      TBeginDhiaPrev = 0xffff;
    
      
      InitBeginTemp = FALSE;
      MlkAmountRobot = 0;
    break;
    case MLK_DHIA:
      MlkMode = MM_DHIA;
    break;
    default:
      CalState = CAL_START;
    break;
  }

  CalcAvePulsWidth();

  switch (MlkMode) {
    case MM_INIT    :
      // Init Niveau measuring -------------------------------------------------

      // Calculate Pulsewidth when flotter is at bottom position ---------------
      PulsFloatBottom = CalcPulsFloatBottom();

      // Set Interrupt registers -----------------------------------------------
      TimerControl = TCTL2;
      TimerControl |= EDG3B;
      TimerControl &= 0xff - EDG3A;
      TCTL2 = TimerControl;
      TFLG1 = IC3F;
      TMSK1 |= IC3I;

      PulsWidth = 0;        
      if (InitBeginTemp == FALSE) {

// Instelling zoals in oude bpmp
//        TBegin = 31100;              //311' Kelvin Start Temperature 38'C
//        KWarm  = 950;                //KWarm after cleaning
//        InitBeginTemp = TRUE;

        // Instelling nieuw ----------------------------------------------------
        TBegin = 29833;               // 298' Kelvin Start Temperature 25'C
        KWarm  = 6000;                // KWarm after cleaning
        InitBeginTemp = TRUE;
      }
      
/*      
      //DHIA - Initialisatie
      TBeginDhia = 31150;
      TBeginDhiaPrev = 0xffff;
//      TBeginDhiaPrev = 31150;
      TempMeasuring  = FALSE;
*/ 

      StoredMlkAmount = 0;
      MlkAmount       = 0;
      MlkAmountKg     = 0;
      MlkDuration     = 0;
      MlkActFlow      = 0;
      MlkMaxFlow      = 0;
      MlkMode = MM_MEAS;
    break;


    case MM_MEAS:
      if ((PulsWidthAve > 365) && (PulsWidthAve <1870) ) {
        if (PulsWidthAve == PulsWidthAvePrev) {
          PulsWidth = PulsWidthAve;
 

          switch(MlkState) {
            // End of milksession ? --------------------------------------------
            case MLK_DONE:
              TBegin = CalcTBegin(PulsWidth);
              KWarm = 6000;
              
              if (EERobot == TRUE) {
                // Milk yield calculation: -------------------------------------
                if (Cnt1sec != Cnt1secLast) {
                  MlkAmountRobot = CalcMilkYield(PulsWidth, KWarm, TBegin);
                }
              }
            break;

            // Milking?? -------------------------------------------------------
            case MLK_IDLE:
            case MLK_NEUTRAL:
            case MLK_MEASURING:
            case MLK_TAKE_OFF_WAIT:
            case MLK_TAKE_OFF_MAN:
              // Milk yield calculation: ---------------------------------------
              MilkAct = CalcMilkYield(PulsWidth, KWarm, TBegin);
              // save milk yield every second, to determine milk flow ----------
              if (Cnt1sec != Cnt1secLast) {
                if (MilkManualInput == FALSE) {
                  MlkAmount = MilkAct;
                  MlkAmountRobot = MilkAct;
                  MlkAmountKg = (MlkAmount+50)/100;
                }
                PutMlkTbl(MlkAmount,100);

                // bewaar ook het verloop van de melkgift iedere 5 sec. ------------    
                if ( (Cnt1sec % 5) == 0 ){
                  PutMlkAmountInTable((unsigned char)(((MlkAmount-StoredMlkAmount)+5)/10));
                  StoredMlkAmount = MlkAmount; 
                }
                Cnt1secLast = Cnt1sec;
              }
            break;

            default:
            break;
          }
        } else {
          PulsWidthAvePrev = PulsWidthAve;
        }
      }
      break;
    case MM_CAL:
      if (PulsWidthAve == PulsWidthAvePrev) {
        PulsWidthCor = PulsWidthAve;
      }
      PulsWidthAvePrev = PulsWidthAve;

      CalibrateMilkmeter();

      break;

    case MM_DHIA:

      if (PulsWidthAve == PulsWidthAvePrev) {
        PulsWidth = PulsWidthAve;                
         
        // Wordt er een geiten melkglas gebruikt ? -----------------------------
        if (EEGoatMilkGlass != TRUE) { 
          // Nee geen geiten melkglas ------------------------------------------
          // Pulsbreedte groter kan calwaarde 2 --------------------------------
          if (TempMeasuring == TRUE) {
            // Mag temperatuur berekekend worden -------------------------------
            if (Cnt1sec > (Cnt1secLast + 10)) {
              MilkAct = CalcMilkYield(PulsWidth, 6000, TBeginDhia);
              if (MilkAct >=2500) {
                TempMeasuring = FALSE;
              } else {
                KWarm = 6000;
                TBegin = CalcTBegin(PulsWidth);
                if (TBegin < TBeginDhiaPrev) {
                  TBeginDhia = TBegin;
                  TBeginDhiaPrev = TBegin;
                }
              }
              MlkAmountKg = 0;
            }
          } 
          else {
            //Melkberekenen ----------------------------------------------------
// Temperatuur berekening waarschijnlijk niet nodig bij watertest
// Dit omdat de watertest altijd bij 38,5 °C  plaats vindt
// Een en ander zal door Henk B nog worden getest. Programma kan in 
// toekomst definitief aangepast worden.           
//          MilkAct = CalcMilkYield(PulsWidth, 6000, 31150);
            MilkAct = CalcMilkYield(PulsWidth, 6000, TBeginDhia);
            // MilkAct > 2,5 kg. -----------------------------------------------
            if (MilkAct > 2500) {
              Cnt1secLast = Cnt1sec;
              TempMeasuring = FALSE;
              TBeginDhiaPrev = 0xffff;
              //melkgift nog niet constant, rode lamp is uit 
              if (!(GetDhiaAtt(FALSE)) || MilkAct <9000)  {
                MlkAmountKg = (MilkAct+50)/100;               
              } 
            } else {
              if (MilkAct < 2000 && TBeginDhiaPrev == 0xffff) {
                TempMeasuring = TRUE;
              }
              MlkAmountKg = 0;
            }
          }
        } else {
          // Geiten melkglas ---------------------------------------------------
          MilkAct = CalcMilkYield(PulsWidth, KWarm, TBegin);
          // MilkAct > 0,5 kg. -------------------------------------------------
          if (MilkAct > 50) {
            Cnt1secLast = Cnt1sec;
            //melkgift nog niet constant, rode lamp is uit 
            if (!(GetDhiaAtt(FALSE)) || MilkAct <4000)  {
              MlkAmountKg = (MilkAct+50)/100;               
            } 
          }
        }  
      } else {
        PulsWidthAvePrev = PulsWidthAve;
      }


      break;

    default :
    break;
  }
}

// =============================================================================
void NiveauTakeOff(void) 
{
}

