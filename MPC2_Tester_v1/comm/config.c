// =============================================================================
// 
//  configuratie tabel voor het EXPERTsysteem 
// 
// =============================================================================

// include files ---------------------------------------------------------------

#include "../comm/config.h"

#pragma codeseg(COMMON)

// initialisatie peripheral configuratie data ----------------------------------

const CONFIG_ROM_INFO PerConfigData = {

  99,  // PerGenProgCode
  0,   // PerGenFuncIdentification 
  1,   // PerGenFuncFeeding 
  1,   // PerGenFuncMilkMeasuring 
  0,   // PerGenFuncAccess 
  0,   // PerGenFuncSeparation 
  0,   // PerGenFuncWeight 
  1,   // PerGenFuncTemperature 
  0,   // PerGenFuncConductivity 
  0,   // PerGenFuncActivity 
  0,   // PerGenFuncDrinking 
  0,   // PerGenFuncMarking 
  40,  // PerGenFuncType 
  1    // PerGenDownLoad 
};




