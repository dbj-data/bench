#ifndef UBUT_TOP_INC
#define UBUT_TOP_INC
#ifdef _MSC_VER
#pragma once
#endif  // _MSC_VER

/*
-------------------------------------------------------------------------------
#define _WIN32_WINNT_WIN10                  0x0A00 // Windows 10
set the WINVER and _WIN32_WINNT macros to the oldest supported platform 
#include <winsdkver.h>
*/

#undef WINVER
#define WINVER 0x0A00

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00

#define NOMINMAX
#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
// #include <processenv.h>

#ifdef DBJ_MINIMAX
#undef  min
#define min(x, y) ((x) < (y) ? (x) : (y))

#undef  max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // DBJ_MINIMAX

/*
-------------------------------------------------------------------------------
*/
#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

#include <crtdbg.h>
#include <errno.h>
/*
-------------------------------------------------------------------------------
UBUT stuff begins here
*/

#endif // UBUT_TOP_INC
