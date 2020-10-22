// #include "../ubench.h/ubench.h"
#include "dbj_main.h"

extern "C" int main (int argc, char ** argv)  
{
#if defined(_WIN32_WINNT_WIN10) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10)		
        // and ... this is it ... really
        // ps: make sure it is not empty string!
        system(" "); 
#else
 // no op
#endif
      __try 
    { 
        __try {
                dbj_main(argc,argv);
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

return EXIT_SUCCESS ;

}


