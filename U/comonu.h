#pragma once
// (c) dbj.org -- DBJ_LICENSE -- https://dbj.org/dbj_license 

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdint.h>

#include <ubench.h/ubench.h>

/********************************************************************************************/
/********************************************************************************************/

#if 0

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

static void last_error_message () // unicode
{
	wchar_t err[256] = {0};
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
	// wprintf(L"\n%s\n", err);
	_putws(err);
}
/********************************************************************************************/
/*
For some reason MSVC's behaviour is to warn about
including this system header. 
*/
#pragma warning(disable : 4820)
#pragma warning(push, 1)
#include <io.h>
#pragma warning(pop)

// we need to do runtime since stdout might be redirected or non existent
inline static void assure_colours(void)
{
     if (! (_isatty(_fileno(stdout))) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10))
	 {
		 _putws(L"stdout appears to be redirected to a file? Exiting");
		 exit(EXIT_FAILURE);
	 }
	// ugly! but works
	// vt100 init for win10 cmd.exe
	system(" ");
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus

#endif // 0
