
#if defined(DBJ_FWK_USES_SIMPLELOG)

/// default is log to file, MT resilient, no console
/// unless you define your combination differently that is
/// as we did here
#define  DBJ_LOG_DEFAULT_SETUP DBJ_LOG_DEFAULT_WITH_CONSOLE
#include "dbj--simplelog/dbj_simple_log_host.h"

#ifndef __clang__

struct simple_log_finalizer final {
	~simple_log_finalizer() {
		_ASSERTE(dbj_log_finalize() == EXIT_SUCCESS);
	}
};
#endif // ! __clang__

#endif // ! defined(DBJ_FWK_USES_SIMPLELOG)

// we can NOT mix utest and ubench 
#ifdef DBJ_USE_UBENCH
#ifdef DBJ_USE_UTEST
#error Currently can not accomodate UBENCH and UTEST in the same program
#endif 
#endif 

// we will conditionaly compile for one or the other

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
extern "C" int program(int argc, char** argv)
{
#ifdef DBJ_USE_UBENCH
	return ubench_main(argc, argv);
#elif DBJ_USE_UTEST
	return utest_main(argc, argv);
#else
#error "Neither DBJ_USE_UBENCH nor DBJ_USE_UTEST, are defined?"
#endif
}