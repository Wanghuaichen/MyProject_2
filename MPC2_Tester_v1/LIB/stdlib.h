/*			- STDLIB.H -

   The ANSI-defined 'standard' functions.

$Name: V3_34A $	   

*/

#ifndef _STDLIB_INCLUDED
#define _STDLIB_INCLUDED

#include "..\lib\sysmac.h"

#ifndef NULL
#define	NULL	((void*) 0 )
#endif

typedef	struct
	{
	  int	quot;
	  int	rem;
	} div_t;

typedef	struct
	{
	  long int	quot;
	  long int	rem;
	} ldiv_t;


#define RAND_MAX	32767

#define EXIT_SUCCESS	0
#define EXIT_FAILURE	1

#define MB_CUR_MAX	1

#if __TID__ & 0x8000
#pragma function=intrinsic(0)
#endif

#ifndef MEMORY_ATTRIBUTE
#define MEMORY_ATTRIBUTE
#endif

#ifndef PTR_ATTRIBUTE
#define PTR_ATTRIBUTE
#endif

MEMORY_ATTRIBUTE void	*malloc(size_t __size);

MEMORY_ATTRIBUTE void	free(void *__ptr);

MEMORY_ATTRIBUTE void	exit(int __status);

MEMORY_ATTRIBUTE void	*calloc(unsigned int __nelem, size_t __elsize);

MEMORY_ATTRIBUTE void	*realloc(void *__ptr, size_t __size);

MEMORY_ATTRIBUTE int atoi (const char *__nptr);

MEMORY_ATTRIBUTE long atol (const char *__nptr);

MEMORY_ATTRIBUTE double atof (const char *__nptr);

MEMORY_ATTRIBUTE double strtod(const char *__nptr, char **__endptr);

MEMORY_ATTRIBUTE long int strtol(const char *__nptr, char **__endptr, int __base);

MEMORY_ATTRIBUTE unsigned long int strtoul(const char *__nptr, char **__endptr, int __base);

MEMORY_ATTRIBUTE int rand(void);

MEMORY_ATTRIBUTE void srand(unsigned int __seed);

MEMORY_ATTRIBUTE void abort(void);

MEMORY_ATTRIBUTE int abs(int __j);

MEMORY_ATTRIBUTE div_t div(int __numer, int __denom);

MEMORY_ATTRIBUTE long int labs(long int __j);

MEMORY_ATTRIBUTE ldiv_t ldiv(long int __numer, long int __denom);

MEMORY_ATTRIBUTE void *bsearch(const void *__key, const void *__base,
			size_t __nmemb, size_t __size,
			int (*__compar) (const void *, const void *));

MEMORY_ATTRIBUTE void qsort(void *__base, size_t __nmemb, size_t __size,
			int (*__compar) (const void *, const void *));

#if __TID__ & 0x8000
#pragma function=default
#endif

#endif

