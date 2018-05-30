/* $Header: dbmstype.h,v 1.3 94/02/21 15:11:27 ews Rel $ */
#ifndef DBMSTYPE_INCUDED
#define DBMSTYPE_INCUDED 1

#ifndef __TURBOC__


#endif

#define U1K unsigned char
#define U2K unsigned short
#define U4K unsigned long
#define S1K   signed char
#define S2K   signed short
#define S4K   signed long
#define DTK unsigned short
#define TMK unsigned short
#define ATK unsigned long

#define U1  unsigned char
#define U2  unsigned short
#define U4  unsigned long
#define S1    signed char
#define S2    signed short
#define S4    signed long
#define DT  unsigned short
#define TM  unsigned short
#define AT  unsigned long
#define BL  unsigned char
#define XX  unsigned char



/* deze item_type enumeratie komt voor de vcpc software uit include/dbmstype.h */
/* _BL wordt gedefinieerd als _Bl vanwege turboc conflict met register variabele _BL */

#ifdef __TURBOC__

#define _BL _Bl

enum item_type {
_U1 = 0,
_U1K,
_S1,
_S1K,
_U2,
_U2K,
_S2,
_S2K,
_U4,
_U4K,
_S4,
_S4K,
_DT,
_DTK,
_TM,
_TMK,
_AT,
_ATK,
_BL,
_BLK,
_XX
};

typedef unsigned char ITEM_TYPE;

#endif

#endif
