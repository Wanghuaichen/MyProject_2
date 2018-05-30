#pragma codeseg(CONSTPAGE0)

#include "mem.h"


// =============================================================================
unsigned char memory_cmp(unsigned char *dest_s, unsigned char *source_s, unsigned short size)
{
  if (size) {
    while ( size--) {
      if( *(dest_s+size) != *(source_s+size) ) return 1;
//      if( *dest_s++ != *source_s++ ) return 1;
    }
  }
  return 0;
}

// =============================================================================
void memory_cpy(unsigned char *dest_s, unsigned char *source_s, unsigned short size)
{
  if (size) {
    while ( size--) {
      *dest_s++ = *source_s++;
    }
  }
}

// =============================================================================
void memory_clear(unsigned char *dest_s, unsigned short size)
{
  if (size) {

    while ( size--) {
      *dest_s++ = 0;
    }
  }
}

// =============================================================================
void memory_set(unsigned char *dest_s, unsigned char value, unsigned short size)
{
  if (size) {

    while ( size--) {
      *dest_s++ = value;
    }
  }
}

// =============================================================================
void memory_move(unsigned char *dest_s, unsigned char *source_s, unsigned short size)
{
  unsigned short i = 0;

  if (size) {

    if ( (unsigned short)dest_s < (unsigned short)source_s ){
      while ( i < size ){
        *(dest_s+i) = *(source_s+i);
        i++;
      }
    }
    else {
      while ( size ) {
        --size;
        *(dest_s+size) = *(source_s+size);
      }
    }
  }
}
