/***************************************************************************

    osdcomm.h

    Common definitions shared by the OSD layer. This includes the most
    fundamental integral types as well as compiler-specific tweaks.

****************************************************************************

    Copyright Aaron Giles
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

        * Redistributions of source code must retain the above copyright
          notice, this list of conditions and the following disclaimer.
        * Redistributions in binary form must reproduce the above copyright
          notice, this list of conditions and the following disclaimer in
          the documentation and/or other materials provided with the
          distribution.
        * Neither the name 'MAME' nor the names of its contributors may be
          used to endorse or promote products derived from this software
          without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY AARON GILES ''AS IS'' AND ANY EXPRESS OR
    IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL AARON GILES BE LIABLE FOR ANY DIRECT,
    INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.

***************************************************************************/

#pragma once

#ifndef __OSDCOMM_H__
#define __OSDCOMM_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
#include <typeinfo>
#endif


/***************************************************************************
    COMPILER-SPECIFIC NASTINESS
***************************************************************************/

/* The Win32 port requires this constant for variable arg routines. */
#ifndef CLIB_DECL
#define CLIB_DECL
#endif


/* In C++ we can do type checking via typeid */
#ifdef __cplusplus
#define TYPES_COMPATIBLE(a,b)	(typeid(a) == typeid(b))
#endif


/* Some optimizations/warnings cleanups for GCC */
#if defined(__GNUC__) && (__GNUC__ >= 3)
#define ATTR_UNUSED				__attribute__((__unused__))
#define ATTR_NORETURN			__attribute__((noreturn))
#define ATTR_PRINTF(x,y)		__attribute__((format(printf, x, y)))
#define ATTR_MALLOC				__attribute__((malloc))
#define ATTR_PURE				__attribute__((pure))
#define ATTR_CONST				__attribute__((const))
#define ATTR_FORCE_INLINE		__attribute__((always_inline))
#define ATTR_NONNULL(...)		__attribute__((nonnull(__VA_ARGS__)))
#define UNEXPECTED(exp)			__builtin_expect(!!(exp), 0)
#define EXPECTED(exp)			__builtin_expect(!!(exp), 1)
#define RESTRICT				__restrict__
#define SETJMP_GNUC_PROTECT()	(void)__builtin_return_address(1)
#ifndef TYPES_COMPATIBLE
#define TYPES_COMPATIBLE(a,b)	__builtin_types_compatible_p(typeof(a), b)
#endif
#else
#define ATTR_UNUSED
#define ATTR_NORETURN
#define ATTR_PRINTF(x,y)
#define ATTR_MALLOC
#define ATTR_PURE
#define ATTR_CONST
#define ATTR_FORCE_INLINE
#define ATTR_NONNULL(...)
#define UNEXPECTED(exp)			(exp)
#define EXPECTED(exp)			(exp)
#define RESTRICT
#define SETJMP_GNUC_PROTECT()	do {} while (0)
#ifndef TYPES_COMPATIBLE
#define TYPES_COMPATIBLE(a,b)	1
#endif
#endif


/* And some MSVC optimizations/warnings */
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#define DECL_NORETURN			__declspec(noreturn)
#else
#define DECL_NORETURN
#endif



/***************************************************************************
    FUNDAMENTAL TYPES
***************************************************************************/

/* These types work on most modern compilers; however, OSD code can
   define their own by setting OSD_TYPES_DEFINED */

#ifndef OSD_TYPES_DEFINED

/* 8-bit values */
typedef unsigned char						UINT8;
typedef signed char 						INT8;

/* 16-bit values */
typedef unsigned short						UINT16;
typedef signed short						INT16;

/* 32-bit values */
#ifndef _WINDOWS_H
typedef unsigned int						UINT32;
typedef signed int							INT32;
#endif

/* 64-bit values */
#ifndef _WINDOWS_H
#ifdef _MSC_VER
typedef signed __int64						INT64;
typedef unsigned __int64					UINT64;
#else
__extension__ typedef unsigned long long	UINT64;
__extension__ typedef signed long long		INT64;
#endif
#endif

#endif



/***************************************************************************
    FUNDAMENTAL CONSTANTS
***************************************************************************/

/* Ensure that TRUE/FALSE are defined */
#ifndef TRUE
#define TRUE    			1
#endif

#ifndef FALSE
#define FALSE  				0
#endif



/***************************************************************************
    FUNDAMENTAL MACROS
***************************************************************************/

/* Standard MIN/MAX macros */
#ifndef MIN
#define MIN(x,y)			((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y)			((x) > (y) ? (x) : (y))
#endif


/* U64 and S64 are used to wrap long integer constants. */
#ifdef __GNUC__
#define U64(val) val##ULL
#define S64(val) val##LL
#else
#define U64(val) val
#define S64(val) val
#endif


/* Concatenate/extract 32-bit halves of 64-bit values */
#define CONCAT_64(hi,lo)	(((UINT64)(hi) << 32) | (UINT32)(lo))
#define EXTRACT_64HI(val)	((UINT32)((val) >> 32))
#define EXTRACT_64LO(val)	((UINT32)(val))


/* MINGW has adopted the MSVC formatting for 64-bit ints as of gcc 4.4 */
#if (defined(__MINGW32__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 4))) || defined(_MSVC_VER)
#define I64FMT   "I64"
#else
#define I64FMT   "ll"
#endif


/* Highly useful macro for compile-time knowledge of an array size */
#define ARRAY_LENGTH(x)		(sizeof(x) / sizeof(x[0]))


/* Macros for normalizing data into big or little endian formats */
#define FLIPENDIAN_INT16(x)	(((((UINT16) (x)) >> 8) | ((x) << 8)) & 0xffff)
#define FLIPENDIAN_INT32(x)	((((UINT32) (x)) << 24) | (((UINT32) (x)) >> 24) | \
	(( ((UINT32) (x)) & 0x0000ff00) << 8) | (( ((UINT32) (x)) & 0x00ff0000) >> 8))
#define FLIPENDIAN_INT64(x)	\
	(												\
		(((((UINT64) (x)) >> 56) & ((UINT64) 0xFF)) <<  0)	|	\
		(((((UINT64) (x)) >> 48) & ((UINT64) 0xFF)) <<  8)	|	\
		(((((UINT64) (x)) >> 40) & ((UINT64) 0xFF)) << 16)	|	\
		(((((UINT64) (x)) >> 32) & ((UINT64) 0xFF)) << 24)	|	\
		(((((UINT64) (x)) >> 24) & ((UINT64) 0xFF)) << 32)	|	\
		(((((UINT64) (x)) >> 16) & ((UINT64) 0xFF)) << 40)	|	\
		(((((UINT64) (x)) >>  8) & ((UINT64) 0xFF)) << 48)	|	\
		(((((UINT64) (x)) >>  0) & ((UINT64) 0xFF)) << 56)		\
	)

#ifdef LSB_FIRST
#define BIG_ENDIANIZE_INT16(x)		(FLIPENDIAN_INT16(x))
#define BIG_ENDIANIZE_INT32(x)		(FLIPENDIAN_INT32(x))
#define BIG_ENDIANIZE_INT64(x)		(FLIPENDIAN_INT64(x))
#define LITTLE_ENDIANIZE_INT16(x)	(x)
#define LITTLE_ENDIANIZE_INT32(x)	(x)
#define LITTLE_ENDIANIZE_INT64(x)	(x)
#else
#define BIG_ENDIANIZE_INT16(x)		(x)
#define BIG_ENDIANIZE_INT32(x)		(x)
#define BIG_ENDIANIZE_INT64(x)		(x)
#define LITTLE_ENDIANIZE_INT16(x)	(FLIPENDIAN_INT16(x))
#define LITTLE_ENDIANIZE_INT32(x)	(FLIPENDIAN_INT32(x))
#define LITTLE_ENDIANIZE_INT64(x)	(FLIPENDIAN_INT64(x))
#endif /* LSB_FIRST */

#ifndef _WIIMAME_H_
/* Wrappers */
#ifdef malloc
#undef malloc
#endif
#ifdef free
#undef free
#endif
#ifdef realloc
#undef realloc
#endif
#ifdef calloc
#undef calloc
#endif

#define malloc(a)		_wii_malloc((a),__FILE__,__LINE__)	
#define free(a)			_wii_free((a),__FILE__,__LINE__)	
#define realloc(a,b)	_wii_realloc((a),(b),__FILE__,__LINE__)
#define calloc(a,b)		_wii_calloc((a),(b),__FILE__,__LINE__)

extern void *_wii_malloc(size_t size, const char *src, int line);
extern void _wii_free(void *addr, const char *src, int line);
extern void *_wii_realloc(void *ptr, size_t size, const char *src, int line);
extern void *_wii_calloc(size_t nelem, size_t elsize, const char *src, int line);
#endif

#endif	/* __OSDCOMM_H__ */
