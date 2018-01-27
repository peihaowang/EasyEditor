#ifndef PLATFORM_SPECIFIC_H
#define PLATFORM_SPECIFIC_H

#if defined(Q_OS_WIN32)

	#define		__STDCALL2		__stdcall

	#define		_END_OF_LINE		"\r\n"

	#include <windows.h>

#else

	#define		__STDCALL2

	#define		_END_OF_LINE		"\n"

#endif

#endif // PLATFORM_SPECIFIC_H
