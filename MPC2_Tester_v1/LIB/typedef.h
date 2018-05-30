/*****************************************************************
** FILE        : TYPEDEF.H
** DESCRIPTION : COMMON TYPE DEFINITIONS
** AUTHOR      : W.A.A. MARKENSTEIN
** DATE        : XX-06-92
** LAST UPDATE : XX-XX-XX
** NOTES       :
*****************************************************************/

#ifndef _TYPEDEF_H
#define _TYPEDEF_H              1

#include "../lib/boolean.h"

/*-------------- type definitions --------------------*/
#define byte unsigned char
#define word unsigned int


#ifndef NULL
  #define NULL     (word)  0
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

/*-------------- EOF --------------------*/

#endif
