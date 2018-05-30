                                                                                                        // Service Items  --------------------------------------------------------------
#include "../appl/niveau.h"
#include "../include/romeehc1.h"
#include "extport.h"
#include "applver.h"
#include "typedef.h"
#include "text.h"
#include "eehc11.h"
#include "item.h"




// volgende items worden niet overschreven met default settings

const ITEM_INFO AddressEE = {                   // Address Node
      (unsigned char *)&EEAddress,              // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_ADDRESS,                              // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO Address2EE = {                  // Address Node
      (unsigned char *)&EEAddress2,             // EEAddress2, last or second
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_ADDRESS2,                             // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MilkMeterTypeEE = {             // Type Melkmeter
      (unsigned char *)&EEMilkMeterType,        // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      40,                                       // Minimum Value
      150,                                      // Maximum Value
      TXT_MM_TYPE,                              // Text number
      42,                                       // Default Value
      WRITE                                     // Attribute
};

// vanaf hier worden de default waarde in de EEprom geschreven bij aanroep
// SetFactorySettings

const ITEM_INFO MmCorMemEE    = {               // Item
      (unsigned char *)&EEMmCorMem,             // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_MM_COR_MEM,                           // Text number
      42,                                       // Default Value
      WRITE                                     // Attribute
};

// tbv NIVEAU METER
const ITEM_INFO MmCorNiv1EE    = {              // Item
      (unsigned char *)&EEMmCorNiv1,            // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_MM_COR_NIV1,                          // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MmCorNiv2EE    = {              // Item
      (unsigned char *)&EEMmCorNiv2,            // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      2,                                        // Number of Decimals
      0,                                        // Minimum Value
      255,                                      // Maximum Value
      TXT_MM_COR_NIV2,                          // Text number
      111,                                      // Default Value
      WRITE                                     // Attribute
};
// ************

const ITEM_INFO ValveTimeEE  = {
      (unsigned char *)&EEValveTime  ,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME,                           // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};
/*
const ITEM_INFO ValveTime1EE  = {
      (unsigned char *)&EEValveTime1 ,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME1,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime2EE  = {
      (unsigned char *)&EEValveTime2,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME2,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime3EE  = {
      (unsigned char *)&EEValveTime3,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME3,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime4EE  = {
      (unsigned char *)&EEValveTime4,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME4,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime5EE  = {
      (unsigned char *)&EEValveTime5,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME5,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime6EE  = {
      (unsigned char *)&EEValveTime6,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME6,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime7EE  = {
      (unsigned char *)&EEValveTime7,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME7,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime8EE  = {
      (unsigned char *)&EEValveTime8,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME8,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime9EE  = {
      (unsigned char *)&EEValveTime9,           // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME9,                          // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime10EE  = {
      (unsigned char *)&EEValveTime10,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME10,                         // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ValveTime11EE  = {
      (unsigned char *)&EEValveTime11,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME11,                         // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO ValveTime12EE  = {
      (unsigned char *)&EEValveTime12,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME12,                         // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO ValveTime13EE  = {
      (unsigned char *)&EEValveTime13,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME13,                         // Text number
      550,                                      // Default Value
      WRITE                                     // Attribute
};
*/
const ITEM_INFO FillTimeEE  = {
      (unsigned char *)&EEFillTime,               // Address
      _S1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      -99,                                      // Minimum Value
      99,                                       // Maximum Value
      TXT_FILL_TIME,                            // Text number
      40,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO LanguageEE    = {
      (unsigned char *)&EELanguage,             // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      6,                                        // Maximum Value
      TXT_LANGUAGE,                             // Text number
      0,                                        // Default Value, ENGELS
      WRITE                                     // Attribute
};

const ITEM_INFO SendTimeEE         = {          // Item
      (unsigned char *)&EESendTime,             // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      1,                                        // Minimum Value
      100,                                      // Maximum Value
      TXT_SEND_TIME,                            // Text number
      10,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO NeutralTime1EE = {              // Item
      (unsigned char *)&EENeutralTime1,         // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_NEUTRAL_TIME1,                        // Text number
      90,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO NeutralTime2EE = {              // Item
      (unsigned char *)&EENeutralTime2,         // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_NEUTRAL_TIME2,                        // Text number
      30,                                       // Default Value
      WRITE                                     // Attribute
};


const ITEM_INFO NeutralThresholdEE = {          // Item
      (unsigned char *)&EENeutralThreshold,     // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      000,                                      // Minimum Value
      9999,                                     // Maximum Value
      TXT_NEUTRAL_TRESHOLD,                     // Text number
      230,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TakeOffThresholdEE = {          // Item
      (unsigned char *)&EETakeOffThreshold,     // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      000,                                      // Minimum Value
      9999,                                     // Maximum Value
      TXT_TAKE_OFF_TRESHOLD,                    // Text number
      230,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TakeOffTimeEE      = {          // Item
      (unsigned char *)&EETakeOffTime,          // EEAddress
      _S2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      -120,                                     // Minimum Value
      120,                                      // Maximum Value
      TXT_TAKE_OFF_TIME,                        // Text number
      10,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MlkFlwCalcTimeEE   = {          // Item
      (unsigned char *)&EEMlkFlwCalcTime,       // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      5,                                        // Minimum Value
      60,                                       // Maximum Value
      TXT_MLK_FLW_CALC_TIME,                    // Text number
      16,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO OutputBlockBitsEE = {           // Item
      (unsigned char *)&EEOutputBlockBits,      // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      15,                                       // Maximum Value
      TXT_OUTPUT_BLOCK_BITS,                    // Text number
      0,                                       // Default Value
      WRITE                                     // Attribute
};

//#if _VERSION_25_
const ITEM_INFO SepMsgCntrlEE = {               // Item
      (unsigned char *)&EESepMsgCntrl,          // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      15,                                       // Maximum Value
      TXT_SEP_MSG_CNTRL_BITS,                   // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SepActFuncNrEE = {              // Item
      (unsigned char *)&EESepActFuncNr,         // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_SEP_ACT_FUNC_NR,                      // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SepCondFuncNrEE = {             // Item
      (unsigned char *)&EESepCondFuncNr,        // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_SEP_COND_FUNC_NR,                     // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SepMinYldFuncNrEE = {           // Item
      (unsigned char *)&EESepMinYldFuncNr,      // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_SEP_MIN_YLD_FUNC_NR,                  // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SepTmpFuncNrEE = {              // Item
      (unsigned char *)&EESepTmpFuncNr,         // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_SEP_TMP_FUNC_NR,                      // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};
//#endif

const ITEM_INFO ClusterDownEE     = {           // Item, tijd waarna het cluster weer zakt
      (unsigned char *)&EEClusterDown,          // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_CLUSTER_DOWN,                         // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ClusterDlyEE       = {          // Item, tijd waarna het cluster hoog komt
      (unsigned char *)&EEClusterDly,           // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      10,                                       // Maximum Value
      TXT_CLUSTER_DLY,                          // Text number
      2,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO VacuumDlyEE        = {          // Item
      (unsigned char *)&EEVacuumDly,            // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      10,                                       // Maximum Value
      TXT_VACUUM_DLY,                           // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MilkSweepDlyTimeEE  = {
      (unsigned char *)&EEMilkSweepDlyTime,     // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_MLK_SWP_DLY_TM,                       // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MilkSweepActTimeEE  = {
      (unsigned char *)&EEMilkSweepActTime,     // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      1,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_MLK_SWP_ACT_TM,                       // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO I2ControlEE        = {          // Item
      (unsigned char *)&EEI2Control,            // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      7,                                        // Maximum Value
      TXT_I2_CONFIG,                            // Text number
      6,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO OutputK2EE         = {          // Item
      (unsigned char *)&EEOutputK2    ,         // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      3,                                        // Maximum Value
      TXT_OUTP_K2,                              // Text number
      3,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO FlowLvl2ThresholdEE = {         // Item
      (unsigned char *)&EEFlowLvl2Threshold,    // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      000,                                      // Minimum Value
      2000,                                     // Maximum Value
      TXT_FLW_LVL2_THRSH,                       // Text number
      800,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TrcArmOnTimeEE = {             // Item
      (unsigned char *)&EETrcArmOnTime,       // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      100,                                      // Maximum Value
      TXT_TRACTION_ARM_ON_TIME,                 // Text number
      25,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TrcArmOffTimeEE = {             // Item
      (unsigned char *)&EETrcArmOffTime,        // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      100,                                      // Maximum Value
      TXT_TRACTION_ARM_OFF_TIME,                // Text number
      5,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TAPulsFreqEE = {                // Item
      (unsigned char *)&EETAPulseFreq,          // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      50,                                       // Minimum Value
      360,                                      // Maximum Value
      TXT_TRACTION_ARM_PLS_FREQ,                // Text number
      120,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TAPulsRatioEE = {               // Item
      (unsigned char *)&EETAPulseRatio,         // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      35,                                       // Minimum Value
      70,                                       // Maximum Value
      TXT_TRACTION_ARM_PLS_RAT,                 // Text number
      35,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CleanStartTimeEE   = {          // Item
      (unsigned char *)&EECleanStartTime,       // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      500,                                      // Maximum Value
      TXT_CLEAN_START_TIME,                     // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

// tbv NIVEAU METER
const ITEM_INFO OutputP1DlyEE      = {          // Item
      (unsigned char *)&EEOutputP1Dly,          // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_OUTPUT_P1_DLY,                        // Text number
      45,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO OutputP1TimeEE     = {          // Item
      (unsigned char *)&EEOutputP1Time,         // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_OUTPUT_P1_TIME,                       // Text number
      15,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO OutputP1InvEE      = {          // Item
      (unsigned char *)&EEOutputP1Inv,          // EEAddress
      _BL,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_OUTPUT_P1_INV,                        // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};


const ITEM_INFO CleanTimeEE  = {
      (unsigned char *)&EECleanTime  ,          // Address
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      999,                                      // Maximum Value
      TXT_CLEAN_TIME,                           // Text number
      90,                                      // Default Value
      WRITE                                     // Attribute
};
// ******************

const ITEM_INFO GateSwitchTimeEE  = {           // Item
      (unsigned char *)&EEGateSwitchTime,       // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_GATE_SWITCH_TIME,                     // Text number
      5,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO DispTimeEE  = {                 // Item
      (unsigned char *)&EEDispTime,             // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_DISPLAY_TIME,                         // Text number
      5,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MmSensorMemEE    = {            // Item
      (unsigned char *)&EEMmSensorMem,          // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_MM_SENSOR_MEM,                        // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorVFlowEE  = {
      (unsigned char *)&EECorVFlow,             // Address
      _S1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      -125,                                     // Minimum Value
      125,                                      // Maximum Value
      TXT_COR_FLOW0,                            // Text number
      100,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow0EE  = {
      (unsigned char *)&EECorFlow0,             // Address
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      200,                                      // Maximum Value
      TXT_COR_FLOW0,                            // Text number
      50,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow1EE  = {
      (unsigned char *)&EECorFlow1,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW1,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow2EE  = {
      (unsigned char *)&EECorFlow2,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW2,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow3EE  = {
      (unsigned char *)&EECorFlow3,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW3,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow4EE  = {
      (unsigned char *)&EECorFlow4,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW4,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow5EE  = {
      (unsigned char *)&EECorFlow5,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW5,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow6EE  = {
      (unsigned char *)&EECorFlow6,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW6,                            // Text number
      49,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow7EE  = {
      (unsigned char *)&EECorFlow7,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW7,                            // Text number
      49,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow8EE  = {
      (unsigned char *)&EECorFlow8,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW8,                            // Text number
      49,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow9EE  = {
      (unsigned char *)&EECorFlow9,             // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW9,                            // Text number
      51,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow10EE  = {
      (unsigned char *)&EECorFlow10,            // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW10,                           // Text number
      54,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow11EE  = {
      (unsigned char *)&EECorFlow11,                     // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW11,                           // Text number
      57,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow12EE  = {
      (unsigned char *)&EECorFlow12,            // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW12,                           // Text number
      57,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow13EE  = {
      (unsigned char *)&EECorFlow13,            // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW13,                           // Text number
      57,                                       // Default Value
      WRITE                                     // Attribute
};

// tbv NIVEAU METER
const ITEM_INFO Cal1EE      = {                 // Item
      (unsigned char *)&EECal1,                 // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_CAL_EE1,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO Cal2EE      = {                 // Item
      (unsigned char *)&EECal2,                 // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_CAL_EE2,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO Cal3EE      = {                 // Item
      (unsigned char *)&EECal3,                 // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_CAL_EE3,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO Cal4EE      = {                 // Item
      (unsigned char *)&EECal4,                 // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_CAL_EE4,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GoatMilkGlassEE   = {           // Item
      (unsigned char *)&EEGoatMilkGlass,        // EEAddress
      _BL,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_GOAT_MILK_GLASS,                      // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MmGoatVal1EE   = {              // Item
      (unsigned char *)&EEMmGoatVal1,           // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      999,                                      // Maximum Value
      TXT_MM_GOAT_VAL1,                         // Text number
      000,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MmGoatVal2EE   = {              // Item
      (unsigned char *)&EEMmGoatVal2,           // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      999,                                      // Maximum Value
      TXT_MM_GOAT_VAL2,                         // Text number
      000,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO RobotEE            = {          // Item
      (unsigned char *)&EERobot,                // EEAddress
      _BL,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_ROBOT,                                // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};


// ********************

const ITEM_INFO PrgVrsIC13 = {                  // Address Node
      (unsigned char *)&IC13PrgVrs,             // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_PRG_VRS_13,                           // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO PrgVrsIC09 = {                  // Address Node
      (unsigned char *)&IC09PrgVrs,             // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_PRG_VRS_09,                           // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO PrgVrsIC08 = {                  // Address Node
      (unsigned char *)&IC08PrgVrs,             // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_PRG_VRS_08,                           // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO VersionRom  = {
      (unsigned char *)&RomVersion,             // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      2,                                        // Number of Decimals
      0,                                        // Minimum Value
      0,                                        // Maximum Value
      TXT_VERSION_ROM,                          // Text number
      9999,                                     // Default Value
      READ_ONLY                                 // Attribute
};

const ITEM_INFO ClnPhs1EE  = {
      (unsigned char *)&EEClnPhs1,              // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_CLN_PHS1,                             // Text number
      45,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ClnPhs2EE  = {
      (unsigned char *)&EEClnPhs2,              // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_CLN_PHS2,                             // Text number
      15,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ClnPhs4EE  = {
      (unsigned char *)&EEClnPhs4,              // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_CLN_PHS4,                             // Text number
      5,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO EmptyTimeEE  = {
      (unsigned char *)&EEEmptyTime,            // Address
      _S1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      -99,                                      // Minimum Value
      99,                                       // Maximum Value
      TXT_EMPTY_TIME,                           // Text number
      20,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO PulsationEE    = {              // Item
      (unsigned char *)&EEPulsation,            // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      7,                                        // Maximum Value
      TXT_PULSATION,                            // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

// hier geen item toevoegen ivm weergave conductivity-items die afhankelijk zijn van
// de SoftKey. In key.c wordt hierop getest.
const ITEM_INFO PulsFrequencyEE    = {          // Item
      (unsigned char *)&EEPulseFrequency,       // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      50,                                       // Minimum Value
      240,                                      // Maximum Value
      TXT_PULS_FREQ,                            // Text number
      60,                                       // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO PulsRatioEE       = {           // Item
      (unsigned char *)&EEPulseRatio,           // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      35,                                       // Minimum Value
      70,                                       // Maximum Value
      TXT_PULS_RATIO,                           // Text number
      60,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO StimPulsFreqEE    = {           // Item
      (unsigned char *)&EEStimPulseFreq,        // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      5,                                        // Minimum Value
      360,                                      // Maximum Value
      TXT_STIM_PULS_FREQ,                       // Text number
      120,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO StimPulsRatioEE       = {       // Item
      (unsigned char *)&EEStimPulseRatio,       // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      5,                                        // Minimum Value
      70,                                       // Maximum Value
      TXT_STIM_PULS_RATIO,                      // Text number
      30,                                       // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO StimTresHoldEE    = {           // Item
      (unsigned char *)&EEStimTresHold,         // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_STIM_TRESHOLD,                        // Text number
      500,                                      // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO StimStartTimeEE       = {       // Item
      (unsigned char *)&EEStimStartTime,        // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_STIM_START_TIME,                      // Text number
      20,                                       // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO StimMaxTimeEE       = {         // Item
      (unsigned char *)&EEStimMaxTime,          // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      240,                                      // Maximum Value
      TXT_STIM_MAX_TIME,                        // Text number
      70,                                       // Default Value
      WRITE                                     // Attribute
};
const ITEM_INFO NrOfPulsatorsEE       = {       // Item
      (unsigned char *)&EENrOfPulsators,        // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      1,                                        // Minimum Value
      100,                                      // Maximum Value
      TXT_NR_OF_PULSATORS,                      // Text number
      10,                                       // Default Value
      WRITE                                     // Attribute
};

// hier geen item toevoegen ivm weergave pulsatie-items die afhankelijk zijn van
// PulsationEE. In key.c wordt hierop getest.
// Geldt ook voor EEFeeding

const ITEM_INFO FeedingEE      = {              // Item
      (unsigned char *)&EEFeeding,              // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_FEEDING,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO FeedSpeedEE    = {              // Item
      (unsigned char *)&EEFeedSpeed,            // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_FEED_SPEED,                           // Text number
      300,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO FeedMtrPortionEE  = {           // Item
      (unsigned char *)&(EEFeedMtrPortion[0]),  // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_FEED_PORTION,                         // Text number
      100,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO FeedBalanceDefEE  = {           // Item
      (unsigned char *)&(EEFeedBalanceDef[0]),  // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_FEED_BALANCE_DEF,                     // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO RelaisTimeEE   = {              // Item
      (unsigned char *)&EERelaisTime,           // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_RELAIS_TIME,                          // Text number
      1500,                                     // Default Value
      WRITE                                     // Attribute
};

// volgende items worden gebruikt bij standalone voeren, swing-over

const ITEM_INFO SB_FeedMtrPortionEE  = {        // Item
      (unsigned char *)&(EEFeedMtrPortion[1]),  // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_SB_FEED_PORTION,                      // Text number
      100,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SB_FeedBalanceDefEE  = {        // Item
      (unsigned char *)&(EEFeedBalanceDef[1]),  // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_SB_FEED_BALANCE_DEF,                  // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO SB_RelaisTimeEE   = {           // Item
      (unsigned char *)&EESB_RelaisTime,        // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_SB_RELAIS_TIME,                       // Text number
      1500,                                     // Default Value
      WRITE                                     // Attribute
};

// hier geen item toevoegen ivm weergave voer-items die afhankelijk zijn van
// FeedingEE. In key.c wordt hierop getest.

const ITEM_INFO TestEE             = {          // Item
      (unsigned char *)&EETest,                 // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9,                                        // Maximum Value
      TXT_TEST,                                 // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ParlourTypeEE = {               // Item
      (unsigned char *)&EEParlourType,          // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      6,                                        // Maximum Value
      TXT_PARLOUR_TYPE,                         // Text number
      1,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GateK3EE = {                    // Item
      (unsigned char *)&EEGateK3,               // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_GATE_K3,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GateK4EE = {                    // Item
      (unsigned char *)&EEGateK4,               // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_GATE_K4,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GateK5EE = {                    // Item
      (unsigned char *)&EEGateK5,               // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_GATE_K5,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO OTRowMasterEE  = {
      (unsigned char *)&EEOTRowMaster,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      90,                                       // Maximum Value
      TXT_OT_ROW_MASTER,                        // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ServiceStandsEE  = {
      (unsigned char *)&EEServiceStands,           // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_NR_OF_STANDS,                         // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO RowSizeEE  = {
      (unsigned char *)&EERowSize,              // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      50,                                       // Maximum Value
      TXT_CLN_START_ADDR,                       // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ConMeasuringEE     = {          // Item
      (unsigned char *)&EEConMeasuring,         // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      3,                                        // Maximum Value
      TXT_CON_MEASURING,                        // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ExtConMeasuringEE     = {       // Item
      (unsigned char *)&EEExtConMeasuring,      // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      7,                                        // Maximum Value
      TXT_EXT_CON_MEASURING,                    // Text number
      4,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TempMeasuringEE    = {          // Item
      (unsigned char *)&EETempMeasuring,        // EEAddress
      _BL,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_TEMP_MEASURING,                       // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO TempRValueEE    = {             // Item
      (unsigned char *)&EETempRValue,           // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_RTEMP,                                // Text number
      5720,                                     // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO KeySoft  = {
      (unsigned char *)&SoftKey,                // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      16,                                       // Maximum Value
      TXT_SOFT_KEY,                             // Text number
      0,                                        // Default Value
      READ_ONLY                                 // Attribute
};

const ITEM_INFO VersionAppl = {
       (unsigned char *)&ApplVersion,           // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      2,                                        // Number of Decimals
      0,                                        // Minimum Value
      0,                                        // Maximum Value
      TXT_VERSION_APPL,                         // Text number
      9999,                                     // Default Value
      READ_ONLY                                 // Attribute
};

const ITEM_INFO DefaultEE          = {          // Item
      (unsigned char *)&EEDefault,              // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9,                                        // Maximum Value
      TXT_DEFAULT,                              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};



const ITEM_INFO DetectLevelEE = {               // Minimum conductivity level for milk indication
      (unsigned char *)&EEDetectLevel,          // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      255,                                      // Maximum Value
      TXT_DETECT_LEVEL,                         // Text number
      20,                                       // Default Value
      WRITE                                     // Attribute
};


// ************************************************************************************
const ITEM_INFO ClusterDownDelayHighEE = {      // Display SwitchOff Time
      (unsigned char *) &EEClusterDownDelayHigh, // EEAddress
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_CLUSTER_DOWN_DELAY_HIGH,              // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};




const ITEM_INFO MmOffsetManEE    = {            // Item                            NOT USED  (Manuflow)
      (unsigned char *)&EEMmOffsetMan,          // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      255,                                      // Maximum Value
      TXT_MM_OFFSET_MAN,                        // Text number
      153,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO MmWaterTestManEE    = {         // Item                            NOT USED  (Manuflow)
      (unsigned char *)&EEMmWaterTestMan,       // EEAddress
      _U1,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      255,                                      // Maximum Value
      TXT_MM_WATER_TEST_MAN,                    // Text number
      186,                                      // Default Value
      WRITE                                     // Attribute
};



const ITEM_INFO KickOffFlowProcEE  = {          // Item                            NOT USED (Surge)
      (unsigned char *)&EEKickOffFlowProc,      // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      999,                                      // Maximum Value
      TXT_KICK_OFF_FLOW_PROC,                   // Text number
      50,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO KickOffSampleTimeEE = {         // Item                            NOT USED (Surge)
      (unsigned char *)&EEKickOffSampleTime,    // EEAddress
      _U2,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      5,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_KICK_OFF_SAMPLE_TIME,                 // Text number
      10,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO KickOffFacThresholdEE = {       // Item                            NOT USED (Surge)
      (unsigned char *)&EEKickOffThreshold,     // EEAddress
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      00,                                       // Minimum Value
      9999,                                     // Maximum Value
      TXT_KICK_OFF_FAC_TRESHOLD,                // Text number
      2000,                                     // Default Value
      WRITE                                     // Attribute
};




const ITEM_INFO ValveTimeManEE  = {             // item                         NOT USED (Manuflow)
      (unsigned char *)&EEValveTimeMan,         // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_VALVE_TIME_MAN,                       // Text number
      400,                                      // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow0ManEE  = {                //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow0Man,          //Address
      _U1,                                      //Type definition
      2,                                        //Size, number of Digits
      0,                                        //Number of Decimals
      0,                                        //Minimum Value
      99,                                       //Maximum Value
      TXT_COR_FLOW0_MAN,                        // Text number
      25,                                       //Default Value
      WRITE                                     //Attribute
};

const ITEM_INFO CorFlow1ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow1Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW1_MAN,                        // Text number
      31,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow2ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow2Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW2_MAN,                        // Text number
      33,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow3ManEE  = {              //                                 NOT USED (Manuflow)
      (unsigned char *)&EECorFlow3Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW3_MAN,                        // Text number
      38,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow4ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow4Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW4_MAN,                        // Text number
      41,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow5ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow5Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW5_MAN,                        // Text number
      50,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow6ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow6Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW6_MAN,                        // Text number
      57,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow7ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow7Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW7_MAN,                        // Text number
      62,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow8ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow8Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW8_MAN,                        // Text number
      67,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow9ManEE  = {              //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow9Man,          // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW9_MAN,                        // Text number
      72,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow10ManEE  = {             //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow10Man,         // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW10_MAN,                       // Text number
      77,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow11ManEE  = {             //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow11Man,         // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW11_MAN,                       // Text number
      82,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow12ManEE  = {             //                                NOT USED (Manuflow)
      (char unsigned *)&EECorFlow12Man,         // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW12_MAN,                       // Text number
      87,                                       // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO CorFlow13ManEE  = {             //                                NOT USED (Manuflow)
      (unsigned char *)&EECorFlow13Man,         // Address
      _U1,                                      // Type definition
      2,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      99,                                       // Maximum Value
      TXT_COR_FLOW13_MAN,                       // Text number
      39,                                       // Default Value
      WRITE                                     // Attribute
};




const ITEM_INFO LevelCounter  = {
      (unsigned char *)&PulsWidthMeas,          // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      0,                                        // Maximum Value
      TXT_LEVEL_COUNTER,                        // Text number
      9999,                                     // Default Value
      READ_ONLY                                 // Attribute
};

const ITEM_INFO SerialNrEE  = {                 //                                NOT USED (Surge)
      (unsigned char *)&EESerialNr,             // Address
      _U2,                                      // Type definition
      4,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      9999,                                     // Maximum Value
      TXT_SERIAL_NR,                            // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO ActMeasuringEE      = {         //                                NOT USED (Surge)
      (unsigned char *)&EEActMeasuring,         // EEAddress
      _BL,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_ACT_MEASURING,                        // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};


const ITEM_INFO GateK3TimeEE   = {              // Item
      (unsigned char *)&EEGateK3Time,           // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      1,                                        // Number of Decimals
      0,                                        // Minimum Value
      300,                                      // Maximum Value
      TXT_GATE_K3_TIME,                         // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GateK4TimeEE   = {              // Item
      (unsigned char *)&EEGateK4Time,           // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      1,                                        // Number of Decimals
      0,                                        // Minimum Value
      300,                                      // Maximum Value
      TXT_GATE_K4_TIME,                         // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO GateK5TimeEE   = {              // Item
      (unsigned char *)&EEGateK5Time,           // EEAddress
      _U2,                                      // Type definition
      3,                                        // Size, number of Digits
      1,                                        // Number of Decimals
      0,                                        // Minimum Value
      300,                                      // Maximum Value
      TXT_GATE_K5_TIME,                         // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

const ITEM_INFO I1ControlEE        = {          // Item
      (unsigned char *)&EEI1Control,            // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      2,                                        // Maximum Value
      TXT_I1_CONFIG,                            // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};

#if _HARTMAN_
const ITEM_INFO HartmanEE        = {            // Item
      (unsigned char *)&EEHartman,              // EEAddress
      _U1,                                      // Type definition
      1,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      1,                                        // Maximum Value
      TXT_HARTMAN_CONFIG,                       // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};
#endif

// **************************************************************************************

const ITEM_INFO LastEE = {                      // LastItem
      0,
      0,                                        // Type definition
      0,                                        // Size, number of Digits
      0,                                        // Number of Decimals
      0,                                        // Minimum Value
      0,                                        // Maximum Value
      0,                                        // Text number
      0,                                        // Default Value
      WRITE                                     // Attribute
};


// Niveau default tabel --------------------------------------------------------
const ITEM_INFO *NiveauDefSet[] = {
    &AddressEE,                            // U1    01      1
    &Address2EE,                           // U1    02      2
    &MmCorNiv1EE,                          // U1    03      3
    &LanguageEE,                           // U1    04      4
    &SendTimeEE,                           // U2    05      6
    &NeutralTime1EE,                       // U2    06      8
    &NeutralTime2EE,                       // U2    07     10
    &NeutralThresholdEE,                   // U2    08     12
    &TakeOffThresholdEE,                   // U2    09     14
    &TakeOffTimeEE,                        // S2    10     16
    &MlkFlwCalcTimeEE,                     // U1    11     17
    &OutputBlockBitsEE,                    // U1    12     18
    &SepMsgCntrlEE,                        // U1    13     19
    &SepActFuncNrEE,                       // U1    14     20
    &SepCondFuncNrEE,                      // U1    15     21
    &SepMinYldFuncNrEE,                    // U1    16     22
    &SepTmpFuncNrEE,                       // U1    17     23
    &ClusterDownEE,                        // U1    18     24
    &ClusterDlyEE,                         // U2    19     26
    &VacuumDlyEE,                          // U2    22     28
    &MilkSweepDlyTimeEE,                   // U1    21     29
    &MilkSweepActTimeEE,                   // U1    22     30
    &I1ControlEE,                          // U1
    &I2ControlEE,                          // U1    23     31
    &OutputK2EE,                           // U1    24     32
    &FlowLvl2ThresholdEE,                  // U2    25     34
    &TrcArmOnTimeEE,                       // U1    26     35
    &TrcArmOffTimeEE,                      // U1    27     36
    &TAPulsFreqEE,                         // U2    28     38
    &TAPulsRatioEE,                        // U1    29     39
    &CleanStartTimeEE,                     // U2    34     47
    &CleanTimeEE,                          // U2    35     49
    &GateSwitchTimeEE,                     // U2    36     51
    &DispTimeEE,                           // U2    37     53
    &MmCorNiv2EE,                          // U1    30     40
    &OutputP1DlyEE,                        // U2    31     42
    &OutputP1TimeEE,                       // U2    32     44
    &OutputP1InvEE,                        // BL    33     45
    &GoatMilkGlassEE,                      // BL    38     54
    &MmGoatVal1EE,                         // U2    39     56
    &MmGoatVal2EE,                         // U2    40     58
    &RobotEE,                              // BL    41     59
    &LevelCounter,                         // U2    42     61        r
    &Cal1EE,                               // U2    43     58
    &Cal2EE,                               // U2    44     60
    &Cal3EE,                               // U2    45     62
    &Cal4EE,                               // U2    46     64
    &PrgVrsIC13,                           // U1    47     65        r
    &PrgVrsIC09,                           // U1    48     66        r
    &PrgVrsIC08,                           // U1    49     67        r
    &VersionRom,                           // U2    50     69        r
    &PulsationEE,                          // U1    51     70
    &PulsFrequencyEE,                      // U1    52     71
    &PulsRatioEE,                          // U1    53     72
    &StimPulsFreqEE,                       // U2    54     74
    &StimPulsRatioEE,                      // U1    55     75
    &StimTresHoldEE,                       // U2    56     77
    &StimStartTimeEE,                      // U2    57     79
    &StimMaxTimeEE,                        // U2    58     81
    &NrOfPulsatorsEE,                      // U1    59     82
    &FeedingEE,                            // U1    60     83
    &FeedSpeedEE,                          // U2    61     85
    &FeedMtrPortionEE,                     // U2    62     87
    &FeedBalanceDefEE,                     // U2    63     89
    &RelaisTimeEE,                         // U2    64     91
    &SB_FeedMtrPortionEE,                  // U2    65     93
    &SB_FeedBalanceDefEE,                  // U2    66     95
    &SB_RelaisTimeEE,                      // U2    67     97
    &TestEE,                               // U1    68     98
    &MilkMeterTypeEE,                      // U1    69     99
    &ParlourTypeEE,                        // U1    70    100
    &GateK3TimeEE,
    &GateK4TimeEE,
    &GateK5TimeEE,
    &GateK3EE,                             // U1    71    101
    &GateK4EE,                             // U1    72    102
    &GateK5EE,                             // U1    73    103
    &OTRowMasterEE,                        // U1    74    104
    &ServiceStandsEE,                      // U1    75    105
    &RowSizeEE,                            // U1    76    106
    &ConMeasuringEE,                       // U1    77    107
    &ExtConMeasuringEE,                    // U1    78    108
    &TempMeasuringEE,                      // BL    79    109
    &TempRValueEE,                         // U2    80    110
    &KeySoft,                              // U1    81    112        r
    &VersionAppl,                          // U2    82    114        r
#if _HARTMAN_
    &HartmanEE,                            // U1
#endif
    &DefaultEE,                            // U1    83    115

    };

// memolac default tabel -------------------------------------------------------
const ITEM_INFO *MemolacDefSet[] = {
    &AddressEE,                            // U1    01      1
    &Address2EE,                           // U1    02      2
    &MmCorMemEE,                           // U1    03      3
    &ValveTimeEE,                          // U2    04      5
    &FillTimeEE,                           // S1    05      6
    &LanguageEE,                           // U1    06      7
    &SendTimeEE,                           // U2    07      9
    &NeutralTime1EE,                       // U2    08     11
    &NeutralTime2EE,                       // U2    09     13
    &NeutralThresholdEE,                   // U2    10     15
    &TakeOffThresholdEE,                   // U2    11     17
    &TakeOffTimeEE,                        // S2    12     19
    &MlkFlwCalcTimeEE,                     // U1    13     20
    &OutputBlockBitsEE,                    // U1    14     21
    &SepMsgCntrlEE,                        // U1    15     22
    &SepActFuncNrEE,                       // U1    16     23
    &SepCondFuncNrEE,                      // U1    17     24
    &SepMinYldFuncNrEE,                    // U1    16     22
    &SepTmpFuncNrEE,                       // U1    17     23
    &ClusterDownEE,                        // U1    18     25
    &ClusterDownDelayHighEE,
    &ClusterDlyEE,                         // U2    19     27
    &VacuumDlyEE,                          // U2    20     29
    &MilkSweepDlyTimeEE,                   // U1    21     30
    &MilkSweepActTimeEE,                   // U1    22     31
    &I1ControlEE,                          // U1
    &I2ControlEE,                          // U1    23     32
    &OutputK2EE,                           // U1    24     33
    &FlowLvl2ThresholdEE,                  // U2    25     35
    &TrcArmOnTimeEE,                       // U1    26     36
    &TrcArmOffTimeEE,                      // U1    27     37
    &TAPulsFreqEE,                         // U2           57     72
    &TAPulsRatioEE,                        // U1          58     73
    &CleanStartTimeEE,                     // U2    28     39
    &GateSwitchTimeEE,                     // U2    29     41
#if _HARTMAN_
    &OutputP1DlyEE,                        // U2    31     42
//    &OutputP1TimeEE,                       // U2    32     44
    &OutputP1InvEE,                        // BL    33     45
#endif
    &DispTimeEE,                           // U2    30     43
    &MmSensorMemEE,                        // U1    31     44
    &CorFlow0EE,                           // U1    32     45
    &CorFlow1EE,                           // U1    33     46
    &CorFlow2EE,                           // U1    34     47
    &CorFlow3EE,                           // U1    35     48
    &CorFlow4EE,                           // U1    36     49
    &CorFlow5EE,                           // U1    37     50
    &CorFlow6EE,                           // U1    38     51
    &CorFlow7EE,                           // U1    39     52
    &CorFlow8EE,                           // U1    40     53
    &CorFlow9EE,                           // U1    41     54
    &CorFlow10EE,                          // U1    42     55
    &CorFlow11EE,                          // U1    43     56
    &CorFlow12EE,                          // U1    44     57
    &CorFlow13EE,                          // U1    45     58
    &PrgVrsIC13,                           // U1    46     59        r
    &PrgVrsIC09,                           // U1    47     60        r
    &PrgVrsIC08,                           // U1    48     61        r
    &VersionRom,                           // U2    49     63        r
    &ClnPhs1EE,                            // U1    50     64
    &ClnPhs2EE,                            // U1    51     65
    &ClnPhs4EE,                            // U1    52     66     voorheen ClnPhs3EE
    &EmptyTimeEE,                          // S1    53     67
    &PulsationEE,                          // U1    54     68
    &PulsFrequencyEE,                      // U1    55     69
    &PulsRatioEE,                          // U1    56     70
    &StimPulsFreqEE,                       // U2    57     72
    &StimPulsRatioEE,                      // U1    58     73
    &StimTresHoldEE,                       // U2    59     75
    &StimStartTimeEE,                      // U2    60     77
    &StimMaxTimeEE,                        // U2    61     79
    &NrOfPulsatorsEE,                      // U1    62     80
    &FeedingEE,                            // U1    63     81
    &FeedSpeedEE,                          // U2    64     83
    &FeedMtrPortionEE,                     // U2    65     85
    &FeedBalanceDefEE,                     // U2    66     87
    &RelaisTimeEE,                         // U2    67     89
    &SB_FeedMtrPortionEE,                  // U2    68     91
    &SB_FeedBalanceDefEE,                  // U2    69     93
    &SB_RelaisTimeEE,                      // U2    70     95
    &TestEE,                               // U1    71     96
    &MilkMeterTypeEE,                      // U1    72     97
    &DetectLevelEE,                        // U1    73
    &ParlourTypeEE,                        // U1    74     98
    &GateK3TimeEE,                         //
    &GateK4TimeEE,
    &GateK5TimeEE,
    &GateK3EE,                             // U1    74     99
    &GateK4EE,                             // U1    75    100
    &GateK5EE,                             // U1    76    101
    &OTRowMasterEE,                        // U1    77    102
    &ServiceStandsEE,                      // U1    78    103
    &RowSizeEE,                            // U1    79    104
    &ConMeasuringEE,                       // U1    80    105
    &ExtConMeasuringEE,                    // U1    81    106
    &TempMeasuringEE,                      // BL    82    107
    &TempRValueEE,                         // U2    83    109
    &KeySoft,                              // U1    84    110        r
    &VersionAppl,                          // U2    85    112        r
#if _HARTMAN_
    &HartmanEE,                            // U1
#endif
    &DefaultEE,                            // U1    86    113
    0
    };


// vflow default tabel -------------------------------------------------------
const ITEM_INFO *VFlowDefSet[] = {
    &AddressEE,                            // U1    01      1
    &Address2EE,                           // U1    02      2
//    &MmCorMemEE,                           // U1    03      3
//    &ValveTimeEE,                          // U2    04      5
//    &FillTimeEE,                           // S1    05      6
    &LanguageEE,                           // U1    06      7
    &SendTimeEE,                           // U2    07      9
    &NeutralTime1EE,                       // U2    08     11
    &NeutralTime2EE,                       // U2    09     13
    &NeutralThresholdEE,                   // U2    10     15
    &TakeOffThresholdEE,                   // U2    11     17
    &TakeOffTimeEE,                        // S2    12     19
    &MlkFlwCalcTimeEE,                     // U1    13     20
    &OutputBlockBitsEE,                    // U1    14     21
    &SepMsgCntrlEE,                        // U1    15     22
    &SepActFuncNrEE,                       // U1    16     23
    &SepCondFuncNrEE,                      // U1    17     24
    &SepMinYldFuncNrEE,                    // U1    16     22
    &SepTmpFuncNrEE,                       // U1    17     23
    &ClusterDownEE,                        // U1    18     25
    &ClusterDownDelayHighEE,
    &ClusterDlyEE,                         // U2    19     27
    &VacuumDlyEE,                          // U2    20     29
    &MilkSweepDlyTimeEE,                   // U1    21     30
    &MilkSweepActTimeEE,                   // U1    22     31
    &I1ControlEE,                          // U1
    &I2ControlEE,                          // U1    23     32
    &OutputK2EE,                           // U1    24     33
    &FlowLvl2ThresholdEE,                  // U2    25     35
    &TrcArmOnTimeEE,                       // U1    26     36
    &TrcArmOffTimeEE,                      // U1    27     37
    &TAPulsFreqEE,                         // U2           57     72
    &TAPulsRatioEE,                        // U1          58     73
    &CleanStartTimeEE,                     // U2    28     39
    &GateSwitchTimeEE,                     // U2    29     41
//#if _HARTMAN_
    &OutputP1DlyEE,                        // U2    31     42
    &OutputP1InvEE,                        // BL    33     45
//#endif
    &DispTimeEE,                           // U2    30     43
//    &MmSensorMemEE,                        // U1    31     44
    &CorVFlowEE,                           // U1    32     45
//    &CorFlow1EE,                           // U1    33     46
//    &CorFlow2EE,                           // U1    34     47
//    &CorFlow3EE,                           // U1    35     48
//    &CorFlow4EE,                           // U1    36     49
//    &CorFlow5EE,                           // U1    37     50
//    &CorFlow6EE,                           // U1    38     51
//    &CorFlow7EE,                           // U1    39     52
//    &CorFlow8EE,                           // U1    40     53
//    &CorFlow9EE,                           // U1    41     54
//    &CorFlow10EE,                          // U1    42     55
//    &CorFlow11EE,                          // U1    43     56
//    &CorFlow12EE,                          // U1    44     57
//    &CorFlow13EE,                          // U1    45     58
    &PrgVrsIC13,                           // U1    46     59        r
    &PrgVrsIC09,                           // U1    47     60        r
    &PrgVrsIC08,                           // U1    48     61        r
    &VersionRom,                           // U2    49     63        r
    &ClnPhs1EE,                            // U1    50     64
    &ClnPhs2EE,                            // U1    51     65
    &ClnPhs4EE,                            // U1    52     66     voorheen ClnPhs3EE
//    &EmptyTimeEE,                          // S1    53     67
    &PulsationEE,                          // U1    54     68
    &PulsFrequencyEE,                      // U1    55     69
    &PulsRatioEE,                          // U1    56     70
    &StimPulsFreqEE,                       // U2    57     72
    &StimPulsRatioEE,                      // U1    58     73
    &StimTresHoldEE,                       // U2    59     75
    &StimStartTimeEE,                      // U2    60     77
    &StimMaxTimeEE,                        // U2    61     79
    &NrOfPulsatorsEE,                      // U1    62     80
    &FeedingEE,                            // U1    63     81
    &FeedSpeedEE,                          // U2    64     83
    &FeedMtrPortionEE,                     // U2    65     85
    &FeedBalanceDefEE,                     // U2    66     87
    &RelaisTimeEE,                         // U2    67     89
    &SB_FeedMtrPortionEE,                  // U2    68     91
    &SB_FeedBalanceDefEE,                  // U2    69     93
    &SB_RelaisTimeEE,                      // U2    70     95
    &TestEE,                               // U1    71     96
    &MilkMeterTypeEE,                      // U1    72     97
//    &DetectLevelEE,                        // U1    73
    &ParlourTypeEE,                        // U1    74     98
    &GateK3TimeEE,                         //
    &GateK4TimeEE,
    &GateK5TimeEE,
    &GateK3EE,                             // U1    74     99
    &GateK4EE,                             // U1    75    100
    &GateK5EE,                             // U1    76    101
    &OTRowMasterEE,                        // U1    77    102
    &ServiceStandsEE,                      // U1    78    103
    &RowSizeEE,                            // U1    79    104
    &ConMeasuringEE,                       // U1    80    105
//    &ExtConMeasuringEE,                    // U1    81    106
    &TempMeasuringEE,                      // BL    82    107
//    &TempRValueEE,                         // U2    83    109
    &KeySoft,                              // U1    84    110        r
    &VersionAppl,                          // U2    85    112        r
//#if _HARTMAN_
//    &HartmanEE,                            // U1
//#endif
    &DefaultEE,                            // U1    86    113
    0
    };


// ALL ITEMS
/*
AddressEE                 _U1    n    m        r
Address2EE                _U1    n    m        r
MilkMeterTypeEE           _U1    n    m
MmCorMemEE                _U1        m
MmCorNiv1EE               _U1    n
MmCorNiv2EE               _U1    n
ValveTimeEE               _U2        m
FillTimeEE                _S1        m
LanguageEE                _U1    n    m
SendTimeEE                _U2    n    m        10
NeutralTime1EE            _U2    n    m
NeutralTime2EE            _U2    n    m
NeutralThresholdEE        _U2    n    m
TakeOffThresholdEE        _U2    n    m
TakeOffTimeEE             _S2    n    m
MlkFlwCalcTimeEE          _U1    n    m
OutputBlockBitsEE         _U1    n    m
SepMsgCntrlEE             _U1    n    m
SepActFuncNrEE            _U1    n    m
SepCondFuncNrEE           _U1    n    m        20
ClusterDownEE             _U1    n    m
ClusterDlyEE              _U2    n    m
VacuumDlyEE               _U2    n    m
MilkSweepDlyTimeEE        _U1    n    m
MilkSweepActTimeEE        _U1    n    m
I2ControlEE               _U1    n    m
OutputK2EE                _U1    n    m
FlowLvl2ThresholdEE       _U2    n    m
CleanStartTimeEE          _U2    n    m
OutputP1DlyEE             _U2    n            30
OutputP1TimeEE            _U2    n
OutputP1InvEE             _BL    n
CleanTimeEE               _U2    n
GateSwitchTimeEE          _U2    n    m
DispTimeEE                _U2    n    m
MmSensorMemEE             _U1        m
CorFlow0EE                _U1        m
CorFlow1EE                _U1        m
CorFlow2EE                _U1        m
CorFlow3EE                _U1        m        40
CorFlow4EE                _U1        m
CorFlow5EE                _U1        m
CorFlow6EE                _U1         m
CorFlow7EE                _U1         m
CorFlow8EE                _U1        m
CorFlow9EE                _U1        m
CorFlow10EE               _U1        m
CorFlow11EE               _U1        m
CorFlow12EE               _U1        m
CorFlow13EE               _U1        m        50
Cal1EE                    _U2    n
Cal2EE                    _U2    n
Cal3EE                    _U2    n
Cal4EE                    _U2    n
GoatMilkGlassEE           _BL    n
MmGoatVal1EE              _U2    n
MmGoatVal2EE              _U2    n
RobotEE                   _BL    n
PrgVrsIC13                _U1    n    m    r
PrgVrsIC09                _U1    n    m    r    60
PrgVrsIC08                _U1    n    m    r
VersionRom                _U2    n    m    r
ClnPhs1EE                 _U1        m
ClnPhs2EE                 _U1        m
ClnPhs4EE                 _U1        m
EmptyTimeEE               _S1        m
PulsationEE               _U1    n    m
PulsFrequencyEE           _U1    n    m
PulsRatioEE               _U1    n    m
StimPulsFreqEE            _U2    n    m        70
StimPulsRatioEE           _U1    n    m
StimTresHoldEE            _U2    n    m
StimStartTimeEE           _U2    n    m
StimMaxTimeEE             _U2    n    m
NrOfPulsatorsEE           _U1    n    m
FeedingEE                 _U1    n    m
FeedSpeedEE               _U2    n    m
FeedMtrPortionEE          _U2    n    m
FeedBalanceDefEE          _U2    n    m
RelaisTimeEE              _U2    n    m        80
SB_FeedMtrPortionEE       _U2    n    m
SB_FeedBalanceDefEE       _U2    n    m
SB_RelaisTimeEE           _U2    n    m
TestEE                    _U1    n    m
ParlourTypeEE             _U1    n    m
GateK3EE                  _U1    n    m
GateK4EE                  _U1    n    m
GateK5EE                  _U1    n    m
OTRowMasterEE             _U1    n    m
ServiceStandsEE           _U1    n    m        90
RowSizeEE                 _U1    n    m
ConMeasuringEE            _U1    n    m
ExtConMeasuringEE         _U1    n    m
TempMeasuringEE           _BL    n    m
TempRValueEE              _U2    n    m
KeySoft                   _U1    n    m    r
VersionAppl               _U2    n    m    r
// DefaultEE              _U1                99
                                110 110 133
*/
