#pragma codeseg(EEPAGE)

#include "../init/start.h"
#include "../lib/iic.h"
#include "../lib/time.h"
#include "../key/keyint.h" 
#include "../lib/convert.h"
#include "../usx/fake_usx.h"
#include "input.h"  
#include "key.h"

     
// -------------- function -----------------------------------------------------
// Task: copies one string into another string.                      
// pre:  p_dest and p_source must be '\0' terminated strings.        
// post: p_dest contains p_source.                                   
// -----------------------------------------------------------------------------
unsigned char *string_cpy (unsigned char *p_dest, unsigned char *p_source)
{
  unsigned char *p_cpy_dest, *p_cpy_source;

   // equalise pointers and copy until termination 
   for (p_cpy_dest = p_dest, p_cpy_source = p_source; *p_cpy_source;)
        *p_cpy_dest++ = *p_cpy_source++;

   // add string termination 
   *p_cpy_dest = '\0';

   return (p_dest);
}

// -----------------------------------------------------------------------------
// Task: compares n character from one string with another string.     
// pre:  p_dest and p_source must be terminated strings.               
// post: returns 0 if equal, returns 1 if greater, returns -1 if les   
// -----------------------------------------------------------------------------
int stringn_cmp (char *p_dest, char *p_source, int count)
{
char *pl_dest, *pl_source;

  /* equalise pointers and copy until termination */
  for (pl_dest = p_dest, pl_source = p_source; count; pl_source++, pl_dest++, count--)
  {
    if      ( *pl_dest < *pl_source ) return -1;
    else if ( *pl_dest > *pl_source ) return  1;
  }

  return 0;
}

// -----------------------------------------------------------------------------
// Task: converts a string to a signed long                       
// in  : p_source must be '\0' terminated strings.                
//       length of string                                         
// out : long value                                               
// -----------------------------------------------------------------------------
void str2long(unsigned char length, char *str, long *value)
{
  signed char       sign = 1;
  unsigned char     i;

  *value = 0;

  for( i = 0; i < length; i++ ){
    if( *(str+i) == '-' ){ sign = -1; continue;}
    if( *(str+i) == '+' )continue;
    if( *(str+i) == '.' )continue;
    if( *(str+i) == 0 )break;
    *value = ( *value * 10 ) + ( *(str+i) - '0' );
  }
  if(sign == -1) *value = -1 * *value;
}


// =============================================================================
// return value: 0 = item not changed.
//               1 = item changed.
// =============================================================================
unsigned char GetNString(unsigned char len, unsigned char spos, unsigned char *str, unsigned char nd)
{
  unsigned short DispFlashOld = DispFlash;
  unsigned short DispStatus = 0, s = 0;
  unsigned char  retval = 0;
  unsigned char  quit = 0;
  unsigned char  KeyOld = KEY_CLEAR, i;
    signed char  digit_ptr = 0, d = 0; 
  unsigned short point = 0;   
  unsigned char  sbuf[10];
  unsigned short time;   

  // schrijf oude waarde naar scherm, zonder uit te lijnen 
  for ( i = 0, digit_ptr = 0; digit_ptr < len ; digit_ptr++, i++){
    str[digit_ptr] = '\0';
    sbuf[i] = '_';
  } 

  HandleWriteIic( KB_ADDRESS, spos, i, sbuf);
  s = 0x0001 << (10 - len - spos);
  for ( i = 0; i < len; i++ ){
    DispStatus |= s; 
    s = s << 1;
  }

  HandleWriteIic(KB_ADDRESS, IIC_WRITE_FLASH_INFO, 2, (unsigned char *)&DispStatus);
  HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&point );

  digit_ptr = 0;
  time = Cnt10msec;

  do{
    // sluit einde string altijd af met een '\0'
    str[digit_ptr+1] = '\0';

    ReadKey();
    if ((KeyA != KeyOld) && (KeyA != 0)) {

      switch( KeyOld = KeyA ){
        case '0': 
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
             // laatste digit ingevuld? 
             if ( digit_ptr >= ( len + (d == 0 ? 0 : 1)) ) {
               digit_ptr = d = 0;
             }
             str[digit_ptr] = (char)KeyA;    
             HandleWriteIic( KB_ADDRESS, ((spos+digit_ptr)-(d == 0?0:1)), 1, &(str[digit_ptr]) );
             digit_ptr++;

             // verhoog aantal decimalen indien er een punt is ingevoegd 
             if ( d ) d++;
             retval = 1;
          break;

        case KEY_ENTER:
             // minder decimalen ingevuld dan max. vul string op met nullen 
             if(d) d -=1;  //correction nr of decimals, d wordt al 1 d.m.v '.'
             while(d < nd ){ str[digit_ptr] = '0'; digit_ptr++ ; d++; }
             // als meer decimalen ingevuld zijn dan toegestaan, kort string in
             if( d > nd ) len = digit_ptr - (d - nd); 
             else         len = digit_ptr; 
             // als er gwijzigd is, geef dan lengte mee terug 
             if ( retval ) retval = len;

             quit = 1;
          break;
        case KEY_CLEAR:     
             // zijn er al decimalen achter de komma ingevoerd (d==0?) 
             if ( digit_ptr ){
               if (d == 0){
                 // nee, voeg punt toe 
                 str[digit_ptr] = '.';
                 d++;
                 // 10 digits, start van af rechter digit (waarde is gespiegeld) 
                 point = 0x0200 >> (10 - (spos+digit_ptr) );

                 if (digit_ptr <  len) digit_ptr++;
                 HandleWriteIic(KB_ADDRESS, IIC_WRITE_POINT_INFO, 2, (unsigned char *)&point );
               } else {
                 // twee maal de 'clear' toets gedrukt 
                 retval = 0;
                 quit =1;
               } 
             } else {
               // openen met clear levert een min teken op
               str[digit_ptr] = '-';
               HandleWriteIic( KB_ADDRESS, ((spos+digit_ptr)-(d == 0?0:1)), 1, &(str[digit_ptr]) );
               digit_ptr++;
             }	
          break;
        case KEY_REL:
             KeyOld = 0;    
          break; 
        case KEY_SHIFT:
             retval = 0;
             quit =1;
          break; 
 
        default:
          break; 
       }    
    }
    // laat een veld niet langer dan 10 seconden open staan
    if ( TimeControl(Cnt10msec, time + 1000)){
      // veld te lang geopend
      retval = 0; 
      quit =1;
    }    
  }while( quit == 0 );

  HandleWriteIic(KB_ADDRESS, IIC_WRITE_FLASH_INFO, 2, (unsigned char *)&DispFlashOld);
  return retval;
}


// =============================================================================
// return value: 0 = item not changed.
//               1 = item changed.
// =============================================================================
unsigned char GetNValue(unsigned char len, unsigned char nd, unsigned char spos, long *value)
{
  unsigned char  buf[11], new_len;

  // initialisatie buffer, GetAString plaatst dan spaties. 
  buf[0] = '\0';

  new_len = GetNString( len, spos, &(buf[0]), nd);
  if ( new_len  ){
    // converteer string naar een long
    str2long(new_len, (char *)buf, value);
    return 1;
  }
  else return 0;
}



