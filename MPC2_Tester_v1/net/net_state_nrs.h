#ifndef NET_STATE_INCLUDED
#define NET_STATE_INCLUDED 1

#define         STxIdle          0              /* TxState  is Idle */

#define         STxFirstByte     1              /* TxState send first byte  */

#define         STxL_9_0_wait    7              /* TxState pause between byte with 9th bit = 1 */
                                                /*         and 9th bit is '0' long dataframe   */
#define         STxLSourceAddr   2              /* TxState send sourcebyte long dataframe      */
#define         STxPacketNr     30              /* TxState send packetnr long dataframe        */
#define         STxLength        3
#define         STxLData         4
#define         STxL_CRC1        5
#define         STxL_CRC2        6
#define         STxL_St_wait     7


/* Net Driver Receive  States */

#define RxReset         0                /* rest, illegal state  handled in NetRx */

#define RxSource        1                /* expect source address */
#define RxShortData     2                /* expect data byte of short frame */
#define RxShortFstCrc   3                /* expect 1rst crc-byte short frame */
#define RxShortSndCrc   4                /* expect 2nd crc-byte short frame */

#define RxLongPackNr    10               /* packetnumber long frame */
#define RxLongLength    5                /* expect length byte long frame */
#define RxLongData      6                /* expect databyte long frame */
#define RxLongDataError 7                /* expect databyte long frame with size error */
#define RxLongFstCrc    8                /* expect first crc-byte long frame */
#define RxLongSndCrc    9                /* expect 2nd crc-byte long frame      */

/* Net Driver Transmit States */

#define TxReset         0                /* reset (disable transmitter) */
#define TxDestination   1                /* token received and Tx request, send message */

#define TxSyncSpace     23               /* space between address and data byte  in synchronisation-packet */
#define TxSyncSource    24               /* sync-packet type and sourcebyte */
#define TxSyncData      25               /* Synchronisation packet, send data byte */
#define TxSyncFstCrc    26               /* synchronisation packet, send first crc byte */
#define TxSyncSndCrc    27               /* synchronisation packet, send second crc byte */
#define TxSyncWaitStat  28               /* wait until status packet (with time out) */
#define TxSyncStatAck   29               /* valid status packet received on synchronisation-packet */

#define TxLongSpace     19               /* space between address and data byte */
#define TxLongSource    2                /* long frame, send type and source address */
#define TxLongPackNr    30               /* long frame, send packetnr */
#define TxLongLength    3                /* long frame send length byte */
#define TxLongData      4                /* long frame send data bytes */
#define TxLongFstCrc    5                /* send first crc byte */
#define TxLongSndCrc    6                /* send second crc byte */

#define TxTokenUncMess  21               /* send token after unconfirmed message */
#define TxStatWait      7                /* wait until status packet (with time out) */
#define TxStatValid     8                /* valid status packet received */
#define TxStatDest      9                /* status packet, send destination address */
#define TxStatSpace     20               /* space between address and data byte */
#define TxStatSource    10               /* status packet, send source address */
#define TxStatData      11               /* status packet, send status byte */
#define TxStatFstCrc    12               /* status packet, send first crc byte */
#define TxStatSndCrc    13               /* status packet, send second crc byte */

#define TxShortSource   14               /* short frame, send source address */
#define TxShortData     15               /* short frame, send data byte */

#define TxPredFstData   16               /* predefined frame, send first data byte */
#define TxPredData      17               /* predefined frame, send data bytes */

#define TxToken         18               /* send token */

#endif
