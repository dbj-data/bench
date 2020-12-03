#pragma once
//(c) 2020 by dbj@dbj.org  -- https://dbj.org/license_dbj

//This is the fastest alloc / dealloc on Windows
//forward declared so you do not have to include windows.h

#if defined(WIN32) || defined(_WIN32)

#   define DBJ_WIN_ALLOCATE(_Sz)    ::HeapAlloc( ::GetProcessHeap() , 0, _Sz)
#   define DBJ_WIN_DEALLOCATE(_Ptr)::HeapFree(  ::GetProcessHeap() ,  0, (void*)_Ptr)

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllimport)
        __declspec(allocator)
        void*
        __stdcall
        HeapAlloc(
            void* hHeap,
            unsigned long dwFlags,
            size_t dwBytes
        );

    __declspec(dllimport)
        int
        __stdcall
        HeapFree(
            void* hHeap,
            unsigned long  dwFlags,
            void* lpMem
        );

    __declspec(dllimport)
        void*
        __stdcall
        GetProcessHeap(
            void
        );

#ifdef __cplusplus
} // extern "C" 
#endif

#else // ! WIN32

// not a windows build
// yes we use calloc, read about it
#   define DBJ_WIN_ALLOCATE(_Sz)     ::calloc(1,sz)
#   define DBJ_WIN_DEALLOCATE(_Ptr) ::free(_Ptr)

#endif // WIN32
