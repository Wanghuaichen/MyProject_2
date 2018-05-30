/****************************************************************/
/*                                                              */
/* (c) Copyright NEDAP N.V. GROENLO HOLLAND                     */
/*     ALL RIGHTS RESERVED                                      */
/*                                                              */
/*                                                              */
/*  MODULE Text String                                          */
/*                                                              */
/****************************************************************/

#include "../lib/text.h"


  /*"123456789 123456789 "*/

#pragma memory=constseg(CONSTPAGE0)


const unsigned char MsgStrings[][11] = {
    "COW TR STP",
    "ENTR LOCK ",
    "EXIT LOCK ",
    "C_ERR  IC8",
    "C_ERR  IC9",
    "C_ERR IC13",
    "COMM_ERROR",
    "K_ERR MODE",
    "K_ERR     ",
    "ENTR OPEN ",
    "EXIT OPEN ",
    "NO ENTR OP",
    "MANUAL CTL",
    "SEMI AUTO ",
    "FULL AUTO ",
    "FLL NO SNS",
    "INPUT_LOW ",
    "COMM_INIT "
};

const unsigned char SrvTxtString[][11] = {
    "ROM_V     ",         // VersionRom
    "APP_V     ",         // VersionAppl
    "IC13_V    ",         // ProgVersionIc13, KeyBoard
    "IC09_V    ",         // ProgVersionIc09, Conductivity
    "IC08_V    ",         // ProgVersionIc08, Communication
    "LANGUA    ",         // Language
    "ADDR      ",         // AddressEE
    "ADDR2     ",         // Address2EE
    "M_TYPE    ",         // MilkMeterTypeEE
    "COM_P     ",         // ComProtEE
    "SW_OFF    ",         // SwitchOffEE
    "CAL_1     ",         // Cal1EE
    "CAL_2     ",         // Cal2EE
    "CAL_3     ",         // Cal3EE
    "CAL_4     ",         // Cal4EE
    "MMC_1     ",         // MmCorNiv1EE
    "MMC_2     ",         // MmCorNiv2EE
    "MCM       ",         // MmCorMemEE
    "MCSM      ",         // MmCorSensorMemEE
    "MOM       ",         // MmOffsetManEE
    "MWTM      ",         // MmWaterTestManEE
    "DSP_TM    ",         // DispTimeEE
    "GS_TM     ",         // GateSwitchTimeEE
    "NTM_1     ",         // NeutralTime1EE
    "NTM_2     ",         // NeutralTime2EE
    "NTH       ",         // NeutralThresholdEE
    "TOTH      ",         // TakeOffThresholdEE
    "TOTM      ",         // TakeOffTimeEE
    "MFCTM     ",         // MlkFlwCalcTimeEE
    "Cl_DLY    ",         // ClusterDlyEE
    "VC_DLY    ",         // VacuumDlyEE
    "P1_DLY    ",         // OutputP1DlyEE
    "P1_TM     ",         // OutputP1TimeEE
    "P1_INV    ",         // OutputP1InvEE
    "KOFP      ",         // KickOffFlowProcEE
    "KOSTM     ",         // KickOffSampleTimeEE
    "KOFTH     ",         // KickOffFacThresholdEE
    "SND_TM    ",         // SendTimeEE
    "CLS_TM    ",         // CleanStartTimeEE
    "CLN_TM    ",         // CleanTimeEE
    "VTM_TM     ",         // ValveTimeEE  VTM_0!!
//    "VTM_1     ",         // ValveTime1EE
//    "VTM_2     ",         // ValveTime2EE
//    "VTM_3     ",         // ValveTime3EE
//    "VTM_4     ",         // ValveTime4EE
//    "VTM_5     ",         // ValveTime5EE
//    "VTM_6     ",         // ValveTime6EE
//    "VTM_7     ",         // ValveTime7EE
//    "VTM_8     ",         // ValveTime8EE
//    "VTM_9     ",         // ValveTime9EE
//    "VTM_10    ",         // ValveTime10EE
//    "VTM_11    ",         // ValveTime11EE
//    "VTM_12    ",         // ValveTime12EE
//    "VTM_13    ",         // ValveTime13EE
    "CRF_0     ",         // CorFlow0EE
    "CRF_1     ",         // CorFlow1EE
    "CRF_2     ",         // CorFlow2EE
    "CRF_3     ",         // CorFlow3EE
    "CRF_4     ",         // CorFlow4EE
    "CRF_5     ",         // CorFlow5EE
    "CRF_6     ",         // CorFlow6EE
    "CRF_7     ",         // CorFlow7EE
    "CRF_8     ",         // CorFlow8EE
    "CRF_9     ",         // CorFlow9EE
    "CRF_10    ",         // CorFlow10EE
    "CRF_11    ",         // CorFlow11EE
    "CRF_12    ",         // CorFlow12EE
    "CRF_13    ",         // CorFlow13EE
    "VTM       ",         // ValveTimeManEE
    "CFM_0     ",         // CorFlow0ManEE
    "CFM_1     ",         // CorFlow1ManEE
    "CFM_2     ",         // CorFlow2ManEE
    "CFM_3     ",         // CorFlow3ManEE
    "CFM_4     ",         // CorFlow4ManEE
    "CFM_5     ",         // CorFlow5ManEE
    "CFM_6     ",         // CorFlow6ManEE
    "CFM_7     ",         // CorFlow7ManEE
    "CFM_8     ",         // CorFlow8ManEE
    "CFM_9     ",         // CorFlow9ManEE
    "CFM_10    ",         // CorFlow10ManEE
    "CFM_11    ",         // CorFlow11ManEE
    "CFM_12    ",         // CorFlow12ManEE
    "CFM_13    ",         // CorFlow13ManEE
    "FIL_TM    ",         // FillTimeEE
    "EMPTM     ",         // EmptyTimeEE
    "CPHS_1    ",         // ClnPhs1EE
    "CPHS_2    ",         // ClnPhs2EE
    "CPHS_4    ",         // ClnPhs4EE
    "SRL_NR    ",         // SerialNrEE
    "LVLCNT    ",         // LevelCounter
    "CON_M     ",         // ConMeasuringEE
    "TMP_M     ",         // TempMeasuringEE
    "RTMP      ",         // RTempValueEE
    "ACT_M     ",         // ActMeasuringEE
    "FEED      ",         // FeedingEE
    "RLST      ",         // RelaisTimeEE
    "GOAT_G    ",         // GoatMilkGlassEE
    "MG_V1     ",         // EEMmGoatVal1
    "MG_V2     ",         // EEMmGoatVal2
    "ROBOT     ",         // RobotEE
    "PLS ON    ",         // PulsationEE
    "PFREQ     ",         // PulseFrequencyEE
    "PRAT      ",         // PulsePeriodEE
    "SPFREQ    ",         // StimulationPulseFrequencyEE
    "SPRAT     ",         // StimulationPulseRatioEE
    "STRSH     ",         // StimulationTresHoldEE
    "STSTM     ",         // StimulationStartTimeEE
    "SMXTM     ",         // StimulationMaxTimeEE
    "NRPLS     ",         // NumberOfPulsatorsEE
    "OT ON     ",         // OpenTandemEE
    "TEST      ",         // TestEE
    "DEFLT     ",         // DefaultEE
    "R35       ",         // Calibratie tbv temperatuur, R35 aansluiten
    "R40       ",         // Calibratie tbv temperatuur, R40 aansluiten
    "CLEAR     ",         // clear key
    "ENTER     ",         // enter key
    "WASH      ",
    "DECEAS    ",
    "COW       ",
    "FEED      ",
    "MILK      ",
    "SHIFT     ",
    "CENTRL    ",
    "EXIT      ",
    "ENTRAN    ",
    "STOP      ",
    "MILK V    ",
    "MANUAL    ",
    "CLUSTR    ",
    "AUTO      ",
    "CALEND    ",
    "I1_CNF    ",         // configuratie van I1
    "I2_CNF    ",         // configuratie van I2
    "REF1      ",         // Calibratie tbv temperatuur, bij R35 ( geleverd bij print)
    "REF2      ",         // Calibratie tbv temperatuur, bij R40
    "K2_CFG    ",         // OutputK2, type melk automaat
    "FL2TH     ",         // FlwLvl2Treshold, Flowlevel for activating milk automat
    "EIND TEST ",
    "PARL_T    ",         // parlour type
    "OUT_BA    ",         // configuratie output block attentie bits
    "GATEK3    ",         // adres van MPC welke de central-entrance-gate bediend.
    "GATEK4    ",         // adres van MPC welke de entrance-gate bediend.
    "GATEK5    ",         // adres van MPC welke de exit-gate bediend.
    "CORR      ",         // correction factor level meter
    "15 KG     ",         //            15 kilogram
    " 5 KG     ",         //             5 kilogram
    " 0 KG     ",         //             0 kilogram
    "CAL 1     ",         // calibration value1
    "CAL 2     ",         // calibration value1
    "CAL_READY ",         // calibration ready
    "SOFT_K    ",
    "ROW_SZ    ",         // RowSizeEE: aantal standen waar het bericht naar toe gestuurd wordt.
    "OT_RMR    ",         // open tandem row-master
    "SWPDLY    ",         // milk-sweep delay time
    "SWPACT    ",         // milk-sweep active time
    "SRV_NR    ",         // aantal standen tbv sturen service-items, 0 is function disable
    "CM_ERR    ",
    "EX_CON    ",         // extern geleidbaarheids module
    "CLTR_D    ",          // ClusterDownEE, tbv reinigen of overzicht in de stal
    "F_SPD     ",         // FeedSpeedEE, voersnelheid
    "F_PRT     ",         // FeedMtrPortionEE, portie grootte
    "F_BAL     ",         // FeedBalanceDefEE, default voertegoed
    "FBPRT     ",         // tbv swing-over stand-alone
    "FBBAL     ",
    "BRLST     ",      //
    "SRVMD     ",         // geen eeprom, toegang to service mode
    "SEPMSG    ",         // automatische separatie berichten
    "SEPACT    ",
    "SEPCON    ",
    "SEPYLD    ",
    "SEPTMP    ",
    "PT_I1     ",
    "PT_I2     ",
    "PT_I3     ",
    "PT_I4     ",
    "PT_P1     ",
    "PT_P2     ",
    "PT_P3     ",
    "PT_LP     ",
    "PT_K1     ",
    "PT_K2     ",
    "PT_K3     ",
    "PT_K4     ",
    "PT_K5     ",
    "PT_M1     ",
    "PT_M2     ",
    "PT_M3     ",
    "PT_MTR    ",
    "TA_ON     ",
    "TA_OFF    ",
    "TA_FRQ    ",
    "TA_RAT    ",
    "K3_PAT    ",
    "K4_PAT    ",
    "K5_PAT    ",
    "D_LVL     ",
    "CLTR_H    "
#if _HARTMAN_
   ,"HART      "
#endif
#if ( _HP_TEST_ )
   ,"   BRNIN  ",
    "   ROM_T  ",
    "   RAM_T  ",
    "   M-K_T  ",
    "   I-K_T  ",
    "   I-P_T  ",
    "   MTR_T  ",
    "   C-T_T  ",
    "   BRN    "
#endif
};

const unsigned char TextString[][56][11] = {
  {
  // 01234567890           // ENGLISH  *****************************************
    "TMP       ",          // Conductivity-data, temperature
    "TMPT      ",          //             test temperature
    "RF        ",          // conductivity, right front  ( percentage)
    "RR        ",          //               right rear
    "LR        ",          //               left rear
    "LF        ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "A RF      ",          //               actual right front
    "A RR      ",          //               actual right rear
    "A LR      ",          //               actual left rear
    "A LF      ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "I_RF      ",          // Illness code right front (91)
    "I_RR      ",          // Illness code right rear  (92)
    "I_LR      ",          // Illness code left rear   (93)
    "I_LF      ",          // Illness code left front  (94)
    "ILC       ",          // Illness code
    "ILT       ",          // Illness treatment code
    "ACTFDM    ",          // Feed-data, actual feed to be fed in milk-parlour
    "FDMILK    ",          //            feed in milk parlour ( programmed)
    "FD TOT    ",          //            feed total ( 5 feedsorts)
    "FD BAL    ",          //            feed balance, total feed at this moment
    "FD CAL    ",          //            feed calibration
    "MILKT     ",          //            milk test
    "SEPMLK    ",          //            separation milk
    "MLKSP     ",          //            milk speed
    "MAXSP     ",          //            maximum speed
    "AVRSP     ",          //            average speed
    "TIME      ",          //            milk time
    "COLOST    ",          //            colostrum
    "COW       ",           //            cow
    "ACT       ",          //            activity
    "RESP      ",          //            responder number
    "GROUP     ",          //            group
    "COWSEP    ",          //            number of separations ( sep. function 99)
    "LACT      ",          // Calendar-info, days in lactation )
    "HEAT      ",          //            number of days since last heat
    "INSEM     ",          //            number of days since last insemination
    "INCALF    ",          //            incalf
    "DRY       ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "ILLATT    ",
    "CALIBRATE ",          // calibration
    "COMM FAIL ",          // communication failure
    "Y         ",          // Yes
    "N         ",          // No
    "MTR ERR   ",          // motor error (feed motor)
    "OUT ERR   ",          // output error ( no voltage on outputs)
    "A_CRV     ",          // actual rolling average
    "ARV_RF    ",          // actual rolling average
    "ARV_RR    ",          // actual rolling average
    "ARV_LR    ",          // actual rolling average
    "ARV_LF    ",          // actual rolling average
    "MAN REL   ",          // manual release milking
    "TMP_ATT   "
  },
  {
    //   01234567890       // NEDERLANDS ***************************************
    "TMP       ",          // Conductivity-data
    "TMPT      ",
    "RV        ",
    "RA        ",
    "LA        ",
    "LV        ",
    "CON       ",          //               conductivity, mengmelk
    "A RV      ",
    "A RA      ",
    "A LA      ",
    "A LV      ",
    "A CON     ",          //               actual, mengmelk
    "Z_RV      ",
    "Z_RA      ",
    "Z_LA      ",
    "Z_LV      ",
    "ZC-       ",
    "ZB-       ",
    "ACTVRM    ",          // Feed-data
    "VRMELK    ",
    "VR TOT    ",
    "VR TEG    ",
    "VR CAL    ",
    "MELKT     ",
    "SEPMLK    ",
    "MLKSN     ",
    "MAXSN     ",
    "GEMSN     ",
    "TYD       ",
    "BIEST     ",
    "KOE       ",
    "ACT       ",
    "RESP      ",
    "GROEP     ",
    "KOESEP    ",
    "LACT      ",          // Calendar-info
    "TOCHT     ",
    "INSEM     ",
    "DRACHT    ",
    "DROOG     ",
    "CON ATT   ",
    "CON PATT  ",
    "Z ATT     ",
    "CALIBRATIE",
    "COMM FOUT ",
    "J         ",
    "N         ",
    "MTR FOUT  ",
    "UITG FOUT ",
    "A_CVG     ",          // actual rolling average
    "AVG_RV    ",          // actual rolling average
    "AVG_RA    ",          // actual rolling average
    "AVG_LA    ",          // actual rolling average
    "AVG_LV    ",          // actual rolling average
    "MAN REL   ",          // manual release milking
    "A_TMP     "
  },
  {          // GERMAN  ******************************************
    "TEMP      ",          // Conductivity-data, temperature
    "TEMP-T    ",          //             test temperature
    "RV        ",          // conductivity, right front  ( percentage)
    "RH        ",          //               right rear
    "LH        ",          //               left rear
    "LV        ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "A RV      ",          //               actual right front
    "A RH      ",          //               actual right rear
    "A LH      ",          //               actual left rear
    "A LV      ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "K_RV      ",          // Illness code right front (91)
    "K_RH      ",          // Illness code right rear  (92)
    "K_LH      ",          // Illness code left rear   (93)
    "K_LV      ",          // Illness code left front  (94)
    "KRK       ",          // Illness code
    "KRB       ",          // Illness treatment code
    "AKTFRM    ",          // Feed-data, actual feed to be fed in milk-parlour
    "FRMLCH    ",          //            feed in milk parlour ( programmed)
    "FR TOT    ",          //            feed total ( 5 feedsorts)
    "FR RES    ",          //            feed balance, total feed at this moment
    "FR CAL    ",          //            feed balance, total feed at this moment
    "MILCHT    ",          //            milk test
    "SEPMLH    ",          //            separation milk
    "MILFLB    ",          //            milk speed
    "MAXFLB    ",          //            maximum speed
    "MTLFLB    ",          //            average speed
    "DAUER     ",          //            milk time
    "BIEST     ",          //            colostrum
    "KUH       ",          //            cow
    "ACT       ",          //            activity
    "RESP      ",          //            responder number
    "GRUP    ",          //            group
    "KUHSEP    ",          //            number of separations
    "LAKT      ",          // Calendar-info, days in lactation )
    "BRUNST    ",          //            number of days since last heat
    "BESAMT    ",          //            number of days since last insemination
    "TRACHT    ",          //            incalf
    "TROCK     ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "KRK ATT   ",
    "KALIBRIER ",          // calibration
    "COMM FHLR ",          // communication failure
    "J         ",          // Yes
    "N         ",          // No
    "MOTOR FHLR",          // motor error (feed motor)
    "OUSG FHLR ",          // output error ( no voltage on outputs)
    "A_CVM     ",          // actual rolling average
    "AVM_RV    ",          // actual rolling average
    "AVM_RH    ",          // actual rolling average
    "AVM_LH    ",          // actual rolling average
    "AVM_LV    ",          // actual rolling average
    "HANDAUS   ",          // manual release milking
    "TEMP ATT  "
  },
  {                        // FRENCH  ******************************************
    "TEMP      ",          // Conductivity-data, temperature
    "TEMP-T    ",          //             test temperature
    "VD        ",          // conductivity, right front  ( percentage)
    "RD        ",          //               right rear
    "RG        ",          //               left rear
    "VG        ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "AVD       ",          //               actual right front
    "ARD       ",          //               actual right rear
    "ARG       ",          //               actual left rear
    "AVG       ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "M_VD      ",          // Illness code right front (91)
    "M_RD      ",          // Illness code right rear  (92)
    "M_RG      ",          // Illness code left rear   (93)
    "M_VG      ",          // Illness code left front  (94)
    "MAL       ",          // Illness code
    "TRT       ",          // Illness treatment code
    "ACTALL    ",          // Feed-data, actual feed to be fed in milk-parlour
    "ALIML     ",          //            feed in milk parlour ( programmed)
    "AL TOT    ",          //            feed total ( 5 feedsorts)
    "AL RST    ",          //            feed balance, total feed at this moment
    "AL CAL    ",          //            feed calibration
    "LAITT     ",          //            milk test
    "SPLAIT    ",          //            separation milk
    "DBACT     ",          //            milk speed
    "DBMAX     ",          //            maximum speed
    "DBMOY     ",          //            average speed
    "TEMPS     ",          //            milk time
    "COLOST    ",          //            colostrum
    "VACHE     ",          //            cow
    "ACT       ",          //            activity
    "REP       ",          //            responder number
    "GROUPE     ",          //            group
    "VACSEP    ",          //            number of separations ( sep. function 99)
    "LACT      ",          // Calendar-info, days in lactation )
    "CHALR     ",          //            number of days since last heat
    "INSEM     ",          //            number of days since last insemination
    "VELAGE    ",          //            incalf
    "TARIE     ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "MAL ATT   ",
    "PARAMETER ",          // calibration
    "COMM ERR  ",          // communication failure
    "Q         ",          // Yes
    "N         ",          // No
    "MOTEUR ERR",          // motor error (feed motor)
    "OUTP ERR  ",          // output error ( no voltage on outputs)
    "A_CMM     ",          // actual rolling average
    "AMM_VD    ",          // actual rolling average
    "AMM_RD    ",          // actual rolling average
    "AMM_RG    ",          // actual rolling average
    "AMM_VG    ",          // actual rolling average
    "DEB MAN   ",          // manual release milking
    "TEMP ATT  "
  },
  {                        // DANISH  ******************************************
    "TEMP      ",          // Conductivity-data, temperature
    "TEMP-T    ",          //             test temperature
    "HF        ",          // conductivity, right front  ( percentage)
    "HB        ",          //               right rear
    "VB        ",          //               left rear
    "VF        ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "A HF      ",          //               actual right front
    "A HB      ",          //               actual right rear
    "A VB      ",          //               actual left rear
    "A VF      ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "S_HF      ",          // Illness code right front (91)
    "S_HB      ",          // Illness code right rear  (92)
    "S_VF      ",          // Illness code left rear   (93)
    "S_VF      ",          // Illness code left front  (94)
    "SYG       ",          // Illness code
    "BEH       ",          // Illness treatment code
    "FODER     ",          // Feed-data, actual feed to be fed in milk-parlour
    "FD MLK    ",          //            feed in milk parlour ( programmed)
    "FD TOT    ",          //            feed total ( 5 feedsorts)
    "FD BAL    ",          //            feed balance, total feed at this moment
    "FD CAL    ",          //            feed calibration
//    "YDELSE    ",          // Milk-data, milk yield
    "FORV-Y    ",          //            milk test
    "SEPMLK    ",          //            separation milk
    "FLOW      ",          //            milk speed
    "MAX-FL    ",          //            maximum speed
    "GNS-FL    ",          //            average speed
    "TID       ",          //            milk time
    "R-MLK     ",          //            colostrum
    "KO        ",          // Cow-data,  cow number
    "ACT       ",          //            activity
    "RESP      ",          //            responder number
    "GRUPPE    ",          //            group
    "KO SEP    ",          //            number of separations ( sep. function 99)
    "LAKT      ",          // Calendar-info, days in lactation )
    "BRUNST    ",          //            number of days since last heat
    "INSEM     ",          //            number of days since last insemination
    "DRGTIG    ",          //            incalf
    "GOLD      ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "SYG ATT   ",
    "KALIBRER  ",          // calibration
    "COMM FEJL ",          // communication failure
    "J         ",          // Yes
    "N         ",          // No
    "MOTOR FEJL",          // motor error (feed motor)
    "OUTP FEJL ",          // output error ( no voltage on outputs)
    "A_CRG     ",          // actual rolling average
    "ARG_HF    ",          // actual rolling average
    "ARG_HB    ",          // actual rolling average
    "ARG_VB    ",          // actual rolling average
    "ARG_VF    ",          // actual rolling average
    "MAN UDL   ",          // manual release milking
    "TEMP ATT  "
  },
  {                        // FINNISH  ******************************************
    "LAMPO     ",          // Conductivity-data, temperature
    "VRTLAM    ",          //             test temperature
    "EOPR      ",          // conductivity, right front  ( percentage)
    "TOPR      ",          //               right rear
    "TVPR      ",          //               left rear
    "EVPR      ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "EOARVO    ",          //               actual right front
    "TOARVO    ",          //               actual right rear
    "TVARVO    ",          //               actual left rear
    "EVARVO    ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "EOSK      ",          // Illness code right front (91)
    "TOSK      ",          // Illness code right rear  (92)
    "TVSK      ",          // Illness code left rear   (93)
    "EVSK      ",          // Illness code left front  (94)
    "SAIR      ",          // Illness code
    "HOIT      ",          // Illness treatment code
    "R-ANNL    ",          // Feed-data, actual feed to be fed in milk-parlour
    "R-LYPS    ",          //            feed in milk parlour ( programmed)
    "R-24T     ",          //            feed total ( 5 feedsorts)
    "R-EIS     ",          //            feed balance, total feed at this moment
    "R-CAL     ",          //            feed calibration
//    "MAITOA    ",          // Milk-data, milk yield
    "VRTMTO    ",          //            milk test
    "ERIL-L    ",          //            separation milk
    "L-NOP     ",          //            milk speed
    "L-N-MX    ",          //            maximum speed
    "L-N-KA    ",          //            average speed
    "L-AIKA    ",          //            milk time
    "TERNIM    ",          //            colostrum
    "LEHMA     ",          // Cow-data,  cow number
    "AKTPOI    ",          //            activity
    "TNSTN     ",          //            responder number
    "RYHMA     ",          //            group
    "LEHERO    ",          //            number of separations ( sep. function 99)
    "VPOIK     ",          // Calendar-info, days in lactation )
    "VKIIMA    ",          //            number of days since last heat
    "VSIEM     ",          //            number of days since last insemination
    "TIINE     ",          //            incalf
    "UMME      ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "SAIRATT   ",
    "KALBROINTI",          // calibration
    "YHT-VIRHE ",          // communication failure
    "K         ",          // Yes
    "E         ",          // No
    "MOTORVIRHE",          // motor error (feed motor)
    "LAHTOVIRHE",          // output error ( no voltage on outputs)
    "CON LK    ",          // actual rolling average
    "AEO_LK    ",          // actual rolling average
    "ATO_LK    ",          // actual rolling average
    "ATV_LK    ",          // actual rolling average
    "AEV_LK    ",          // actual rolling average
    "KASIVA    ",          // manual release milking
    "TEMP ATT  "
  },
  {                        // SWEDISH  ******************************************
    "TEMP      ",          // Conductivity-data, temperature
    "TEMP-T    ",          //             test temperature
    "HF        ",          // conductivity, right front  ( percentage)
    "HB        ",          //               right rear
    "VB        ",          //               left rear
    "VF        ",          //               left front
    "CON       ",          //               conductivity, mengmelk
    "A HF      ",          //               actual right front
    "A HB      ",          //               actual right rear
    "A VB      ",          //               actual left rear
    "A VF      ",          //               actual left front
    "A CON     ",          //               actual, mengmelk
    "S_HF      ",          // Illness code right front (91)
    "S_HB      ",          // Illness code right rear  (92)
    "S_VF      ",          // Illness code left rear   (93)
    "S_VF      ",          // Illness code left front  (94)
    "SJUG      ",          // Illness code
    "SJBE      ",          // Illness treatment code
    "FODER     ",          // Feed-data, actual feed to be fed in milk-parlour
    "FD MJG    ",          //            feed in milk parlour ( programmed)
    "FD TOT    ",          //            feed total ( 5 feedsorts)
    "FD BAL    ",          //            feed balance, total feed at this moment
    "FD CAL    ",          //            feed balance, total feed at this moment
//    "MJOLK     ",          // Milk-data, milk yield
    "MJTEST    ",          //            milk test
    "SEPMLK    ",          //            separation milk
    "FLODE     ",          //            milk speed
    "MAX-FL    ",          //            maximum speed
    "GSHAST    ",          //            average speed
    "TID       ",          //            milk time
    "RMJOLK    ",          //            colostrum
    "KO        ",          // Cow-data,  cow number
    "ACTAVK    ",          //            activity
    "RESP      ",          //            responder number
    "GRUPP     ",          //            group
    "KOSEP     ",          //            number of separations ( sep. function 99)
    "LACT      ",          // Calendar-info, days in lactation )
    "BRUNST    ",          //            number of days since last heat
    "INSEM     ",          //            number of days since last insemination
    "DRAK      ",          //            incalf
    "SINT      ",          //            number of days dry
    "CON ATT   ",
    "CON PATT  ",
    "SJUGATT   ",
    "KALIBRERA ",          // calibration
    "KOMMFEL   ",          // communication failure
    "J         ",          // Yes
    "N         ",          // No
    "MOTORFEL  ",          // motor error (feed motor)
    "UTGANG FEL",          // output error ( no voltage on outputs)
    "A_CRG     ",          // actual rolling average
    "ARG_HF    ",          // actual rolling average
    "ARG_HB    ",          // actual rolling average
    "ARG_VB    ",          // actual rolling average
    "ARG_VF    ",          // actual rolling average
    "MAN UTL   ",          // manual release milking
    "TEMP ATT  "
  },
};


#pragma memory=default
