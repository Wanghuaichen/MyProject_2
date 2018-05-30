#ifndef ITEM_INCLUDED
#define ITEM_INCLUDED 1


#define   READ_ONLY             0
#define   WRITE                 1


typedef struct {
  unsigned char *PEEAddress;    //  Pointer to EEProm Address
  unsigned char  Type;          // Type, U1K, U1 etc.
  unsigned char  Size;          // Size,number of digits
  unsigned char  NrDec;         // Number of Decimals
  unsigned short MinVal;        // Minimum Value
  unsigned short MaxVal;        // Maximum Value
  unsigned short TextNr;        // Text Number
  unsigned short Default;       // Default Value
  unsigned char  Attrib;        // Attribute
  unsigned char  Pos;           // Position
  } ITEM_INFO;


extern const  ITEM_INFO PulsationEE;
extern const  ITEM_INFO NrOfPulsatorsEE;
extern const  ITEM_INFO PulsRatioEE;
extern const  ITEM_INFO FeedingEE;
extern const  ITEM_INFO FlowLvl2ThresholdEE;
extern const  ITEM_INFO ParlourTypeEE;
extern const  ITEM_INFO OutputBlockBitsEE;
extern const  ITEM_INFO CentralGateEE;
extern const  ITEM_INFO EntranceGateEE;
extern const  ITEM_INFO ExitGateEE;
extern const  ITEM_INFO LanguageEE;
extern const  ITEM_INFO GateK5EE;

extern const  ITEM_INFO PrgVrsIC13;                        // KeyBoard
extern const  ITEM_INFO PrgVrsIC09;                        // Conductivity/Temperature
extern const  ITEM_INFO PrgVrsIC08;                        // Communicatie


extern const  ITEM_INFO VersionRom;
extern const  ITEM_INFO VersionAppl;
extern const  ITEM_INFO AddressEE;
extern const  ITEM_INFO Address2EE;
extern const  ITEM_INFO CleanStartTimeEE;
extern const  ITEM_INFO OutputK2EE;
extern const  ITEM_INFO MmCorNiv1EE;
extern const  ITEM_INFO DefaultEE;
extern const  ITEM_INFO ComProtEE;
extern const  ITEM_INFO MilkMeterTypeEE;
extern const  ITEM_INFO MlkFlwCalcTimeEE;
extern const  ITEM_INFO KeySoft;
extern const  ITEM_INFO AddrAlarmUnitEE;
extern const  ITEM_INFO ServiceStandsEE;
extern const  ITEM_INFO OTRowMasterEE;
extern const  ITEM_INFO MilkSweepDlyTimeEE;
extern const  ITEM_INFO MilkSweepActTimeEE;
extern const  ITEM_INFO ClusterDownEE;
extern const  ITEM_INFO ActMeasuringEE;
extern const  ITEM_INFO SerialNrEE;
extern const  ITEM_INFO FeedBalanceDefEE;
extern const  ITEM_INFO FeedMtrPortionEE;
extern const  ITEM_INFO FeedSpeedEE;
extern const  ITEM_INFO RelaisTimeEE;
extern const  ITEM_INFO SB_FeedBalanceDefEE;
extern const  ITEM_INFO SB_FeedMtrPortionEE;
extern const  ITEM_INFO RowSizeEE;
extern const  ITEM_INFO GoatMilkGlassEE;
extern const  ITEM_INFO MmCorMemEE;
extern const  ITEM_INFO SepMsgCntrlEE;
extern const  ITEM_INFO SepActFuncNrEE;
extern const  ITEM_INFO SepCondFuncNrEE;
extern const  ITEM_INFO SepMinYldFuncNrEE;
extern const  ITEM_INFO SepTmpFuncNrEE;
extern const  ITEM_INFO LastEE;
extern const  ITEM_INFO CorVFlowEE;
extern const  ITEM_INFO CorFlow1EE;
extern const  ITEM_INFO CorFlow12EE;
extern const  ITEM_INFO DispTimeEE;
extern const  ITEM_INFO ConMeasuringEE;
extern const  ITEM_INFO TempMeasuringEE;
extern const  ITEM_INFO TakeOffTimeEE;
extern const  ITEM_INFO ClnPhs1EE;

extern        ITEM_INFO  Item;
extern        ITEM_INFO *ScreenEEPtr;
#if _HARTMAN_
extern const  ITEM_INFO *NiveauDefSet[89];
extern const  ITEM_INFO *MemolacDefSet[100];           // 105
extern const  ITEM_INFO *VFlowDefSet[78];
#else
extern const  ITEM_INFO *NiveauDefSet[88];
extern const  ITEM_INFO *MemolacDefSet[97];           // 105
extern const  ITEM_INFO *VFlowDefSet[78];
#endif
extern const  ITEM_INFO SB_RelaisTimeEE;


#endif
