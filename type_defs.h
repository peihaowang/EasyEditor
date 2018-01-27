
#if !defined(TYPE_DEFS_H)
#define TYPE_DEFS_H

///////////////////////////////////////////////////////////

#if !defined(unix)
	#if defined(linux) || defined(__FreeBSD__) || defined(__APPLE__)
		#define 	unix
	#endif
#endif

///////////////////////////////////////////////////////////

#if defined(WIN32)

        #if defined(_MSC_VER)

		typedef         __int8			int8_t; //conflict in MinGW: typedef signed char int8_t
		typedef         unsigned __int8		uint8_t;
		typedef         __int16			int16_t;
		typedef         unsigned __int16	uint16_t;
		typedef         __int32			int32_t; //conflict in MinGW: typedef int int32_t
		typedef         unsigned __int32	uint32_t; //conflict in MinGW: typedef unsigned int uint32_t
		typedef         __int64			int64_t;
		typedef         unsigned __int64	uint64_t;

	#elif defined(__GNUC__)

		#include <inttypes.h> //2014.9.26 int64_t (mingw32-4.8.2)

//		typedef         signed char		int8_t;
//		typedef         unsigned char		uint8_t;
//		typedef         signed short int	int16_t;
//		typedef         unsigned short int	uint16_t;
//		typedef         signed long int		int32_t;
//		typedef         unsigned long int	uint32_t;
//		typedef         long long int		int64_t;
//		typedef         unsigned long long int	uint64_t;

        #endif

#elif defined(__linux__)

	#include <inttypes.h> //2014.9.26 introducing int64_t ...

//	typedef         signed char		int8_t;
//	typedef         unsigned char		uint8_t;
//	//typedef         signed short int	int16_t;
//	typedef         unsigned short int	uint16_t;
//	//typedef         signed long int	int32_t;
//	typedef         unsigned long int	uint32_t;
//	//typedef         signed long long int	int64_t;
//	typedef         unsigned long long int	uint64_t;

#elif defined(__APPLE__)

	#include <inttypes.h> //2014.9.26 introducing int64_t ...

//	typedef         signed char		int8_t;
//	typedef         unsigned char		uint8_t;
//	//typedef         signed int		int16_t;
//	//typedef         unsigned int		uint16_t;
//	//typedef         signed long int	int32_t;
//	//typedef         unsigned long int	uint32_t;
//	typedef         signed long long int	int64_t;
//	typedef         unsigned long long int	uint64_t;

#endif

///////////////////////////////////////////////////////////

#if defined(sparc)

	#define		_LE16(A)	( \
					(((uint16_t)(A) & 0xff00) >> 8) | \
					(((uint16_t)(A) & 0x00ff) << 8) \
					)

	#define		_LE32(A)	( \
					(((uint32_t)(A) & 0xff000000) >> 24) | \
					(((uint32_t)(A) & 0x00ff0000) >> 8)  | \
					(((uint32_t)(A) & 0x0000ff00) << 8)  | \
					(((uint32_t)(A) & 0x000000ff) << 24) \
                                        )

        #define         _LE64(A)        (\
                                        (((uint64_t)(A) & 0xff00000000000000) >> 56) |\
                                        (((uint64_t)(A) & 0x00ff000000000000) >> 40) |\
                                        (((uint64_t)(A) & 0x0000ff0000000000) >> 24) |\
                                        (((uint64_t)(A) & 0x000000ff00000000) >> 8)  |\
                                        (((uint64_t)(A) & 0x00000000ff000000) << 8)  |\
                                        (((uint64_t)(A) & 0x0000000000ff0000) << 24) |\
                                        (((uint64_t)(A) & 0x000000000000ff00) << 40) |\
                                        (((uint64_t)(A) & 0x00000000000000ff) << 56) \
                                        )

#elif defined(x86) || defined(i386) || defined(amd64) || defined(WIN32) || defined(__APPLE__)

	#define		_LE16(A)	(A)
	#define		_LE32(A)	(A)
	#define		_LE64(A)	(A)

#else

	#error "Error: byte-order not defined."

#endif

///////////////////////////////////////////////////////////

#define			_INT8_MIN		((int8_t)(1<<7))		//0x80 or -128
#define			_INT16_MIN		((int16_t)(1<<15))		//0x8000 or -32768
#define			_INT32_MIN		((int32_t)(1<<31))		//0x80000000 or -2147483648
#define			_INT64_MIN		((int64_t)(1<<63))		//0x8000000000000000 or -9223372036854775808

#define 		_UINT8_MAX		((uint8_t)( ~(uint8_t)0 ))	//0xff
#define 		_UINT16_MAX		((uint16_t)( ~(uint16_t)0 ))	//0xffff
#define 		_UINT32_MAX		((uint32_t)( ~(uint32_t)0 ))	//0xffffFFFF
#define 		_UINT64_MAX		((uint64_t)( ~(uint64_t)0 ))	//0xffffFFFFffffFFFF

#define 		_INT8_MAX		((int8_t)( _UINT8_MAX >> 1 ))	//0x7f
#define 		_INT16_MAX		((int16_t)( _UINT16_MAX >> 1 ))	//0x7fff
#define 		_INT32_MAX		((int32_t)( _UINT32_MAX >> 1 ))	//0x7fffFFFF
#define 		_INT64_MAX		((int64_t)( _UINT64_MAX >> 1 ))	//0x7fffFFFFffffFFFF

///////////////////////////////////////////////////////////

typedef		uint64_t	time64_t;

///////////////////////////////////////////////////////////

//2013.10.26 To make an integer constant of type unsigned long long int, add the suffix 'ULL' to the intege;
//http://gcc.gnu.org/onlinedocs/gcc/Long-Long.html

#if defined(__GNUC__) || defined(unix) || defined(linux) || defined(__FreeBSD__) || defined(__APPLE__)

	#define		_LL(x)			((int64_t)x##LL)
	#define		_ULL(x)			((uint64_t)x##ULL)

#elif defined(_MSC_VER)

	#define		_LL(x)			((int64_t)x##I64)
	#define		_ULL(x)			((uint64_t)x##UI64)

#endif

#endif
