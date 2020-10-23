#pragma once
/*
whatever /kernel or no /EHsc build needs or might need
*/
#include "meta.h"
#include "command_line.h"
#include "generate_dump.h"
#include "vt100.h"
#include "nanoclib.h"

#include <stdio.h>

// Depending on _HAS_EXCEPTIONS
// MS STL transforms to non standard  std lib
/*
<vcruntime.h> #100

#ifndef _HAS_EXCEPTIONS // Predefine as 0 to disable exceptions
    #ifdef _KERNEL_MODE
        #define _HAS_EXCEPTIONS 0
    #else
        #define _HAS_EXCEPTIONS 1
    #endif // _KERNEL_MODE
#endif // _HAS_EXCEPTIONS

*/
#ifdef DBJ_NEW_DEL_REPLACEMENTS
#include <new>
// --------------------------------------------------------------------------
// optional
void dbj_new_handler() noexcept {
  perror(DBJ_ERR_PROMPT("Memory allocation failed, terminating"));
  exit(EXIT_FAILURE);
}

inline auto init_once = []() {  std::set_new_handler(dbj_new_handler); }() ;

// --------------------------------------------------------------------------
// https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019

#ifdef __clang__
#define NEW_DEL_NOEXCEPT
#else // ! __clang__
#define NEW_DEL_NOEXCEPT noexcept
#endif // ! __clang__

//
#ifdef __cpp_aligned_new
namespace my {
enum class align_val_t : size_t {};
}
#endif // __cpp_aligned_new
void *__CRTDECL operator new(size_t count) NEW_DEL_NOEXCEPT {
  return calloc(1, count);
}
#ifdef __cpp_aligned_new
void *__CRTDECL operator new(size_t count,
                             my::align_val_t al) NEW_DEL_NOEXCEPT {
  return calloc(1, count);
}
#endif
void __CRTDECL operator delete(void *ptr)NEW_DEL_NOEXCEPT { free(ptr); }
#ifdef __cpp_aligned_new
void __CRTDECL operator delete(void *ptr, my::align_val_t al)NEW_DEL_NOEXCEPT {
  free(ptr);
}
#endif

#endif // DBJ_NEW_DEL_REPLACEMENTS


// --------------------------------------------------------------------------
// user code start point
extern "C" int program(int argc, char **argv);
// --------------------------------------------------------------------------
/*
Policy is very simple:

use /kernel switch or simply do not use any /EH switch
have only one top level function as bellow to catch SE
then generate minidump if SE is raised.

Although this function bellow works always. With or without
SEH buils. As SEH is intrinsic to Windows.

*/
extern "C" inline int dbj_main(int argc, char **argv) {
#if _CPPUNWIND
  try {
#endif
    errno = 0;
    // command_line cli(argc,argv); // currently not in use

    return program(argc, argv);
#if _CPPUNWIND
  } catch (...) {
    DBJ_ERROR("Unknown Exception!");
  }
#endif
  return 0;
} // main
