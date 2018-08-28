/*
** $Id: memops.h 7337 2007-08-16 03:44:29Z xgwang $
**
** Copyring (C) 2003 ~ 2007 Feynman Software.
** Copyring (C) 2001 ~ 2002 Wei Yongming.
*/

#ifndef _GAL_memops_h
#define _GAL_memops_h

/* System dependent optimized memory manipulation routines:
*/
#include <string.h>

#if defined(__GNUC__) && defined(i386)
/* Thanks to Brennan "Bas" Underwood, for the inspiration. :)
 */
#define GAL_memcpy(dst, src, len)					  \
do {									  \
	int u0, u1, u2;						  \
	__asm__ __volatile__ (						  \
		"cld\n\t"						  \
		"rep ; movsl\n\t"					  \
		"testb $2,%b4\n\t"					  \
		"je 1f\n\t"						  \
		"movsw\n"						  \
		"1:\ttestb $1,%b4\n\t"					  \
		"je 2f\n\t"						  \
		"movsb\n"						  \
		"2:"							  \
		: "=&c" (u0), "=&D" (u1), "=&S" (u2)			  \
		: "0" ((unsigned)(len)/4), "q" (len), "1" (dst),"2" (src) \
		: "memory" );						  \
} while(0)

#define GAL_memcpy4(dst, src, len)				\
do {								\
	int ecx, edi, esi;					\
	__asm__ __volatile__ (					\
		"cld\n\t"					\
		"rep ; movsl"					\
		: "=&c" (ecx), "=&D" (edi), "=&S" (esi)		\
		: "0" ((unsigned)(len)), "1" (dst), "2" (src)	\
		: "memory" );					\
} while(0)

#define GAL_revcpy(dst, src, len)			\
do {							\
	int u0, u1, u2;					\
	char *dstp = (char *)(dst);			\
	char *srcp = (char *)(src);			\
	int n = (len);					\
	if ( n >= 4 ) {					\
	__asm__ __volatile__ (				\
		"std\n\t"				\
		"rep ; movsl\n\t"			\
		: "=&c" (u0), "=&D" (u1), "=&S" (u2)	\
		: "0" (n >> 2),				\
		  "1" (dstp+(n-4)), "2" (srcp+(n-4))	\
		: "memory" );				\
	}						\
	switch (n & 3) {				\
		case 3: dstp[2] = srcp[2];		\
		case 2: dstp[1] = srcp[1];		\
		case 1: dstp[0] = srcp[0];		\
			break;				\
		default:				\
			break;				\
	}						\
} while(0)

#define GAL_memmove(dst, src, len)			\
do {							\
	if ( (dst) < (src) ) {				\
		GAL_memcpy((dst), (src), (len));	\
	} else {					\
		GAL_revcpy((dst), (src), (len));	\
	}						\
} while(0)

#define GAL_memset4(dst, val, len)				\
do {								\
	int u0, u1, u2;					\
	__asm__ __volatile__ (					\
		"cld\n\t"					\
		"rep ; stosl\n\t"				\
		: "=&D" (u0), "=&a" (u1), "=&c" (u2)		\
		: "0" (dst), "1" (val), "2" ((Uint32)(len))	\
		: "memory" );					\
} while(0)

#endif /* GNU C and x86 */

/* If there are no optimized versions, define the normal versions */
#ifndef GAL_memcpy
#define GAL_memcpy(dst, src, len)	memcpy(dst, src, len)
#endif

#ifndef GAL_memcpy4
#define GAL_memcpy4(dst, src, len)	memcpy(dst, src, (len) << 2)
#endif

#ifndef GAL_revcpy
#define GAL_revcpy(dst, src, len)	memmove(dst, src, len)
#endif

#define GAL_memset4(dst, val, len) ({	\
	register unsigned _n = (((unsigned)(len)) + 3) >> 2;		\
	register Uint32 *_p = (Uint32 *)(dst);		\
    switch (((unsigned)(len)) & 3) {				\
        case 0: do {    *_p++ = ((Uint32)(val));		\
        case 3:         *_p++ = ((Uint32)(val));		\
        case 2:         *_p++ = ((Uint32)(val));		\
        case 1:         *_p++ = ((Uint32)(val));		\
		} while ( --_n );		\
	}					\
})
#ifndef GAL_memset4
#define GAL_memset4(dst, val, len)		\
do {						\
	unsigned _count = (len);		\
	unsigned _n = (_count + 3) / 4;		\
	Uint32 *_p = (Uint32 *)(dst);		\
	Uint32 _val = (val);			\
        switch (_count % 4) {			\
        case 0: do {    *_p++ = _val;		\
        case 3:         *_p++ = _val;		\
        case 2:         *_p++ = _val;		\
        case 1:         *_p++ = _val;		\
		} while ( --_n );		\
	}					\
} while(0)
#endif

#endif /* _GAL_memops_h */

