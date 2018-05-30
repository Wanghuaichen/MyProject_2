#include "../lib/eeprom.h"
#include "../lib/eehc11.h"
#include "../init/start.h"
#include "../comm/send_msg.h"
#include "../appl/milkmisc.h"
#include "../flowlib/peripheral_vcp_flow.h"
#include "error.h"

 // =============================================================================
unsigned char CheckErrorInfo( unsigned char ErrorNumber )
{
  if ( ErrorNumber < 12 ){
    return (  ErrorInfo & ( 0x0001 << ErrorNumber ) );
  }
  else return 0;
}


// =============================================================================
unsigned char WriteErrorInfo( unsigned char ErrorNumber )
{
  unsigned short ErrorState;
  unsigned char  r_val =0;

  if ( ErrorNumber < 12 ){
    // voor de betekenis van de diverse bits zie error.h  ----------------------
    ErrorState = ErrorInfo | ( 0x0001 << ErrorNumber );
    r_val = WriteEEpromShort( ErrorState, &ErrorInfo);
  }
  return r_val;
}


// =============================================================================
void ResetErrorInfoBit( unsigned char ErrorNumber )
{
  unsigned short ErrorState;

  if ( ErrorNumber < 12 ){
    ErrorState =  ErrorInfo & (0xff - ( 0x01 << ErrorNumber ));
    WriteEEpromShort( ErrorState, &ErrorInfo);
  }
}


// =============================================================================
void ResetErrorInfo( void)
{
  WriteEEpromShort( 0, ( unsigned short *)( &ErrorInfo));
}


// =============================================================================
void SendPerGenErrorRes( signed char Side)
{

  unsigned char  Buffer[MESS_LEN(PerGenErrorRes)];
  unsigned short length;
    signed char  SrcNode;
  PerGenErrorRes  *p_PerGenErrorRes = (PerGenErrorRes *)(Buffer + 2);

  *((unsigned short *)Buffer) = PerGenErrorResId;

  length = MESS_LEN(PerGenErrorRes);
  p_PerGenErrorRes->PerGenNumber      = PerGenNumber[Side];
  p_PerGenErrorRes->PerGenErrorInfo   = ErrorInfo;
  p_PerGenErrorRes->PerGenWarningInfo = 0;

  SrcNode = GetNodeAddress(Side);
  SendMessage( ConLow, SrcNode, 0, length, Buffer);
}

