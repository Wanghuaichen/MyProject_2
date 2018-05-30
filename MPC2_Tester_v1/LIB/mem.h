#ifndef MEM_INCLUDED
#define MEM_INCLUDED 1

// routines tbv. vaste ram -----------------------------------------------------
unsigned char  memory_cmp(  unsigned char *dest_s, unsigned char *source_s, unsigned short size);
void           memory_cpy(  unsigned char *dest_s, unsigned char *source_s, unsigned short size);
void           memory_move( unsigned char *dest_s, unsigned char *source_s, unsigned short size);
void           memory_set(  unsigned char *dest_s, unsigned char  value,    unsigned short size);
void           memory_clear(unsigned char *dest_s, unsigned short size);


#endif
