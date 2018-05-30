// =============================================================================
//
// (c) Copyright NEDAP N.V. GROENLO HOLLAND
//     ALL RIGHTS RESERVED
//
// =============================================================================

// include files ---------------------------------------------------------------

#include "../appl/conduct.h"
#include "../appl/feed.h"
#include "../appl/temp.h"
#include "../appl/tandem.h"
#include "../appl/pulsation.h"
#include "../lib/watchdog.h"
#include "../lib/time.h"
#include "flowctrl.h"
#include "config.h"
#include "flowrcv.h"

#include "../include/romver.h"
#include "../include/cstartup.h"
#include "../include/romeehc1.h"
#include "../include/romstnet.h"
#include "../include/romdwnld.h"

#include "../lib/rti_int.h"
#include "../lib/applver.h"
#include "../lib/eeprom.h"
#include "../lib/eehc11.h"
#include "../lib/stdlib.h"
#include "../lib/string.h"
#include "../lib/boolean.h"
#include "../lib/burnin.h"
#include "../lib/schedule.h"
#include "../lib/extport.h"
#include "../lib/iic.h"
#include "../lib/mem.h"
#include "../init/start.h"
#include "../comm/send_msg.h"
#include "../comm/startnet.h"
#include "../usx/fake_usx.h"
#include "../flowlib/per_func_test_vcp_flow.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "../flowlib/periph_vcp_flow.h"
#include "../flowlib/system_vcp_flow.h"
#include "../flowlib/transfer_vcp_flow.h"
#include "../flowlib/separation_vcp_flow.h"
#include "../flowlib/milk_vcp_flow.h"
#include "../flowlib/activity_vcp_flow.h"
#include "../flowlib/mat_robot_vcp_flow.h"
#include "../flowlib/cond_temp_vcp_flow.h"
#include "../flowlib/disease_weight_vcp_flow.h"
#include "../flowlib/test_mpc1_flow.h"
#include "../flowlib/rom_test_vcp_flow.h"
#include "../flowlib/cow_herd_vcp_flow.h"
#include "../appl/milk.h"
#include "../appl/milkmisc.h"
#include "../key/disp.h"
#include "../key/key.h"
#include "../net/ext_net.h"

#pragma codeseg(EEPAGE)

unsigned short DL_Status;



// =============================================================================
void HndlFTBurnInRes(void)
{
  unsigned char Buffer[sizeof(short) + sizeof(PerFuncTest)];

  PerFuncTest *PerFuncTestPtr = ( PerFuncTest *)(Buffer +2);


  *((unsigned short *)Buffer) = PerFuncTestId;
  PerFuncTestPtr->PerFuncTestCommand = FTBurnInResId;

  StopScheduler();

  PerFuncTestPtr->Array[0].PerFuncTestDataHigh = TestResult.Ready;
  PerFuncTestPtr->Array[0].PerFuncTestDataLow  = TestResult.RomResult;
  PerFuncTestPtr->Array[1].PerFuncTestDataHigh = TestResult.RamResult;
  PerFuncTestPtr->Array[1].PerFuncTestDataLow  = TestResult.VC3Result;
  PerFuncTestPtr->Array[2].PerFuncTestDataHigh = TestResult.PMResult;
  PerFuncTestPtr->Array[2].PerFuncTestDataLow  = TestResult.RelaisResult;
  PerFuncTestPtr->Array[3].PerFuncTestDataHigh = TestResult.UartResult;
  PerFuncTestPtr->Array[3].PerFuncTestDataLow  = TestResult.DummyResult;

  ContScheduler();

  //Controleer tijdens burnin of er geen problemen met het netwerk zijn,
  //zo ja, initialiseer het netwerk opnieuw
  if (SendMessage( ConLow, EEAddress, 0, sizeof(Buffer), Buffer ) !=0) {
    NetClose(CHANNEL);
    InitStartNet();
  }
}

// =============================================================================
void HndlSysGenAbsTimeBroadcast(NETIO_MAIL *MailPtr)
{
  SysGenAbsTimeBroadcast *SysGenAbsTimePtr = ( SysGenAbsTimeBroadcast *)( &( MailPtr->message_start));
  StopScheduler();
  AbsTime = SysGenAbsTimePtr->SysGenAbsTime;
  AbsDate = AbsTime/86400;
  ContScheduler();
}

// =============================================================================
// PerGenNumberReference Request
// =============================================================================
unsigned char HndlPerGenNumberRefReq(signed short PerAddress)
{
  unsigned char Buffer[sizeof(unsigned short)];

  *((unsigned short *)Buffer) = PerGenNumberRefReqId;
  return SendMessage( ConLow, PerAddress, 0, sizeof(Buffer), Buffer );
}


// =============================================================================
void HndlCowActMemAttRef(NETIO_MAIL *MailPtr)
{
  unsigned char Side;

  void *CowActMemAttRefPtr = ( void *)( &( MailPtr->message_start));

  if (MailPtr->message_length < (LONG_COW_GEN_NUMBER ? sizeof(ExtCowActMemAttRef): sizeof(CowActMemAttRef))) {goto exit;}  /* bericht is te klein !!! */
  Side = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  if (p_CowActMemAttRef[Side] != NULL) {free(p_CowActMemAttRef[Side]); p_CowActMemAttRef[Side] = NULL;}
  while (p_CowActMemAttRef[Side] == NULL) {p_CowActMemAttRef[Side] = calloc(1, MailPtr->message_length);}
  memcpy(p_CowActMemAttRef[Side], CowActMemAttRefPtr, MailPtr->message_length);

  ContScheduler();

exit: ;
}

// =============================================================================
// Handle CowActMemAttRefReq
// =============================================================================
unsigned char HndlCowActMemAttRefReq(unsigned char Side)
{
  unsigned char  Buffer[MESS_LEN(ExtCowActMemAttRefReq)];
  void  *p_CowActMemAttRefReq = (void *)(Buffer + 2);

  if (p_CowActMemAttRef[Side] != NULL) {free(p_CowActMemAttRef[Side]); p_CowActMemAttRef[Side] = NULL;}

  if ( LONG_COW_GEN_NUMBER ){
    *((unsigned short *)Buffer)                                    = ExtCowActMemAttRefReqId;
    (( ExtCowActMemAttRefReq *)p_CowActMemAttRefReq)->PerGenNumber = PerGenNumber[Side];
    (( ExtCowActMemAttRefReq *)p_CowActMemAttRefReq)->CowGenNumber = ((ExtMlkRef **)p_MlkRef)[Side]->CowGenNumber;
  } else {
    *((unsigned short *)Buffer)                                 = CowActMemAttRefReqId;
    (( CowActMemAttRefReq *)p_CowActMemAttRefReq)->PerGenNumber = PerGenNumber[Side];
    (( CowActMemAttRefReq *)p_CowActMemAttRefReq)->CowGenNumber = ((MlkRef **)p_MlkRef)[Side]->CowGenNumber;
  }
  return SendMessage( ConLow, GetNodeAddress(Side), 0, LONG_COW_GEN_NUMBER ? MESS_LEN(ExtCowActMemAttRefReq) : MESS_LEN(CowActMemAttRefReq), Buffer);
}

// =============================================================================
// Handle PerMatMlkRef
// =============================================================================
void HndlPerMatMlkRef(NETIO_MAIL *MailPtr)
{
  PerMatMlkRef *PerMatMlkRefPtr = ( PerMatMlkRef *)( &( MailPtr->message_start));

  if (MailPtr->message_length < sizeof(PerMatMlkRef)) {goto exit;}  // bericht is te klein !!!

  StopScheduler();

  PerRbtMlkRef.PerMatMlkStart  = PerMatMlkRefPtr->PerMatMlkStart;
  PerRbtMlkRef.PerMatMlkEnd    = PerMatMlkRefPtr->PerMatMlkEnd;
  PerRbtMlkRef.PerMatMlkFailed = PerMatMlkRefPtr->PerMatMlkFailed;

  ContScheduler();

exit: ;
}


// =============================================================================
// Handle PerMatMlkResReqMlkRes
// =============================================================================
unsigned char HndlPerMatMlkResReq(NETIO_MAIL *MailPtr)
{
  unsigned char      Buffer[MESS_LEN(PerMatMlkRes)];
  PerMatMlkRes      *p_PerMatMlkRes = (PerMatMlkRes *)(Buffer + 2);
  HVC_BUFFER_INFO   *p = HVC_Buffer;
  CONDUCTIVITY_INFO *pi = ConductivityBuffer;
  unsigned char      x;

  *((unsigned short *)Buffer) = PerMatMlkResId;

  StopScheduler();

  p_PerMatMlkRes->PerMatMlkAmount = (unsigned short)((MlkAmountRobot+5)/10);
  p_PerMatMlkRes->PerMatTmpActual = CowTemperature*10;
  p_PerMatMlkRes->PerMatTmpHigh   = MaxCowTemperature*10;
  x = CBWptr == 143 ? 143 : CBWptr-1;
  p_PerMatMlkRes->PerMatConCurrentRF = pi[x].RV;
  p_PerMatMlkRes->PerMatConCurrentRR = pi[x].RA;
  p_PerMatMlkRes->PerMatConCurrentLR = pi[x].LA;
  p_PerMatMlkRes->PerMatConCurrentLF = pi[x].LV;
  p_PerMatMlkRes->PerMatConAvgHighRF = p[0].average;
  p_PerMatMlkRes->PerMatConAvgHighRR = p[1].average;
  p_PerMatMlkRes->PerMatConAvgHighLR = p[2].average;
  p_PerMatMlkRes->PerMatConAvgHighLF = p[3].average;

  ContScheduler();

  return SendMessage( ConLow, MailPtr->address_dest, MailPtr->address_src, MESS_LEN(PerMatMlkRes), Buffer);
}

// =============================================================================
// Service items ontvangen van de PC, neem ze over
// =============================================================================
void HndlPerMPCInstRef(NETIO_MAIL *MailPtr)
{
  BOOLEAN restart = FALSE;
  MPCVersInstRef *MPCVersInstRefPtr = ( MPCVersInstRef *)( &( MailPtr->message_start));

  if (MailPtr->message_length != sizeof(MPCVersInstRef)) {goto exit;}  // bericht is te klein of te groot !!!

  // controleer de versie
  if ( MPCVersInstRefPtr->VersionAppl  != ApplVersion ) return;

  StopScheduler();

  // hou rekening met eventuele NO_MILK_METER_TYPE
  if ( MPCVersInstRefPtr->MilkMeterType !=  (EEMilkMeterType%100) ){
    WriteEEpromByte( MPCVersInstRefPtr->MilkMeterType,     &EEMilkMeterType); restart = TRUE;
  }
  WriteEEpromByte( MPCVersInstRefPtr->MmCorMem,           &EEMmCorMem);
  WriteEEpromByte( MPCVersInstRefPtr->MmCorNiv1,          &EEMmCorNiv1);
  WriteEEpromByte( MPCVersInstRefPtr->MmCorNiv2,          &EEMmCorNiv2);
  WriteEEpromShort(MPCVersInstRefPtr->ValveTime,          &EEValveTime);
  WriteEEpromByte( MPCVersInstRefPtr->FillTime,           (unsigned char *)&EEFillTime);
  WriteEEpromByte( MPCVersInstRefPtr->Language,           &EELanguage);
  WriteEEpromShort(MPCVersInstRefPtr->SendTime,           &EESendTime);
  WriteEEpromShort(MPCVersInstRefPtr->NeutralTime1,       &EENeutralTime1);
  WriteEEpromShort(MPCVersInstRefPtr->NeutralTime2,       &EENeutralTime2);
  WriteEEpromShort(MPCVersInstRefPtr->NeutralThreshold,   &EENeutralThreshold);
  WriteEEpromShort(MPCVersInstRefPtr->TakeOffThreshold,   &EETakeOffThreshold);
  WriteEEpromShort(MPCVersInstRefPtr->TakeOffTime,        (unsigned short *)&EETakeOffTime);
  WriteEEpromByte( MPCVersInstRefPtr->MlkFlwCalcTime,     &EEMlkFlwCalcTime);
  WriteEEpromByte( MPCVersInstRefPtr->OutputBlockBits,    &EEOutputBlockBits);
  WriteEEpromByte( MPCVersInstRefPtr->SepMsgCntrl,        &EESepMsgCntrl);
  WriteEEpromByte( MPCVersInstRefPtr->SepActFuncNr,       &EESepActFuncNr);
  WriteEEpromByte( MPCVersInstRefPtr->SepCondFuncNr,      &EESepCondFuncNr);
  WriteEEpromByte( MPCVersInstRefPtr->SepMinYldFuncNr,    &EESepMinYldFuncNr);
  WriteEEpromByte( MPCVersInstRefPtr->SepTmpFuncNr,       &EESepTmpFuncNr);
  WriteEEpromByte( MPCVersInstRefPtr->ClusterDown,        &EEClusterDown);
  WriteEEpromShort(MPCVersInstRefPtr->ClusterDly,         &EEClusterDly);
  WriteEEpromShort(MPCVersInstRefPtr->VacuumDly,          &EEVacuumDly);
  WriteEEpromByte( MPCVersInstRefPtr->MilkSweepDlyTime,   &EEMilkSweepDlyTime);
  WriteEEpromByte( MPCVersInstRefPtr->MilkSweepActTime,   &EEMilkSweepActTime);
  WriteEEpromByte( MPCVersInstRefPtr->I2Control,          &EEI2Control);
  WriteEEpromByte( MPCVersInstRefPtr->OutputK2,           &EEOutputK2);
  WriteEEpromShort(MPCVersInstRefPtr->FlowLvl2Threshold,  &EEFlowLvl2Threshold);
  WriteEEpromByte( MPCVersInstRefPtr->TrcArmOnTime,       &EETrcArmOnTime);
  WriteEEpromByte( MPCVersInstRefPtr->TrcArmOffTime,      &EETrcArmOffTime);
  WriteEEpromShort(MPCVersInstRefPtr->TAPulsFreq,         &EETAPulseFreq);
  WriteEEpromByte( MPCVersInstRefPtr->TAPulsRatio,        &EETAPulseRatio);
  WriteEEpromShort(MPCVersInstRefPtr->CleanStartTime,     &EECleanStartTime);
  WriteEEpromShort(MPCVersInstRefPtr->OutputP1Dly,        &EEOutputP1Dly);
  WriteEEpromShort(MPCVersInstRefPtr->OutputP1Time,       &EEOutputP1Time);
  WriteEEpromByte( MPCVersInstRefPtr->OutputP1Inv,        &EEOutputP1Inv);
  WriteEEpromShort(MPCVersInstRefPtr->CleanTime,          &EECleanTime);
  WriteEEpromShort(MPCVersInstRefPtr->GateSwitchTime,     &EEGateSwitchTime);
  WriteEEpromShort(MPCVersInstRefPtr->DispTime,           &EEDispTime);
  WriteEEpromByte( MPCVersInstRefPtr->MmSensorMem,        &EEMmSensorMem);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow0,           &EECorFlow0);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow1,           &EECorFlow1);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow2,           &EECorFlow2);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow3,           &EECorFlow3);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow4,           &EECorFlow4);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow5,           &EECorFlow5);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow6,           &EECorFlow6);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow7,           &EECorFlow7);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow8,           &EECorFlow8);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow9,           &EECorFlow9);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow10,          &EECorFlow10);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow11,          &EECorFlow11);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow12,          &EECorFlow12);
  WriteEEpromByte( MPCVersInstRefPtr->CorFlow13,          &EECorFlow13);
  WriteEEpromShort(MPCVersInstRefPtr->Cal1,               &EECal1);
  WriteEEpromShort(MPCVersInstRefPtr->Cal2,               &EECal2);
  WriteEEpromShort(MPCVersInstRefPtr->Cal3,               &EECal3);
  WriteEEpromShort(MPCVersInstRefPtr->Cal4,               &EECal4);
  WriteEEpromByte( MPCVersInstRefPtr->GoatMilkGlass,      &EEGoatMilkGlass);
  WriteEEpromShort(MPCVersInstRefPtr->MmGoatVal1,         &EEMmGoatVal1);
  WriteEEpromShort(MPCVersInstRefPtr->MmGoatVal2,         &EEMmGoatVal2);
  if ( WriteEEpromByte( MPCVersInstRefPtr->Robot,              &EERobot)) restart = TRUE;
  WriteEEpromByte( MPCVersInstRefPtr->ClnPhs1,            &EEClnPhs1);
  WriteEEpromByte( MPCVersInstRefPtr->ClnPhs2,            &EEClnPhs2);
  WriteEEpromByte( MPCVersInstRefPtr->ClnPhs4,            &EEClnPhs4);
  WriteEEpromByte( MPCVersInstRefPtr->EmptyTime,          (unsigned char *)&EEEmptyTime);
  if ( WriteEEpromByte( MPCVersInstRefPtr->Pulsation,          &EEPulsation) ) restart = TRUE;
  WriteEEpromByte( MPCVersInstRefPtr->PulsFrequency,      &EEPulseFrequency);
  WriteEEpromByte( MPCVersInstRefPtr->PulsRatio,          &EEPulseRatio);
  WriteEEpromShort(MPCVersInstRefPtr->StimPulsFreq,       &EEStimPulseFreq);
  WriteEEpromByte( MPCVersInstRefPtr->StimPulsRatio,      &EEStimPulseRatio);
  WriteEEpromShort(MPCVersInstRefPtr->StimTresHold,       &EEStimTresHold);
  WriteEEpromShort(MPCVersInstRefPtr->StimStartTime,      &EEStimStartTime);
  WriteEEpromShort(MPCVersInstRefPtr->StimMaxTime,        &EEStimMaxTime);
  WriteEEpromByte( MPCVersInstRefPtr->NrOfPulsators,      &EENrOfPulsators);
  // indien voeren aan of uit wordt gezet dan een herstart!
  if ( EEFeeding && !(MPCVersInstRefPtr->Feeding) ||
      !EEFeeding && MPCVersInstRefPtr->Feeding  ) restart = TRUE;
  WriteEEpromByte( MPCVersInstRefPtr->Feeding,            &EEFeeding);
  WriteEEpromShort(MPCVersInstRefPtr->FeedSpeed,          &EEFeedSpeed);
  WriteEEpromShort(MPCVersInstRefPtr->FeedMtrPortion,     &(EEFeedMtrPortion[0]));
  WriteEEpromShort(MPCVersInstRefPtr->FeedBalanceDef,     &(EEFeedBalanceDef[0]));
  WriteEEpromShort(MPCVersInstRefPtr->RelaisTime,         &EERelaisTime);
  WriteEEpromShort(MPCVersInstRefPtr->SB_FeedMtrPortion,  &(EEFeedMtrPortion[1]));
  WriteEEpromShort(MPCVersInstRefPtr->SB_FeedBalanceDef,  &(EEFeedBalanceDef[1]));
  WriteEEpromShort(MPCVersInstRefPtr->SB_RelaisTime,      &EESB_RelaisTime);
  WriteEEpromByte( MPCVersInstRefPtr->Test,               &EETest);
  if ( WriteEEpromByte( MPCVersInstRefPtr->ParlourType,        &EEParlourType)) restart = TRUE;
  WriteEEpromByte( MPCVersInstRefPtr->GateK3,             &EEGateK3);
  WriteEEpromByte( MPCVersInstRefPtr->GateK4,             &EEGateK4);
  WriteEEpromByte( MPCVersInstRefPtr->GateK5,             &EEGateK5);
  if ( WriteEEpromByte( MPCVersInstRefPtr->OTRowMaster,        &EEOTRowMaster))restart = TRUE;
  WriteEEpromByte( MPCVersInstRefPtr->ServiceStands,      &EEServiceStands);
  WriteEEpromByte( MPCVersInstRefPtr->RowSize,            &EERowSize);
  if ( WriteEEpromByte( MPCVersInstRefPtr->ConMeasuring,       &EEConMeasuring)) restart = TRUE;
  if ( WriteEEpromByte( MPCVersInstRefPtr->ExtConMeasuring,    &EEExtConMeasuring))restart = TRUE;
  if ( WriteEEpromByte( MPCVersInstRefPtr->TempMeasuring,      &EETempMeasuring))restart = TRUE;
  WriteEEpromShort(MPCVersInstRefPtr->TempRValue,         &EETempRValue);

  ContScheduler();

  if ( restart) RomReset();
exit: ;
}

// =============================================================================
// PC vraagt om Service items, stuur ze op
// =============================================================================
unsigned char HndlPerMPCInstResReq( NETIO_MAIL *MailPtr)
{
  unsigned char Buffer[MESS_LEN(MPCVersInstRef)];

  PerMPCInstResReq *PerMPCInstResReqPtr = (PerMPCInstResReq *)(&(MailPtr->message_start));
  MPCVersInstRef   *MPCVersInstRefPtr   = (MPCVersInstRef *)(Buffer +2);

  StopScheduler();

  // zend peripheral instellingen
  *((unsigned short *)Buffer)           = PerMPCInstResId;
  MPCVersInstRefPtr->PerGenNumber       = PerMPCInstResReqPtr->PerGenNumber;
  MPCVersInstRefPtr->VersionAppl        = ApplVersion;
  MPCVersInstRefPtr->Address            = EEAddress;
  MPCVersInstRefPtr->Address2           = EEAddress2;
  MPCVersInstRefPtr->MilkMeterType      = EEMilkMeterType%100;    // afkappen NO_MILK_METER_TYPE
  MPCVersInstRefPtr->MmCorMem           = EEMmCorMem;
  MPCVersInstRefPtr->MmCorNiv1          = EEMmCorNiv1;
  MPCVersInstRefPtr->MmCorNiv2          = EEMmCorNiv2;
  MPCVersInstRefPtr->ValveTime          = EEValveTime;
  MPCVersInstRefPtr->FillTime           = EEFillTime;
  MPCVersInstRefPtr->Language           = EELanguage;
  MPCVersInstRefPtr->SendTime           = EESendTime;
  MPCVersInstRefPtr->NeutralTime1       = EENeutralTime1;
  MPCVersInstRefPtr->NeutralTime2       = EENeutralTime2;
  MPCVersInstRefPtr->NeutralThreshold   = EENeutralThreshold;
  MPCVersInstRefPtr->TakeOffThreshold   = EETakeOffThreshold;
  MPCVersInstRefPtr->TakeOffTime        = EETakeOffTime;
  MPCVersInstRefPtr->MlkFlwCalcTime     = EEMlkFlwCalcTime;
  MPCVersInstRefPtr->OutputBlockBits    = EEOutputBlockBits;
  MPCVersInstRefPtr->SepMsgCntrl        = EESepMsgCntrl;
  MPCVersInstRefPtr->SepActFuncNr       = EESepActFuncNr;
  MPCVersInstRefPtr->SepCondFuncNr      = EESepCondFuncNr;
  MPCVersInstRefPtr->SepMinYldFuncNr    = EESepMinYldFuncNr;
  MPCVersInstRefPtr->SepTmpFuncNr       = EESepTmpFuncNr;
  MPCVersInstRefPtr->ClusterDown        = EEClusterDown;
  MPCVersInstRefPtr->ClusterDly         = EEClusterDly;
  MPCVersInstRefPtr->VacuumDly          = EEVacuumDly;
  MPCVersInstRefPtr->MilkSweepDlyTime   = EEMilkSweepDlyTime;
  MPCVersInstRefPtr->MilkSweepActTime   = EEMilkSweepActTime;
  MPCVersInstRefPtr->I2Control          = EEI2Control;
  MPCVersInstRefPtr->OutputK2           = EEOutputK2;
  MPCVersInstRefPtr->FlowLvl2Threshold  = EEFlowLvl2Threshold;
  MPCVersInstRefPtr->CleanStartTime     = EECleanStartTime;
  MPCVersInstRefPtr->OutputP1Dly        = EEOutputP1Dly;
  MPCVersInstRefPtr->OutputP1Time       = EEOutputP1Time;
  MPCVersInstRefPtr->OutputP1Inv        = EEOutputP1Inv;
  MPCVersInstRefPtr->CleanTime          = EECleanTime;
  MPCVersInstRefPtr->GateSwitchTime     = EEGateSwitchTime;
  MPCVersInstRefPtr->DispTime           = EEDispTime;
  MPCVersInstRefPtr->MmSensorMem        = EEMmSensorMem;
  MPCVersInstRefPtr->CorFlow0           = EECorFlow0;
  MPCVersInstRefPtr->CorFlow1           = EECorFlow1;
  MPCVersInstRefPtr->CorFlow2           = EECorFlow2;
  MPCVersInstRefPtr->CorFlow3           = EECorFlow3;
  MPCVersInstRefPtr->CorFlow4           = EECorFlow4;
  MPCVersInstRefPtr->CorFlow5           = EECorFlow5;
  MPCVersInstRefPtr->CorFlow6           = EECorFlow6;
  MPCVersInstRefPtr->CorFlow7           = EECorFlow7;
  MPCVersInstRefPtr->CorFlow8           = EECorFlow8;
  MPCVersInstRefPtr->CorFlow9           = EECorFlow9;
  MPCVersInstRefPtr->CorFlow10          = EECorFlow10;
  MPCVersInstRefPtr->CorFlow11          = EECorFlow11;
  MPCVersInstRefPtr->CorFlow12          = EECorFlow12;
  MPCVersInstRefPtr->CorFlow13          = EECorFlow13;
  MPCVersInstRefPtr->Cal1               = EECal1;
  MPCVersInstRefPtr->Cal2               = EECal2;
  MPCVersInstRefPtr->Cal3               = EECal3;
  MPCVersInstRefPtr->Cal4               = EECal4;
  MPCVersInstRefPtr->GoatMilkGlass      = EEGoatMilkGlass;
  MPCVersInstRefPtr->MmGoatVal1         = EEMmGoatVal1;
  MPCVersInstRefPtr->MmGoatVal2         = EEMmGoatVal2;
  MPCVersInstRefPtr->Robot              = EERobot;
  MPCVersInstRefPtr->PrgVrsIC13         = IC13PrgVrs;
  MPCVersInstRefPtr->PrgVrsIC09         = IC09PrgVrs;
  MPCVersInstRefPtr->PrgVrsIC08         = IC08PrgVrs;
  MPCVersInstRefPtr->VersionRom         = RomVersion;
  MPCVersInstRefPtr->ClnPhs1            = EEClnPhs1;
  MPCVersInstRefPtr->ClnPhs2            = EEClnPhs2;
  MPCVersInstRefPtr->ClnPhs4            = EEClnPhs4;
  MPCVersInstRefPtr->EmptyTime          = EEEmptyTime;
  MPCVersInstRefPtr->Pulsation          = EEPulsation;
  MPCVersInstRefPtr->PulsFrequency      = EEPulseFrequency;
  MPCVersInstRefPtr->PulsRatio          = EEPulseRatio;
  MPCVersInstRefPtr->StimPulsFreq       = EEStimPulseFreq;
  MPCVersInstRefPtr->StimPulsRatio      = EEStimPulseRatio;
  MPCVersInstRefPtr->StimTresHold       = EEStimTresHold;
  MPCVersInstRefPtr->StimStartTime      = EEStimStartTime;
  MPCVersInstRefPtr->StimMaxTime        = EEStimMaxTime;
  MPCVersInstRefPtr->NrOfPulsators      = EENrOfPulsators;
  MPCVersInstRefPtr->Feeding            = EEFeeding;
  MPCVersInstRefPtr->FeedSpeed          = EEFeedSpeed;
  MPCVersInstRefPtr->FeedMtrPortion     = EEFeedMtrPortion[0];
  MPCVersInstRefPtr->FeedBalanceDef     = EEFeedBalanceDef[0];
  MPCVersInstRefPtr->RelaisTime         = EERelaisTime;
  MPCVersInstRefPtr->SB_FeedMtrPortion  = EEFeedMtrPortion[0];
  MPCVersInstRefPtr->SB_FeedBalanceDef  = EEFeedBalanceDef[0];
  MPCVersInstRefPtr->SB_RelaisTime      = EESB_RelaisTime;
  MPCVersInstRefPtr->Test               = EETest;
  MPCVersInstRefPtr->ParlourType        = EEParlourType;
  MPCVersInstRefPtr->GateK3             = EEGateK3;
  MPCVersInstRefPtr->GateK4             = EEGateK4;
  MPCVersInstRefPtr->GateK5             = EEGateK5;
  MPCVersInstRefPtr->OTRowMaster        = EEOTRowMaster;
  MPCVersInstRefPtr->ServiceStands      = EEServiceStands;
  MPCVersInstRefPtr->RowSize            = EERowSize;
  MPCVersInstRefPtr->ConMeasuring       = EEConMeasuring;
  MPCVersInstRefPtr->ExtConMeasuring    = EEExtConMeasuring;
  MPCVersInstRefPtr->TempMeasuring      = EETempMeasuring;
  MPCVersInstRefPtr->TempRValue         = EETempRValue;
  MPCVersInstRefPtr->KeySoft            = SoftKey;

  ContScheduler();

  return SendMessage( ConLow, MailPtr->address_dest, 0, sizeof(Buffer), Buffer );
}


// =============================================================================
// stuur de service items, die gelijk zijn, naar de ander MPC's
// =============================================================================
void SendDefSet( void )
{
  unsigned char           buffer[sizeof(NiveauDefSetUpdateRef) > sizeof(MemolacDefSetUpdateRef)
                             ? sizeof(NiveauDefSetUpdateRef)+4 : sizeof(MemolacDefSetUpdateRef)+4];
  unsigned char           i;
  unsigned short          time;
  unsigned short          message_length = 0;
  NiveauDefSetUpdateRef  *NiveauDefSetUpdateRefPtr;
  MemolacDefSetUpdateRef *MemolacDefSetUpdateRefPtr;
  VFlowDefSetUpdateRef   *VFlowDefSetUpdateRefPtr;
  PerFuncTest            *PerFuncTestPtr;
    signed char           idx = GetNodeAddress(SIDE_A);

  *((unsigned short *)buffer) = PerFuncTestId;
  PerFuncTestPtr = (PerFuncTest *)(buffer + 2);

  switch ( EEMilkMeterType%100 ){
    case MMT_NIVEAU:
         PerFuncTestPtr->PerFuncTestCommand = NiveauDefSetUpdateRefId;
         message_length = 2 * sizeof(unsigned short);

         NiveauDefSetUpdateRefPtr = (NiveauDefSetUpdateRef *)(buffer+4);
         NiveauDefSetUpdateRefPtr->Language          = EELanguage;
         NiveauDefSetUpdateRefPtr->SendTime          = EESendTime;
         NiveauDefSetUpdateRefPtr->NeutralTime1      = EENeutralTime1;
         NiveauDefSetUpdateRefPtr->NeutralTime2      = EENeutralTime2;
         NiveauDefSetUpdateRefPtr->NeutralThreshold  = EENeutralThreshold;
         NiveauDefSetUpdateRefPtr->TakeOffThreshold  = EETakeOffThreshold;
         NiveauDefSetUpdateRefPtr->TakeOffTime       = EETakeOffTime;
         NiveauDefSetUpdateRefPtr->MlkFlwCalcTime    = EEMlkFlwCalcTime;
         NiveauDefSetUpdateRefPtr->OutputBlockBits   = EEOutputBlockBits;
         NiveauDefSetUpdateRefPtr->SepMsgCntrl       = EESepMsgCntrl;
         NiveauDefSetUpdateRefPtr->SepActFuncNr      = EESepActFuncNr;
         NiveauDefSetUpdateRefPtr->SepCondFuncNr     = EESepCondFuncNr;
         NiveauDefSetUpdateRefPtr->SepMinYldFuncNr   = EESepMinYldFuncNr;
         NiveauDefSetUpdateRefPtr->SepTmpFuncNr      = EESepTmpFuncNr;
         NiveauDefSetUpdateRefPtr->ClusterDown       = EEClusterDown;
         NiveauDefSetUpdateRefPtr->ClusterDly        = EEClusterDly;
         NiveauDefSetUpdateRefPtr->VacuumDly         = EEVacuumDly;
         NiveauDefSetUpdateRefPtr->MilkSweepDlyTime  = EEMilkSweepDlyTime;
         NiveauDefSetUpdateRefPtr->MilkSweepActTime  = EEMilkSweepActTime;
         NiveauDefSetUpdateRefPtr->I2Control         = EEI2Control;
         NiveauDefSetUpdateRefPtr->OutputK2          = EEOutputK2;
         NiveauDefSetUpdateRefPtr->FlowLvl2Threshold = EEFlowLvl2Threshold;
         NiveauDefSetUpdateRefPtr->OutputP1Dly       = EEOutputP1Dly;
         NiveauDefSetUpdateRefPtr->OutputP1Time      = EEOutputP1Time;
         NiveauDefSetUpdateRefPtr->OutputP1Inv       = EEOutputP1Inv;
         NiveauDefSetUpdateRefPtr->CleanStartTime    = EECleanStartTime;
         NiveauDefSetUpdateRefPtr->CleanTime         = EECleanTime;
         NiveauDefSetUpdateRefPtr->GateSwitchTime    = EEGateSwitchTime;
         NiveauDefSetUpdateRefPtr->DispTime          = EEDispTime;
         NiveauDefSetUpdateRefPtr->GoatMilkGlass     = EEGoatMilkGlass;
         NiveauDefSetUpdateRefPtr->MmGoatVal1        = EEMmGoatVal1;
         NiveauDefSetUpdateRefPtr->MmGoatVal2        = EEMmGoatVal2;
         NiveauDefSetUpdateRefPtr->Pulsation         = EEPulsation;
         NiveauDefSetUpdateRefPtr->PulseFrequency    = EEPulseFrequency;
         NiveauDefSetUpdateRefPtr->PulseRatio        = EEPulseRatio;
         NiveauDefSetUpdateRefPtr->StimPulseFreq     = EEStimPulseFreq;
         NiveauDefSetUpdateRefPtr->StimPulseRatio    = EEStimPulseRatio;
         NiveauDefSetUpdateRefPtr->StimTresHold      = EEStimTresHold;
         NiveauDefSetUpdateRefPtr->StimStartTime     = EEStimStartTime;
         NiveauDefSetUpdateRefPtr->StimMaxTime       = EEStimMaxTime;
         NiveauDefSetUpdateRefPtr->NrOfPulsators     = EENrOfPulsators;
         NiveauDefSetUpdateRefPtr->Feeding           = EEFeeding;
         NiveauDefSetUpdateRefPtr->RelaisTime        = EERelaisTime;
         NiveauDefSetUpdateRefPtr->SB_RelaisTime     = EESB_RelaisTime;
         NiveauDefSetUpdateRefPtr->MilkMeterType     = EEMilkMeterType;
         NiveauDefSetUpdateRefPtr->ParlourType       = EEParlourType;
         NiveauDefSetUpdateRefPtr->CentralGate       = EEGateK3;
         NiveauDefSetUpdateRefPtr->EntranceGate      = EEGateK4;
         NiveauDefSetUpdateRefPtr->ExitGate          = EEGateK5;
         NiveauDefSetUpdateRefPtr->OTRowMaster       = EEOTRowMaster;
         NiveauDefSetUpdateRefPtr->RowSize           = 0;
         NiveauDefSetUpdateRefPtr->ConMeasuring      = EEConMeasuring;
         NiveauDefSetUpdateRefPtr->ExtConMeasuring   = EEExtConMeasuring;
         NiveauDefSetUpdateRefPtr->TempMeasuring     = EETempMeasuring;
         NiveauDefSetUpdateRefPtr->I1Control         = EEI1Control;
         message_length += sizeof(NiveauDefSetUpdateRef);
      break;

    case MMT_VFLOW:
         PerFuncTestPtr->PerFuncTestCommand = VFlowDefSetUpdateRefId;
         message_length = 2 * sizeof(unsigned short);
         VFlowDefSetUpdateRefPtr = (VFlowDefSetUpdateRef *)(buffer+4);
         VFlowDefSetUpdateRefPtr->Language          = EELanguage;
         VFlowDefSetUpdateRefPtr->SendTime          = EESendTime;
         VFlowDefSetUpdateRefPtr->NeutralTime1      = EENeutralTime1;
         VFlowDefSetUpdateRefPtr->NeutralTime2      = EENeutralTime2;
         VFlowDefSetUpdateRefPtr->NeutralThreshold  = EENeutralThreshold;
         VFlowDefSetUpdateRefPtr->TakeOffThreshold  = EETakeOffThreshold;
         VFlowDefSetUpdateRefPtr->TakeOffTime       = EETakeOffTime;
         VFlowDefSetUpdateRefPtr->OutputBlockBits   = EEOutputBlockBits;
         VFlowDefSetUpdateRefPtr->SepMsgCntrl       = EESepMsgCntrl;
         VFlowDefSetUpdateRefPtr->SepActFuncNr      = EESepActFuncNr;
         VFlowDefSetUpdateRefPtr->SepCondFuncNr     = EESepCondFuncNr;
         VFlowDefSetUpdateRefPtr->SepMinYldFuncNr   = EESepMinYldFuncNr;
         VFlowDefSetUpdateRefPtr->SepTmpFuncNr      = EESepTmpFuncNr;
         VFlowDefSetUpdateRefPtr->ClusterDown       = EEClusterDown;
         VFlowDefSetUpdateRefPtr->ClusterDly        = EEClusterDly;
         VFlowDefSetUpdateRefPtr->VacuumDly         = EEVacuumDly;
         VFlowDefSetUpdateRefPtr->MilkSweepDlyTime  = EEMilkSweepDlyTime;
         VFlowDefSetUpdateRefPtr->MilkSweepActTime  = EEMilkSweepActTime;
         VFlowDefSetUpdateRefPtr->I1Control         = EEI1Control;
         VFlowDefSetUpdateRefPtr->I2Control         = EEI2Control;
         VFlowDefSetUpdateRefPtr->OutputK2          = EEOutputK2;
         VFlowDefSetUpdateRefPtr->FlowLvl2Threshold = EEFlowLvl2Threshold;
         VFlowDefSetUpdateRefPtr->TrcArmOnTime      = EETrcArmOnTime;
         VFlowDefSetUpdateRefPtr->TrcArmOffTime     = EETrcArmOffTime;
         VFlowDefSetUpdateRefPtr->TAPulsFreq        = EETAPulseFreq;
         VFlowDefSetUpdateRefPtr->TAPulsRatio       = EETAPulseRatio;
         VFlowDefSetUpdateRefPtr->CleanStartTime    = EECleanStartTime;
         VFlowDefSetUpdateRefPtr->GateSwitchTime    = EEGateSwitchTime;
         VFlowDefSetUpdateRefPtr->OutputP1Dly       = EEOutputP1Dly;
         VFlowDefSetUpdateRefPtr->OutputP1Inv       = EEOutputP1Inv;
         VFlowDefSetUpdateRefPtr->DispTime          = EEDispTime;
         VFlowDefSetUpdateRefPtr->CorVFlow          = EECorVFlow;
         VFlowDefSetUpdateRefPtr->ClnPhs1           = EEClnPhs1;
         VFlowDefSetUpdateRefPtr->ClnPhs2           = EEClnPhs2;
         VFlowDefSetUpdateRefPtr->ClnPhs4           = EEClnPhs4;
         VFlowDefSetUpdateRefPtr->Pulsation         = EEPulsation;
         VFlowDefSetUpdateRefPtr->PulseFrequency    = EEPulseFrequency;
         VFlowDefSetUpdateRefPtr->PulseRatio        = EEPulseRatio;
         VFlowDefSetUpdateRefPtr->StimPulseFreq     = EEStimPulseFreq;
         VFlowDefSetUpdateRefPtr->StimPulseRatio    = EEStimPulseRatio;
         VFlowDefSetUpdateRefPtr->StimTresHold      = EEStimTresHold;
         VFlowDefSetUpdateRefPtr->StimStartTime     = EEStimStartTime;
         VFlowDefSetUpdateRefPtr->StimMaxTime       = EEStimMaxTime;
         VFlowDefSetUpdateRefPtr->NrOfPulsators     = EENrOfPulsators;
         VFlowDefSetUpdateRefPtr->Feeding           = EEFeeding;
         VFlowDefSetUpdateRefPtr->RelaisTime        = EERelaisTime;
         VFlowDefSetUpdateRefPtr->SB_RelaisTime     = EESB_RelaisTime;
         VFlowDefSetUpdateRefPtr->MilkMeterType     = EEMilkMeterType;
         VFlowDefSetUpdateRefPtr->ParlourType       = EEParlourType;
         VFlowDefSetUpdateRefPtr->GateK3Time        = EEGateK3Time;
         VFlowDefSetUpdateRefPtr->GateK4Time        = EEGateK4Time;
         VFlowDefSetUpdateRefPtr->GateK5Time        = EEGateK5Time;
         VFlowDefSetUpdateRefPtr->GateK3            = EEGateK3;
         VFlowDefSetUpdateRefPtr->GateK4            = EEGateK4;
         VFlowDefSetUpdateRefPtr->GateK5            = EEGateK5;
         VFlowDefSetUpdateRefPtr->OTRowMaster       = EEOTRowMaster;
         VFlowDefSetUpdateRefPtr->RowSize           = 0;
         VFlowDefSetUpdateRefPtr->ConMeasuring      = EEConMeasuring;
         VFlowDefSetUpdateRefPtr->TempMeasuring     = EETempMeasuring;
         message_length += sizeof(VFlowDefSetUpdateRef);
    break;
    case MMT_MEMOLAC:
    case MMT_TAKE_OFF_SENSOR:
         PerFuncTestPtr->PerFuncTestCommand = MemolacDefSetUpdateRefId;
         message_length = 2 * sizeof(unsigned short);
         MemolacDefSetUpdateRefPtr = (MemolacDefSetUpdateRef *)(buffer+4);
         MemolacDefSetUpdateRefPtr->ValveTime         = EEValveTime;
         MemolacDefSetUpdateRefPtr->FillTime          = EEFillTime;
         MemolacDefSetUpdateRefPtr->Language          = EELanguage;
         MemolacDefSetUpdateRefPtr->SendTime          = EESendTime;
         MemolacDefSetUpdateRefPtr->NeutralTime1      = EENeutralTime1;
         MemolacDefSetUpdateRefPtr->NeutralTime2      = EENeutralTime2;
         MemolacDefSetUpdateRefPtr->NeutralThreshold  = EENeutralThreshold;
         MemolacDefSetUpdateRefPtr->TakeOffThreshold  = EETakeOffThreshold;
         MemolacDefSetUpdateRefPtr->TakeOffTime       = EETakeOffTime;
         MemolacDefSetUpdateRefPtr->MlkFlwCalcTime    = EEMlkFlwCalcTime;
         MemolacDefSetUpdateRefPtr->OutputBlockBits   = EEOutputBlockBits;
         MemolacDefSetUpdateRefPtr->SepMsgCntrl       = EESepMsgCntrl;
         MemolacDefSetUpdateRefPtr->SepActFuncNr      = EESepActFuncNr;
         MemolacDefSetUpdateRefPtr->SepCondFuncNr     = EESepCondFuncNr;
         MemolacDefSetUpdateRefPtr->SepMinYldFuncNr   = EESepMinYldFuncNr;
         MemolacDefSetUpdateRefPtr->SepTmpFuncNr      = EESepTmpFuncNr;
         MemolacDefSetUpdateRefPtr->ClusterDown       = EEClusterDown;
         MemolacDefSetUpdateRefPtr->ClusterDly        = EEClusterDly;
         MemolacDefSetUpdateRefPtr->VacuumDly         = EEVacuumDly;
         MemolacDefSetUpdateRefPtr->MilkSweepDlyTime  = EEMilkSweepDlyTime;
         MemolacDefSetUpdateRefPtr->MilkSweepActTime  = EEMilkSweepActTime;
         MemolacDefSetUpdateRefPtr->I2Control         = EEI2Control;
         MemolacDefSetUpdateRefPtr->OutputK2          = EEOutputK2;
         MemolacDefSetUpdateRefPtr->FlowLvl2Threshold = EEFlowLvl2Threshold;
         MemolacDefSetUpdateRefPtr->CleanStartTime    = EECleanStartTime;
         MemolacDefSetUpdateRefPtr->GateSwitchTime    = EEGateSwitchTime;
         MemolacDefSetUpdateRefPtr->DispTime          = EEDispTime;
         MemolacDefSetUpdateRefPtr->MmSensorMem       = EEMmSensorMem;
         MemolacDefSetUpdateRefPtr->ClnPhs1           = EEClnPhs1;
         MemolacDefSetUpdateRefPtr->ClnPhs2           = EEClnPhs2;
         MemolacDefSetUpdateRefPtr->ClnPhs4           = EEClnPhs4;
         MemolacDefSetUpdateRefPtr->EmptyTime         = EEEmptyTime;
         MemolacDefSetUpdateRefPtr->Pulsation         = EEPulsation;
         MemolacDefSetUpdateRefPtr->PulseFrequency    = EEPulseFrequency;
         MemolacDefSetUpdateRefPtr->PulseRatio        = EEPulseRatio;
         MemolacDefSetUpdateRefPtr->StimPulseFreq     = EEStimPulseFreq;
         MemolacDefSetUpdateRefPtr->StimPulseRatio    = EEStimPulseRatio;
         MemolacDefSetUpdateRefPtr->StimTresHold      = EEStimTresHold;
         MemolacDefSetUpdateRefPtr->StimStartTime     = EEStimStartTime;
         MemolacDefSetUpdateRefPtr->StimMaxTime       = EEStimMaxTime;
         MemolacDefSetUpdateRefPtr->NrOfPulsators     = EENrOfPulsators;
         MemolacDefSetUpdateRefPtr->Feeding           = EEFeeding;
         MemolacDefSetUpdateRefPtr->RelaisTime        = EERelaisTime;
         MemolacDefSetUpdateRefPtr->SB_RelaisTime     = EESB_RelaisTime;
         MemolacDefSetUpdateRefPtr->MilkMeterType     = EEMilkMeterType;
         MemolacDefSetUpdateRefPtr->ParlourType       = EEParlourType;
         MemolacDefSetUpdateRefPtr->CentralGate       = EEGateK3;
         MemolacDefSetUpdateRefPtr->EntranceGate      = EEGateK4;
         MemolacDefSetUpdateRefPtr->ExitGate          = EEGateK5;
         MemolacDefSetUpdateRefPtr->OTRowMaster       = EEOTRowMaster;
         MemolacDefSetUpdateRefPtr->RowSize           = 0;
         MemolacDefSetUpdateRefPtr->ConMeasuring      = EEConMeasuring;
         MemolacDefSetUpdateRefPtr->ExtConMeasuring   = EEExtConMeasuring;
         MemolacDefSetUpdateRefPtr->TempMeasuring     = EETempMeasuring;
         MemolacDefSetUpdateRefPtr->I1Control         = EEI1Control;
         MemolacDefSetUpdateRefPtr->TrcArmOnTime      = EETrcArmOnTime;        // tbv 'nach milch automat'
         MemolacDefSetUpdateRefPtr->TrcArmOffTime     = EETrcArmOffTime;
         MemolacDefSetUpdateRefPtr->TAPulseFreq       = EETAPulseFreq;
         MemolacDefSetUpdateRefPtr->TAPulseRatio      = EETAPulseRatio;
         message_length += sizeof(MemolacDefSetUpdateRef);
    break;
  } // end switch ( EEMilkMeterType )

  if ( message_length ){
    for( i = 1; i < EEServiceStands;  ){
      if (SendMessage( ConLow, idx, idx+i, message_length, buffer )){
      	i++;                     // next message
      
        time = Cnt1sec;
        while (NetCheckSending(1) == 1){
      	  WatchDog();
          TaskExit();
      	  if (TimeControl( Cnt1sec, time+8))break;
        }
      }
      TaskExit();
    } 
//    while (NetCheckSending(1) == 1)WatchDog();
    time = Cnt1sec;
    while(!TimeControl( Cnt1sec, time+8))WatchDog();
  }
}


// =============================================================================
void HndlFTBurnInResReq(NETIO_MAIL *MailPtr)
{
  unsigned char Buffer[sizeof(short) + sizeof(PerFuncTest)];

  PerFuncTest *PerFuncTestPtr = ( PerFuncTest *)(Buffer +2);
  *((unsigned short *)Buffer)        = PerFuncTestId;
  PerFuncTestPtr->PerFuncTestCommand = FTBurnInResId;

  StopScheduler();

  PerFuncTestPtr->Array[0].PerFuncTestDataHigh = TestResult.Ready;
  PerFuncTestPtr->Array[0].PerFuncTestDataLow  = TestResult.RomResult;
  PerFuncTestPtr->Array[1].PerFuncTestDataHigh = TestResult.RamResult;
  PerFuncTestPtr->Array[1].PerFuncTestDataLow  = TestResult.VC3Result;
  PerFuncTestPtr->Array[2].PerFuncTestDataHigh = TestResult.PMResult;
  PerFuncTestPtr->Array[2].PerFuncTestDataLow  = TestResult.RelaisResult;
  PerFuncTestPtr->Array[3].PerFuncTestDataHigh = TestResult.UartResult;
  PerFuncTestPtr->Array[3].PerFuncTestDataLow  = TestResult.DummyResult;

  ContScheduler();

  SendMessage( ConLow, MailPtr->address_dest, 0, sizeof(Buffer), Buffer );
}

// =============================================================================
void HndlPerGenInstResReq( NETIO_MAIL *MailPtr)
{
  unsigned char AddressChanged = 0;
  unsigned char Buffer[MESS_LEN(PerGenInstRes)];
  unsigned char idx;

  PerGenInstResReq *PerGenInstResReqPtr = (PerGenInstResReq *)(&(MailPtr->message_start));
  PerGenInstRes *PerGenInstResPtr = (PerGenInstRes *)(Buffer +2);

  idx = GetNodeIndex(MailPtr->address_dest);

  StopScheduler();

  // zend peripheral instellingen
  *((unsigned short *)Buffer)                = PerGenInstResId;
  PerGenInstResPtr->PerGenNumber             = PerGenInstResReqPtr->PerGenNumber;
  PerGenInstResPtr->PerGenProgCode           = PerConfigData.PerGenProgCode;
  PerGenInstResPtr->PerGenProgVersionHigh    = VERSION_HIGH(RomProgVersion);
  PerGenInstResPtr->PerGenProgVersionLow     = VERSION_LOW(RomProgVersion);
  PerGenInstResPtr->PerGenFuncIdentification = PerConfigData.PerGenFuncIdentification;
  PerGenInstResPtr->PerGenFuncFeeding        = EEFeeding != 0 ? 1 : 0;
  PerGenInstResPtr->PerGenFuncMilkMeasuring  = PerConfigData.PerGenFuncMilkMeasuring;
  PerGenInstResPtr->PerGenFuncAccess         = PerConfigData.PerGenFuncAccess;
  PerGenInstResPtr->PerGenFuncSeparation     = PerConfigData.PerGenFuncSeparation;
  PerGenInstResPtr->PerGenFuncWeight         = PerConfigData.PerGenFuncWeight;
  PerGenInstResPtr->PerGenFuncTemperature    = EETempMeasuring;
  PerGenInstResPtr->PerGenFuncConductivity   = EEConMeasuring != 0 ? 1 : 0;
  PerGenInstResPtr->PerGenFuncActivity       = PerConfigData.PerGenFuncActivity;
  PerGenInstResPtr->PerGenFuncDrinking       = PerConfigData.PerGenFuncDrinking;
  PerGenInstResPtr->PerGenFuncMarking        = PerConfigData.PerGenFuncMarking;
  PerGenInstResPtr->PerGenType               = EEMilkMeterType%100;
  if (idx == 0) {
    PerGenInstResPtr->PerGenDownload = 0x01;
  } else {
    PerGenInstResPtr->PerGenDownload = 0;
  }
  PerGenInstResPtr->PerGenEEProgVersHigh     = VERSION_HIGH(ApplProgVersion);
  PerGenInstResPtr->PerGenEEProgVersLow      = VERSION_LOW(ApplProgVersion);
  PerGenInstResPtr->PerGenDldBrdcst          = ( RomVersion >= 5 ) ? 0x01 : 0;

  ContScheduler();

  SendMessage( ConLow, MailPtr->address_dest, 0, sizeof(Buffer), Buffer );
}

// =============================================================================
// Erase EEProm Result
// =============================================================================
void HndlDldPerEraseRes(void)
{
  unsigned char Buffer[sizeof(unsigned short) + sizeof(DldPerEraseRes)];
  DldPerEraseRes *DldPerEraseResPtr = (DldPerEraseRes *)(Buffer +2);
  *((unsigned short *)Buffer) = DldPerEraseResId;

  DldPerEraseResPtr->DldPerGenNumber = PerGenNumber[0];
  DldPerEraseResPtr->DldPerResult =  DLR_ERROR_VERSION;

  SendMessage( ConLow, EEAddress, 0, sizeof(Buffer), Buffer );
}



// =============================================================================
// Erase EEProm
// =============================================================================
void HndlDldPerEraseRef( NETIO_MAIL *MailPtr)
{
  unsigned char RetVal, Count, MaxCount;
  DldPerEraseRef *DldPerEraseRefPtr = (DldPerEraseRef *)(&(MailPtr->message_start));


  if (MailPtr->message_length >= (sizeof(DldPerEraseRef) - sizeof(DldPerEraseRefPtr->Array[0]))) {

    // bepaal het aantal results
    MaxCount = (MailPtr->message_length - sizeof(DldPerEraseRef)) / sizeof(DldPerEraseRefPtr->Array[0]) + 1;

    if (DldPerEraseRefPtr->Array[MaxCount-1].DldPerData == 0xffffffff) {

      RetVal = 0;

      // Check Rom-versions
      for (Count = 2; Count < MaxCount; Count++) {
        if ((DldPerEraseRefPtr->Array[Count].DldPerData) == RomProgVersion) {
          RetVal = 1;
          break;
        }
      }

      Count = 1;
      //Check file type  'CNM4MAE'
      if ( (DldPerEraseRefPtr->Array[0].DldPerData)     != 0x434E4D34 ||
           (DldPerEraseRefPtr->Array[Count].DldPerData) != 0x4D414520) {
        RetVal = 0;
      }


      if (RetVal == 1) {
        *(unsigned char *)CheckSum_Appl = 0;       // voor emulator
        *(unsigned char *)CheckSum_Comp_Appl = 0;  // voor emulator

        if (EEBankSize != DldPerEraseRefPtr->EEBankSize) {
          WriteEEpromShort(DldPerEraseRefPtr->EEBankSize, &EEBankSize);
        }

        if (EEBankStart != DldPerEraseRefPtr->StartEEBank) {
          WriteEEpromShort(DldPerEraseRefPtr->StartEEBank, &EEBankStart);
        }

        if (EENrOfBanks != DldPerEraseRefPtr->NrEEBanks) {
          WriteEEpromByte(DldPerEraseRefPtr->NrEEBanks, &EENrOfBanks);
        }

        if (EEBankPROG != DldPerEraseRefPtr->StartPROGBank) {
          WriteEEpromByte(DldPerEraseRefPtr->StartPROGBank, &EEBankPROG);
        }

        if (EEBankDBMS != DldPerEraseRefPtr->StartDBMSBank) {
          WriteEEpromByte(DldPerEraseRefPtr->StartDBMSBank, &EEBankDBMS);
        }

        EraseFlashProm();
      } else {
        DL_Status = DOWN_LOAD_VERSION_FAIL;
        HndlDldPerEraseRes();
      }
    }
  }
}


// =============================================================================
void HndlFTBurnInTotResReq(NETIO_MAIL *MailPtr)
{
  unsigned char Buffer[sizeof(short) + sizeof(PerFuncTest) ];

  PerFuncTest *PerFuncTestPtr = ( PerFuncTest *)(Buffer +2);
  FTBurnInTotRes *BurnInTotResPtr = (FTBurnInTotRes *)(Buffer + 4);

  *((unsigned short *)Buffer) = PerFuncTestId;
  PerFuncTestPtr->PerFuncTestCommand = FTBurnInTotResId;

  StopScheduler();

  BurnInTotResPtr->FTNrOfTests       = TotalTestResult.NrOfTests;
  BurnInTotResPtr->FTTotRomResult    = TotalTestResult.RomResult;
  BurnInTotResPtr->FTTotRamResult    = TotalTestResult.RamResult;
  BurnInTotResPtr->FTTotVC3Result    = TotalTestResult.VC3Result;
  BurnInTotResPtr->FTTotPMResult     = TotalTestResult.PMResult;
  BurnInTotResPtr->FTTotRelaisResult = TotalTestResult.RelaisResult;
  BurnInTotResPtr->FTTotUartResult   = TotalTestResult.UartResult;

  ContScheduler();

  SendMessage( ConLow, MailPtr->address_dest, 0, sizeof(Buffer), Buffer );
}

// =============================================================================
void HndlPerFuncTest(NETIO_MAIL *MailPtr)
{
  OTExtPortWriteRef      *OTExtPortWriteRefPtr;
  OTMessageRef           *OTMessageRefPtr;
  NiveauDefSetUpdateRef  *NiveauDefSetUpdateRefPtr;
  MemolacDefSetUpdateRef *MemolacDefSetUpdateRefPtr;
  VFlowDefSetUpdateRef   *VFlowDefSetUpdateRefPtr;
  SystKeyExtSwitchRef    *SystKeyExtSwitchRefPtr;
  PerFuncTest            *PerFuncTestPtr = ( PerFuncTest *)(&MailPtr->message_start);

  StopScheduler();

  switch (PerFuncTestPtr->PerFuncTestCommand)
  {

    case OTExtPortWriteRefId:
         OTExtPortWriteRefPtr = (OTExtPortWriteRef *)(&(MailPtr->message_start)+2);
         switch( OTExtPortWriteRefPtr->OTExtPort ){
           case OT_EXT_PORT_WRITE_K3: ExtPortK3State = OTExtPortWriteRefPtr->OTExtPortState; break;
           case OT_EXT_PORT_WRITE_K4: ExtPortK4State = OTExtPortWriteRefPtr->OTExtPortState; break;
           case OT_EXT_PORT_WRITE_K5: ExtPortK5State = OTExtPortWriteRefPtr->OTExtPortState; break;
         }
      break;
    case SystKeyExtSwitchRefId:
         SystKeyExtSwitchRefPtr = (SystKeyExtSwitchRef *)(&(MailPtr->message_start)+2);
         if ( SystKeyExtSwitchRefPtr->SystInternalCommand == SystKeyExtSwitchRefPtr->SystInternalCommandCheck ){
           switch ( SystKeyExtSwitchRefPtr->SystInternalCommand ){
             case SET_PULSATION_MESSAGE:
             case STOP_PULSATION_MESSAGE:
                  if ( KeyMode == KM_CLEANING ) KeyExtPulseControl = SystKeyExtSwitchRefPtr->SystInternalCommand;
                  else                          KeyExtPulseControl = 0;
               break;
             case START_I2C_WASH_MODE_MESSAGE:
             case STOP_I2C_WASH_MODE_MESSAGE:
                  if ( KeyMode == KM_CLEANING ) KeyExtSwitch = SystKeyExtSwitchRefPtr->SystInternalCommand;
               break;
             case START_CLEAN_PROCESS:
             case SET_PRE_WASH_MODE:
                  KeyExtSwitch = SystKeyExtSwitchRefPtr->SystInternalCommand;
               break;
             case STOP_CLEAN_PROCESS:
             case SET_WASH_MODE_4:
             case SET_WASH_MODE_3:
             case EXTRA_DUMP:
                  if ( KeyMode == KM_CLEANING ) KeyExtSwitch = SystKeyExtSwitchRefPtr->SystInternalCommand;
               break;
           }
         }
      break;

    case OTMessageRefId:
         OTMessageRefPtr = (OTMessageRef *)(&(MailPtr->message_start)+2);
         if ( OTMessageRefPtr->OTMessage == OTMessageRefPtr->OTMessageCheck ){
           // vul eigen adres in, zodat bericht wordt weergegeven
           OTSndMsgToSlave( EEAddress, OTMessageRefPtr->OTMessage );
         }
      break;

    case OTMasterSlaveStatusRefId:
         HndlRcvMasterState(MailPtr);
      break;

    case OTStateResId:
         HndlRcvOTSStateRes(MailPtr);
      break;

    case OTStateRefReqId:
         HndlSendOTSStateRes();
      break;
    case OTCommandRefId:
         // dit bericht moet eventueel door gestuurd worden naar een andere MPC
         HndlRcvOTCommandRef(MailPtr);
      break;
    case NiveauDefSetUpdateRefId:
         if ((MailPtr->message_length-2) != sizeof(NiveauDefSetUpdateRef)) return;

         NiveauDefSetUpdateRefPtr = (NiveauDefSetUpdateRef *)(&(MailPtr->message_start)+2);
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->Language,          &EELanguage );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->SendTime,          &EESendTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->NeutralTime1,      &EENeutralTime1 );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->NeutralTime2,      &EENeutralTime2 );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->NeutralThreshold,  &EENeutralThreshold );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->TakeOffThreshold,  &EETakeOffThreshold );
         if ( NiveauDefSetUpdateRefPtr->TakeOffTime > 0 ){
           // negatieve waardes worden niet overgenomen
           WriteEEpromShort( (unsigned short)NiveauDefSetUpdateRefPtr->TakeOffTime, (unsigned short *)&EETakeOffTime );
         }
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->MlkFlwCalcTime,    &EEMlkFlwCalcTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->OutputBlockBits,   &EEOutputBlockBits );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->SepMsgCntrl,       &EESepMsgCntrl );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->SepActFuncNr,      &EESepActFuncNr );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->SepCondFuncNr,     &EESepCondFuncNr );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->SepMinYldFuncNr,   &EESepMinYldFuncNr );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->SepTmpFuncNr,      &EESepTmpFuncNr );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->ClusterDown,       &EEClusterDown );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->ClusterDly,        &EEClusterDly );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->VacuumDly,         &EEVacuumDly );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->MilkSweepDlyTime,  &EEMilkSweepDlyTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->MilkSweepActTime,  &EEMilkSweepActTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->I2Control,         &EEI2Control );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->OutputK2,          &EEOutputK2 );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->FlowLvl2Threshold, &EEFlowLvl2Threshold );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->OutputP1Dly,       &EEOutputP1Dly );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->OutputP1Time,      &EEOutputP1Time );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->OutputP1Inv,       &EEOutputP1Inv );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->CleanStartTime,    &EECleanStartTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->CleanTime,         &EECleanTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->GateSwitchTime,    &EEGateSwitchTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->DispTime,          &EEDispTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->GoatMilkGlass,     &EEGoatMilkGlass );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->MmGoatVal1,        &EEMmGoatVal1 );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->MmGoatVal2,        &EEMmGoatVal2 );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->Pulsation,         &EEPulsation );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->PulseFrequency,    &EEPulseFrequency );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->PulseRatio,        &EEPulseRatio );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->StimPulseFreq,     &EEStimPulseFreq );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->StimPulseRatio,    &EEStimPulseRatio );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->StimTresHold,      &EEStimTresHold );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->StimStartTime,     &EEStimStartTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->StimMaxTime,       &EEStimMaxTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->NrOfPulsators,     &EENrOfPulsators );
         // pas EEFeeding alleen aan indien voeren aan of uit wordt gezet
         if ( (NiveauDefSetUpdateRefPtr->Feeding != 0) && !EEFeeding )WriteEEpromByte( 1, &EEFeeding );
         if ( (NiveauDefSetUpdateRefPtr->Feeding == 0) &&  EEFeeding )WriteEEpromByte( 0, &EEFeeding );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->RelaisTime,        &EERelaisTime );
         WriteEEpromShort( NiveauDefSetUpdateRefPtr->SB_RelaisTime,     &EESB_RelaisTime );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->MilkMeterType,     &EEMilkMeterType );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->ParlourType,       &EEParlourType );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->CentralGate,       &EEGateK3 );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->EntranceGate,      &EEGateK4 );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->ExitGate,          &EEGateK5 );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->OTRowMaster,       &EEOTRowMaster);
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->RowSize,           &EERowSize );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->ConMeasuring,      &EEConMeasuring );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->ExtConMeasuring,   &EEExtConMeasuring );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->TempMeasuring,     &EETempMeasuring );
         WriteEEpromByte(  NiveauDefSetUpdateRefPtr->I1Control,         &EEI1Control );
         RomReset();
      break;
    case MemolacDefSetUpdateRefId:
         if ((MailPtr->message_length-2) != sizeof(MemolacDefSetUpdateRef)) return;

         MemolacDefSetUpdateRefPtr = (MemolacDefSetUpdateRef *)(&(MailPtr->message_start)+2);
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->ValveTime,        &EEValveTime );
         if ( MemolacDefSetUpdateRefPtr->FillTime > 0 ){
           // negatieve waardes worden niet overgenomen
           WriteEEpromByte(  (unsigned char)MemolacDefSetUpdateRefPtr->FillTime, (unsigned char *)&EEFillTime );
         }
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->Language,         &EELanguage );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->SendTime,         &EESendTime );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->NeutralTime1,     &EENeutralTime1 );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->NeutralTime2,     &EENeutralTime2 );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->NeutralThreshold, &EENeutralThreshold );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->TakeOffThreshold, &EETakeOffThreshold );
         if ( MemolacDefSetUpdateRefPtr->TakeOffTime > 0 ){
           // negatieve waardes worden niet overgenomen
           WriteEEpromShort( (unsigned short)MemolacDefSetUpdateRefPtr->TakeOffTime,(unsigned short *)&EETakeOffTime );
         }
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->MlkFlwCalcTime,   &EEMlkFlwCalcTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->OutputBlockBits,  &EEOutputBlockBits );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->SepMsgCntrl,      &EESepMsgCntrl );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->SepActFuncNr,     &EESepActFuncNr );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->SepCondFuncNr,    &EESepCondFuncNr );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->SepMinYldFuncNr,  &EESepMinYldFuncNr );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->SepTmpFuncNr,     &EESepTmpFuncNr );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ClusterDown,      &EEClusterDown );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->ClusterDly,       &EEClusterDly );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->VacuumDly,        &EEVacuumDly );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->MilkSweepDlyTime, &EEMilkSweepDlyTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->MilkSweepActTime, &EEMilkSweepActTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->I2Control,        &EEI2Control );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->OutputK2,         &EEOutputK2 );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->FlowLvl2Threshold,&EEFlowLvl2Threshold );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->CleanStartTime,   &EECleanStartTime );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->GateSwitchTime,   &EEGateSwitchTime );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->DispTime,         &EEDispTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->MmSensorMem,      &EEMmSensorMem );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ClnPhs1,          &EEClnPhs1 );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ClnPhs2,          &EEClnPhs2 );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ClnPhs4,          &EEClnPhs4 );
         if ( MemolacDefSetUpdateRefPtr->EmptyTime > 0 ){
           // negatieve waardes worden niet overgenomen
           WriteEEpromByte(  (unsigned char)MemolacDefSetUpdateRefPtr->EmptyTime,(unsigned char *)&EEEmptyTime );
         }
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->Pulsation,        &EEPulsation );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->PulseFrequency,   &EEPulseFrequency );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->PulseRatio,       &EEPulseRatio );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->StimPulseFreq,    &EEStimPulseFreq );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->StimPulseRatio,   &EEStimPulseRatio );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->StimTresHold,     &EEStimTresHold );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->StimStartTime,    &EEStimStartTime );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->StimMaxTime,      &EEStimMaxTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->NrOfPulsators,    &EENrOfPulsators );
         // pas EEFeeding alleen aan indien voeren aan of uit wordt gezet
         if ( (MemolacDefSetUpdateRefPtr->Feeding != 0) && !EEFeeding )WriteEEpromByte( 1, &EEFeeding );
         if ( (MemolacDefSetUpdateRefPtr->Feeding == 0) &&  EEFeeding )WriteEEpromByte( 0, &EEFeeding );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->RelaisTime,       &EERelaisTime );
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->SB_RelaisTime,    &EESB_RelaisTime );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->MilkMeterType,    &EEMilkMeterType );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ParlourType,      &EEParlourType );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->CentralGate,      &EEGateK3 );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->EntranceGate,     &EEGateK4 );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ExitGate,         &EEGateK5 );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->OTRowMaster,      &EEOTRowMaster);
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->RowSize,          &EERowSize );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ConMeasuring,     &EEConMeasuring );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->ExtConMeasuring,  &EEExtConMeasuring );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->TempMeasuring,    &EETempMeasuring );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->I1Control,        &EEI1Control );
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->TrcArmOnTime,     &EETrcArmOnTime);        // tbv 'nach milch automat'
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->TrcArmOffTime,    &EETrcArmOffTime);
         WriteEEpromShort( MemolacDefSetUpdateRefPtr->TAPulseFreq,      &EETAPulseFreq);
         WriteEEpromByte(  MemolacDefSetUpdateRefPtr->TAPulseRatio,     &EETAPulseRatio);

         RomReset();
      break;

    case VFlowDefSetUpdateRefId:
         if ((MailPtr->message_length-2) != sizeof(VFlowDefSetUpdateRef)) return;

         VFlowDefSetUpdateRefPtr = (VFlowDefSetUpdateRef *)(&(MailPtr->message_start)+2);
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->Language,         &EELanguage );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->SendTime,         &EESendTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->NeutralTime1,     &EENeutralTime1 );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->NeutralTime2,     &EENeutralTime2 );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->NeutralThreshold, &EENeutralThreshold );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->TakeOffThreshold, &EETakeOffThreshold );
         if ( VFlowDefSetUpdateRefPtr->TakeOffTime > 0 ){
           // negatieve waardes worden niet overgenomen
           WriteEEpromShort( (unsigned short)VFlowDefSetUpdateRefPtr->TakeOffTime,(unsigned short *)&EETakeOffTime );
         }
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->OutputBlockBits,  &EEOutputBlockBits );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->SepMsgCntrl,      &EESepMsgCntrl );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->SepActFuncNr,     &EESepActFuncNr );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->SepCondFuncNr,    &EESepCondFuncNr );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->SepMinYldFuncNr,  &EESepMinYldFuncNr );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->SepTmpFuncNr,     &EESepTmpFuncNr );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ClusterDown,      &EEClusterDown );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->ClusterDly,       &EEClusterDly );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->VacuumDly,        &EEVacuumDly );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->MilkSweepDlyTime, &EEMilkSweepDlyTime );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->MilkSweepActTime, &EEMilkSweepActTime );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->I1Control,        &EEI1Control );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->I2Control,        &EEI2Control );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->OutputK2,         &EEOutputK2 );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->FlowLvl2Threshold,&EEFlowLvl2Threshold );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->TrcArmOnTime,     &EETrcArmOnTime );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->TrcArmOffTime,    &EETrcArmOffTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->TAPulsFreq,       &EETAPulseFreq );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->TAPulsRatio,      &EETAPulseRatio );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->CleanStartTime,   &EECleanStartTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->GateSwitchTime,   &EEGateSwitchTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->OutputP1Dly,      &EEOutputP1Dly );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->OutputP1Inv,      &EEOutputP1Inv );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->DispTime,         &EEDispTime );
         if (VFlowDefSetUpdateRefPtr->CorVFlow > 0){         // negatieve waardes worden niet overgenomen
           WriteEEpromByte(  VFlowDefSetUpdateRefPtr->CorVFlow,(unsigned char *) &EECorVFlow );
         }
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ClnPhs1,          &EEClnPhs1 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ClnPhs2,          &EEClnPhs2 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ClnPhs4,          &EEClnPhs4 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->Pulsation,        &EEPulsation );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->PulseFrequency,   &EEPulseFrequency );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->PulseRatio,       &EEPulseRatio );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->StimPulseFreq,    &EEStimPulseFreq );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->StimPulseRatio,   &EEStimPulseRatio );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->StimTresHold,     &EEStimTresHold );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->StimStartTime,    &EEStimStartTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->StimMaxTime,      &EEStimMaxTime );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->NrOfPulsators,    &EENrOfPulsators );
         // pas EEFeeding alleen aan indien voeren aan of uit wordt gezet
         if ( (VFlowDefSetUpdateRefPtr->Feeding != 0) && !EEFeeding )WriteEEpromByte( 1, &EEFeeding );
         if ( (VFlowDefSetUpdateRefPtr->Feeding == 0) &&  EEFeeding )WriteEEpromByte( 0, &EEFeeding );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->RelaisTime,       &EERelaisTime );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->SB_RelaisTime,    &EESB_RelaisTime );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->MilkMeterType,    &EEMilkMeterType );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ParlourType,      &EEParlourType );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->GateK3Time,       &EEGateK3Time );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->GateK4Time,       &EEGateK4Time );
         WriteEEpromShort( VFlowDefSetUpdateRefPtr->GateK5Time,       &EEGateK5Time );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->GateK3,           &EEGateK3 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->GateK4,           &EEGateK4 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->GateK5,           &EEGateK5 );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->OTRowMaster,      &EEOTRowMaster);
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->RowSize,          &EERowSize );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->ConMeasuring,     &EEConMeasuring );
         WriteEEpromByte(  VFlowDefSetUpdateRefPtr->TempMeasuring,    &EETempMeasuring );
         RomReset();
      break;

    case FTBurnInRefId:
      // Start testprogramma
      TestCommand = TRUE;
    break;

    case FTBurnInResReqId:
      // Geef resultaat van test terug
      HndlFTBurnInResReq(MailPtr);
    break;

    case FTBurnInTotRefId:
      // Start testprogramma, laat automatisch doorlopen
      TestCommand = PerFuncTestPtr->Array[0].PerFuncTestDataHigh;
    break;

    case FTBurnInTotResReqId:
      // Geef de totaal resultaten van test terug
      HndlFTBurnInTotResReq(MailPtr);
    break;
  }
  ContScheduler();
}

