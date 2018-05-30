/*******************************************************************/
/*                                                                 */
/*    AMICE  4.0  -  68HC11                                        */
/*                                                                 */
/*    declaration global (static) variables and constants          */
/*                                                                 */
/*******************************************************************/

#include "net_def.h"
#include "local_net.h"

#pragma codeseg(COMMON)


/**** declaration of global variables ****/

NetType        Net;
unsigned long  NetApplSendTimeout;


/**** declaration size of structure Net ****/

const short NetSize = sizeof(NetType);

/**** table to calculate token frames from node address ****/

const unsigned char TokenCode[64] =
             { 0x40, 0x01, 0x02, 0x43, 0x04, 0x45, 0x46, 0x07,
               0x08, 0x49, 0x4A, 0x0B, 0x4C, 0x0D, 0x0E, 0x4F,
               0x10, 0x51, 0x52, 0x13, 0x54, 0x15, 0x16, 0x57,
               0x58, 0x19, 0x1A, 0x5B, 0x1C, 0x5D, 0x5E, 0x1F,
               0x20, 0x61, 0x62, 0x23, 0x64, 0x25, 0x26, 0x67,
               0x68, 0x29, 0x2A, 0x6B, 0x2C, 0x6D, 0x6E, 0x2F,
               0x70, 0x31, 0x32, 0x73, 0x34, 0x75, 0x76, 0x37,
               0x38, 0x79, 0x7A, 0x3B, 0x7C, 0x3D, 0x3E, 0x7F };


/**** table to get frame priority code from command message type ****/

const unsigned char  PriorCode[10] = { 0x00,    /* dummy MsgType 1..9 */
                            0x00,    /* confirmed, low priority */
                            0x40,    /* confirmed, high priority */
                            0x00,    /* unconfirmed, low priority */
                            0x40,    /* unconfirmed, high priority */
                            0x00,    /* test, confirmed, low priority */
                            0x40,    /* test, confirmed, high priority */
                            0x00,    /* test, unconfirmed, low priority */
                            0x40,    /* test, unconfirmed, high priority */
                            0x00     /* looptest */
                            };


/**** table to get frame type code from command message type ****/

const unsigned char  TypeCode[10] = { 0x00,     /* dummy MsgType 1..9 */
                           0x00,     /* confirmed, low priority */
                           0x00,     /* confirmed, high priority */
                           0x40,     /* unconfirmed, low priority */
                           0x40,     /* unconfirmed, high priority */
                           0x00,     /* test, confirmed, low priority */
                           0x00,     /* test, confirmed, high priority */
                           0x40,     /* test, unconfirmed, low priority */
                           0x40,     /* test, unconfirmed, high priority */
                           0xC0      /* looptest */
                           };


/**** predefined Tx ID's ****/

const TxIdType  TxStsReady        = { 0,0,0,0,0,0,0 };
const TxIdType  TxStsNoBuf        = { 0,2,0,0,0,0,0 };
const TxIdType  TxStsError        = { 0,6,0,0,0,0,0 };
const TxIdType  TxStsApplTimeout  = { 0,8,0,0,0,0,0 };

unsigned char UartIntMaskStatus;
unsigned char TimerIntMaskStatus;

