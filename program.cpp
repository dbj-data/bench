
// we can NOT mix utest and ubench 
#ifdef DBJ_USE_UBENCH
#ifdef DBJ_USE_UTEST
#error Currently can not accomodate UBENCH and UTEST in the same program
#endif 
#endif 

// we will conidtionaly compile for one or the other

#ifdef DBJ_USE_UBENCH
#include "ubench/ubench.h"
 UBENCH_STATE();
#elif DBJ_USE_UTEST
#include "utest.h/utest.h"
 UTEST_STATE();
#else 
#error "Neither DBJ_USE_UBENCH nor DBJ_USE_UTEST, are defined?"
#endif

// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
#ifdef DBJ_USE_UBENCH
     return ubench_main(argc, argv);
#elif DBJ_USE_UTEST
     return utest_main(argc, argv);
#else
#error "Neither DBJ_USE_UBENCH nor DBJ_USE_UTEST, are defined?"
#endif
}