
#ifndef ERROR_INCLUDED
#define ERROR_INCLUDED  1

enum zo_errors {
  PWR_RELAIS_ERR_NR = 0,
  MTR1_ERR_NR,
  MTR_GRP_ERR_NR
  };




         void SendPerGenWarningRes( signed char Side, unsigned short warning);
unsigned char CheckErrorInfo( unsigned char ErrorNumber );
         void SendPerGenErrorRes( signed char Side);
unsigned char WriteErrorInfo( unsigned char ErrorNumber );
         void ResetErrorInfoBit( unsigned char ErrorNumber );
         void ResetErrorInfo( void);

#endif
