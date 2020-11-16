#ifndef UBUT_TOP_INC
#define UBUT_TOP_INC

#if defined(__clang__) 
#pragma clang system_header
#endif // __clang__
/*
-------------------------------------------------------------------------------
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

#ifdef NOMINMAX
#undef  min
#define min(x, y) ((x) < (y) ? (x) : (y))

#undef  max
#define max(x, y) ((x) > (y) ? (x) : (y))
#endif // NOMINMAX

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
#include <stdbool.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <inttypes.h>


/*
    io.h contains definitions for some structures with natural padding. This is
    uninteresting, but for some reason MSVC's behaviour is to warn about
    including this system header. That *is* interesting
*/
#pragma warning(disable : 4820)
#pragma warning(push, 1)
#include <io.h>
#pragma warning(pop)
/*
-------------------------------------------------------------------------------
UBUT stuff begins here
-------------------------------------------------------------------------------
*/

// clang-cl.exe has them both defined
// thus it is not enugh to use _MSC_VER only
#ifdef _WIN32
#define UBUT_IS_WIN
#endif

/* 
clang_cl.exe is LLVM c++ compiler compiled by MSFT and included 
with VS 2019
*/
#undef UBUT_IS_CLANG_CL
#ifdef _MSC_VER
#ifdef __clang__
#define UBUT_IS_CLANG_CL 1
#endif // __clang__
#endif // _MSC_VER

#if defined(__cplusplus)
/* if we are using c++ we can use overloaded methods (its in the language) */
#define UBUT_OVERLOADABLE
#elif defined(__clang__)
/* otherwise, if we are using clang with c - use the overloadable attribute */
#define UBUT_OVERLOADABLE __attribute__((overloadable))
#endif

#ifdef __clang__
#define UBUT_UNUSED __attribute__((unused))
#else
#define UBUT_UNUSED
#endif

/*
   Disable warning about not inlining 'inline' functions.
   TODO: We'll fix this later by not using fprintf within our macros, and
   instead use snprintf to a realloc'ed buffer.
*/
#pragma warning(disable : 4710)

/*
   Disable warning about inlining functions that are not marked 'inline'.
   TODO: add a UBUT_NOINLINE onto the macro generated functions to fix this.
*/
#pragma warning(disable : 4711)
#pragma warning(push, 1)

typedef __int64 ubut_int64_t;
typedef unsigned __int64 ubut_uint64_t;

#pragma warning(pop)

#if defined(__cplusplus)
#define UBUT_C_FUNC extern "C"
#else
#define UBUT_C_FUNC
#endif

#if defined(__cplusplus) && (__cplusplus >= 201103L)
#define UBUT_NOEXCEPT noexcept
#else
#define UBUT_NOEXCEPT
#endif

#ifdef UBUT_IS_CLANG_CL
#define UBUT_NOTHROW __declspec(nothrow)
#else
#define UBUT_NOTHROW
#endif

#define UBUT_PRId64 "I64d"
#define UBUT_PRIu64 "I64u"
// #define UBUT_INLINE __forceinline
#define UBUT_NOINLINE __declspec(noinline)
#define UBUT_FORCEINLINE __forceinline

#if defined(_WIN64)
#define UBUT_SYMBOL_PREFIX
#else
#define UBUT_SYMBOL_PREFIX "_"
#endif

#pragma section(".CRT$XCU", read)
#define UBUT_INITIALIZER(f)                                                  \
  static void __cdecl f(void);                                                 \
  __pragma(comment(linker, "/include:" UBUT_SYMBOL_PREFIX #f "_"));          \
  UBUT_C_FUNC __declspec(allocate(".CRT$XCU")) void(__cdecl * f##_)(void) =  \
      f;                                                                       \
  static void __cdecl f(void)

// clang on win aka clang-cl.exe
#ifdef __clang__
#undef UBUT_INITIALIZER
#define UBUT_INITIALIZER(f)                                                  \
  static void f(void) __attribute__((constructor));                            \
  static void f(void)
#endif // __clang__

#define __STDC_FORMAT_MACROS 1

#if defined(__clang__) && defined( __has_warning )
#if __has_warning("-Wreserved-id-macro")
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#pragma clang diagnostic pop
#endif
#endif // clang has warning

#if defined(__cplusplus)
#define UBUT_CAST(type, x) static_cast<type>(x)
#define UBUT_PTR_CAST(type, x) reinterpret_cast<type>(x)
#define UBUT_EXTERN extern "C"
#define UBUT_NULL NULL
#else
#define UBUT_CAST(type, x) ((type)x)
#define UBUT_PTR_CAST(type, x) ((type)x)
#define UBUT_EXTERN extern
#define UBUT_NULL 0
#endif

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvariadic-macros"
#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
#endif

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//#ifdef UBUT_IS_WIN
#define UBUT_SNPRINTF(BUFFER, N, ...) _snprintf_s(BUFFER, N, N, __VA_ARGS__)
//#else
//#ifdef __clang__
//#pragma clang diagnostic push
//#pragma clang diagnostic ignored "-Wvariadic-macros"
//#pragma clang diagnostic ignored "-Wc++98-compat-pedantic"
//#endif
//#define UBUT_SNPRINTF(...) snprintf(__VA_ARGS__)
//#ifdef __clang__
//#pragma clang diagnostic pop
//#endif
//#endif

static UBUT_FORCEINLINE int ubut_strncmp
(const char *a, const char *b, size_t n) 
{
  /* strncmp breaks on Wall / Werror on gcc/clang, so we avoid using it */
  unsigned i;

  for (i = 0; i < n; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (a[i] > b[i]) {
      return 1;
    }
  }

  return 0;
}

static UBUT_FORCEINLINE FILE * ubut_fopen(const char *filename,
                                        const char *mode) {
#ifdef UBUT_IS_WIN
  FILE *file;
  if (0 == fopen_s(&file, filename, mode)) {
    return file;
  } else {
    return 0;
  }
#else
  return fopen(filename, mode);
#endif
}


static UBUT_FORCEINLINE ubut_int64_t ubut_ns(void) {
    //#ifdef UBENCH_IS_WIN
    LARGE_INTEGER counter;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&counter);
    QueryPerformanceFrequency(&frequency);
    return UBUT_CAST(ubut_int64_t,
        (counter.QuadPart * 1000000000) / frequency.QuadPart);
}

/*
-------------------------------------------------------------------------------
CAUTION: This is compile time. 

Meaning: you compile and run on W10 and all is fine.
But, you run the same exe on W7, and squigly bits will be shown instead of colours

If you check at runtime and stop if running bellow W10, in that respect you will be ok.
-------------------------------------------------------------------------------
*/

#if (_WIN32_WINNT == 0x0A00)
#define UBUT_VT_ESC "\x1b["
#define UBUT_VT_RESET UBUT_VT_ESC "0m"
#define UBUT_VT_GRAY UBUT_VT_ESC "90m"
#define UBUT_VT_BLUE UBUT_VT_ESC "94m"
#define UBUT_VT_CYAN UBUT_VT_ESC "36m"
#define UBUT_VT_YELLOW UBUT_VT_ESC "33m"
#define UBUT_VT_GREEN UBUT_VT_ESC "32m"
#define UBUT_VT_RED UBUT_VT_ESC "31m"
#define UBUT_VT_MAGENTA UBUT_VT_ESC "35m"
#else // ! WIN10
#define UBUT_VT_ESC ""
#define UBUT_VT_RESET UBUT_VT_ESC ""
#define UBUT_VT_GRAY UBUT_VT_ESC ""
#define UBUT_VT_BLUE UBUT_VT_ESC ""
#define UBUT_VT_CYAN UBUT_VT_ESC ""
#define UBUT_VT_YELLOW UBUT_VT_ESC ""
#define UBUT_VT_GREEN UBUT_VT_ESC ""
#define UBUT_VT_RED UBUT_VT_ESC ""
#define UBUT_VT_MAGENTA UBUT_VT_ESC ""
#endif

/*
-------------------------------------------------------------------------------
EOF
-------------------------------------------------------------------------------
*/
#endif // UBUT_TOP_INC
