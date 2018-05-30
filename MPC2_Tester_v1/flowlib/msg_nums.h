/* $Header: msg_nums.h,v 1.9 93/11/02 08:37:55 ews Rel $ */
#ifndef MSG_NUMS_INCLUDED

#define MSG_NUMS_INCLUDED 1

/*--------------------------------------------------------------------*/
/* Message group numbering                  XXXXX... ........         */
/*--------------------------------------------------------------------*/
#define PDUMMY00         0x0000          /* 00000000 00000000       0 */
#define PDUMMY01         0x0800          /* 00001000 00000000    2048 */
#define ACTIVITY         0x1000          /* 00010000 00000000    4096 */
#define PDUMMY03         0x1800          /* 00011000 00000000    6144 */
#define PDUMMY04         0x2000          /* 00100000 00000000    8192 */
#define COND_TEMP        0x2800          /* 00101000 00000000   10240 */
#define COW_HERD         0x3000          /* 00110000 00000000   12288 */
#define PDUMMY07         0x3800          /* 00111000 00000000   14336 */
#define FEED             0x4000          /* 01000000 00000000   16384 */
#define DISEASE_WEIGHT   0x4800          /* 01001000 00000000   18432 */
#define DRINK_ADDITIVE   0x5000          /* 01010000 00000000   20480 */
#define MILK             0x5800          /* 01011000 00000000   22528 */
#define MOTOR            0x6000          /* 01100000 00000000   24576 */
#define PERIPHERAL       0x6800          /* 01101000 00000000   26624 */
#define PDUMMY14         0x7000          /* 01110000 00000000   28672 */
#define SEPARATION       0x7800          /* 01111000 00000000   30720 */
#define SYSTEM           0x8000          /* 10000000 00000000   32768 */
#define PDUMMY17         0x8800          /* 10001000 00000000   34816 */
#define ROBOT            0x9000          /* 10010000 00000000   36864 */
#define MAT_ROBOT        0x9800          /* 10011000 00000000   38912 */
#define PDUMMY20         0xA000          /* 10100000 00000000   40960 */
#define PDUMMY21         0xA800          /* 10101000 00000000   43008 */
#define PDUMMY22         0xB000          /* 10110000 00000000   45056 */
#define PDUMMY23         0xB800          /* 10111000 00000000   47104 */
#define PDUMMY24         0xC000          /* 11000000 00000000   49152 */
#define PERIPH           0xC800          /* 11001000 00000000   51200 */
#define TRANSFER         0xD000          /* 11010000 00000000   53248 */
#define PDUMMY27         0xD800          /* 11011000 00000000   55296 */
#define PDUMMY28         0xE000          /* 11100000 00000000   57344 */
#define DBMS             0xE800          /* 11101000 00000000   59392 */
#define ROM_TEST         0xF000          /* 11110000 00000000   61440 */
#define DISTRIBUTOR      0xF800          /* 11111000 00000000   63488 */

#define GROUP_MASK       0xF800          /* 11111000 00000000   63488 */



/*--------------------------------------------------------------------*/
/* Message priority numbering               .....X.. .......          */
/*--------------------------------------------------------------------*/
#define LOW              0x0000          /* 00000000 00000000       0 */
#define HIGH             0x0400          /* 00000100 00000000    1024 */

#define PRIORITY_MASK    0x0400          /* 00000100 00000000    1024 */



/*--------------------------------------------------------------------*/
/* Message type numbering                   ......XX XX......         */
/*                                                                    */
/*                      Peripherals         ......00 XX......         */
/*                      PC                  ......XX XX......         */
/*                                                                    */
/*--------------------------------------------------------------------*/
#define REFREQ           0x0000          /* 00000000 00000000       0 */
#define REF              0x0040          /* 00000000 01000000      64 */

#define RESREQ           0x0080          /* 00000000 10000000     128 */
#define RES              0x00C0          /* 00000000 11000000     192 */

#define TDUMMY04         0x0100          /* 00000001 00000000     256 */

#define PCREFREQ         0x0140          /* 00000001 01000000     320 */
#define PCREF            0x0180          /* 00000001 10000000     384 */

#define PCRESREQ         0x01C0          /* 00000001 11000000     448 */
#define PCRES            0x0200          /* 00000010 00000000     512 */
#define PCRESEND         0x0240          /* 00000010 01000000     576 */

#define PCDUMPREQ        0x0280          /* 00000010 10000000     640 */
#define PCDUMP           0x02C0          /* 00000010 11000000     704 */
#define PCDUMPEND        0x0300          /* 00000011 00000000     768 */

#define PCDELETEREF      0x0340          /* 00000011 01000000     832 */

#define PCRESETREF       0x0380          /* 00000011 10000000     896 */

#define TDUMMY15         0x03C0          /* 00000011 11000000     960 */


#define TYPE_MASK        0x03C0          /* 00000011 11000000     960 */
#define PC_MASK          0x0300          /* 00000011 00000000     768 */


/*--------------------------------------------------------------------*/
/* Nedap / third-party oem                  ........ ...X....         */
/*--------------------------------------------------------------------*/
#define NEDAP            0x0000          /* 00000000 00000000       0 */
#define OEM              0x0010          /* 00000000 00010000      16 */



/*--------------------------------------------------------------------*/
/* Future use numbering                     ........ ..X.....         */
/*--------------------------------------------------------------------*/
#define FDUMMY00         0x0000          /* 00000000 00000000       0 */
#define FDUMMY04         0x0020          /* 00000000 00100000      32 */



/*--------------------------------------------------------------------*/
/* Rotation number                          ........ ....XXXX         */
/*--------------------------------------------------------------------*/





/*====================================================================*/
/* Key definitions for dedicated keyboard vcpc dbase commands         */
/*====================================================================*/

#define  KBD_CMD_KEY_NONE      0
#define  KBD_CMD_KEY_FIRST    11
#define  KBD_CMD_KEY_LAST     12
#define  KBD_CMD_KEY_NEXT     21
#define  KBD_CMD_KEY_PREV     22
#define  KBD_CMD_KEY_GET      31
#define  KBD_CMD_KEY_PUT      32
#define  KBD_CMD_KEY_CREATE   41
#define  KBD_CMD_KEY_DELETE   42


/*====================================================================*/
/* Key definitions for maximum numbers of message arrays              */
/*====================================================================*/

#define  KBD_MAX_ARRAY_ACT    10
#define  KBD_MAX_ARRAY_CON    10
#define  KBD_MAX_ARRAY_TMP    10
#define  KBD_MAX_ARRAY_FED    5
#define  KBD_MAX_ARRAY_DIS    10
#define  KBD_MAX_ARRAY_WHT    10
#define  KBD_MAX_ARRAY_DRK    10
#define  KBD_MAX_ARRAY_ADD    10
#define  KBD_MAX_ARRAY_MLK    10
#define  KBD_MAX_ARRAY_SEP    10


/*====================================================================*/
/* Key definitions for dedicated keyboard vcpc error codes            */
/*====================================================================*/

#define  KBD_ERR_NONE          0x0000
#define  KBD_ERR_NO_CMD        0x8000
#define  KBD_ERR_NO_PARS       0x4000
#define  KBD_ERR_NO_ROOM       0x2000
#define  KBD_ERR_NO_HERD       0x1000
#define  KBD_ERR_NO_GRP        0x0800
#define  KBD_ERR_NO_COW        0x0400
#define  KBD_ERR_EXIST_COWID   0x0001
#define  KBD_ERR_EXIST_COWNR   0x0002
#define  KBD_ERR_EXIST_COWRNR  0x0004
#define  KBD_ERR_WRONG_RECORD  0x0010


#endif
