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
#include <new>


// --------------------------------------------------------------------------
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
// --------------------------------------------------------------------------
// optional
void my_handler() noexcept 
{
    puts("Memory allocation failed, terminating");
    exit( EXIT_FAILURE);
}
// --------------------------------------------------------------------------
// https://docs.microsoft.com/en-us/cpp/build/reference/kernel-create-kernel-mode-binary?view=vs-2019
#ifdef _DEBUG
#define DBJ_FOLLOWS_KERNEL_ADVICE
#else // _DEBUG
#undef DBJ_FOLLOWS_KERNEL_ADVICE
#endif // ! _DEBUG

#ifdef __clang__
#define NEW_DEL_NOEXCEPT
#else  // ! __clang__ 
#define NEW_DEL_NOEXCEPT noexcept
#endif // ! __clang__

// 
#ifdef DBJ_FOLLOWS_KERNEL_ADVICE
#ifdef __cpp_aligned_new
namespace my {  enum class align_val_t : size_t {}; }
#endif // __cpp_aligned_new
void* __CRTDECL operator new  ( size_t count ) NEW_DEL_NOEXCEPT {  return calloc(1, count) ;}
#ifdef __cpp_aligned_new
void* __CRTDECL operator new  ( size_t count, my::align_val_t al )  NEW_DEL_NOEXCEPT { return calloc(1, count) ; }
#endif
void __CRTDECL operator delete  ( void* ptr ) NEW_DEL_NOEXCEPT { free(ptr); }
#ifdef __cpp_aligned_new
void __CRTDECL operator delete  ( void* ptr, my::align_val_t al ) NEW_DEL_NOEXCEPT {free(ptr); }
#endif

#endif // DBJ_FOLLOWS_KERNEL_ADVICE

// --------------------------------------------------------------------------
// user code start point
extern "C" int program (int argc , char ** argv ) ;
// --------------------------------------------------------------------------
/*
Policy is very simple: 

use /kernel switch or simply do not use any /EH switch
have only one top level function as bellow to catch SE
then generate minidump if SE is raised.

Although this function bellow works always. With or without
SEH buils. As SEH is intrinsic to Windows.

*/
extern "C" inline int dbj_main (int argc, char ** argv) noexcept
{
     __try 
    { 
        __try {
        std::set_new_handler(my_handler);
        errno = 0 ;
            command_line cli(argc,argv); // currently not in use
                program(argc,argv);
        }
        __finally {
#ifdef _DEBUG
            DBJ_INFO(  ":  __finally block visited");
#endif // _DEBUG
        }
    }
    __except ( 
        GenerateDump(GetExceptionInformation())
        /* returns 1 aka EXCEPTION_EXECUTE_HANDLER */
      ) 
    { 
        // MS STL "throws" are raised SE's under /kernel builds
        // caught here and nowhere else in the app
         DBJ_ERROR(  ": Structured Exception caught");
        
        DBJ_WARN(  ": %s", 
( dump_last_run.finished_ok == TRUE ? "minidump creation succeeded" : "minidump creation failed" ) 
        );

        if ( dump_last_run.finished_ok ) {
            DBJ_INFO(  ": Dump folder: %s", dump_last_run.dump_folder_name );
            DBJ_INFO(  ": Dump file  : %s", dump_last_run.dump_file_name);
        }
    }

#ifdef _DEBUG

#ifdef _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is defined");
#else // ! _KERNEL_MODE
DBJ_INFO(  ": _KERNEL_MODE is NOT defined");
#endif // !_KERNEL_MODE


#if _HAS_EXCEPTIONS
DBJ_INFO(  ": _HAS_EXCEPTIONS == 1");
#else
DBJ_INFO(  ": _HAS_EXCEPTIONS == 0");
#endif // _HAS_EXCEPTIONS

#if _CPPRTTI 
DBJ_INFO(  ": _CPPRTTI == 1");
#else
DBJ_INFO(  ": _CPPRTTI == 0");
#endif // ! _CPPRTTI

#if _CPPUNWIND 
DBJ_INFO(  ": _CPPUNWIND == 1");
#else // ! _CPPUNWIND 
DBJ_INFO(  ": _CPPUNWIND == 0");
#endif //! _CPPUNWIND 

        DBJ_INFO( ": " DBJ_APP_NAME " " DBJ_APP_VERSION );
        DBJ_INFO(  " ...Leaving... ");
#endif // _DEBUG

        return 42;
} // main
