/// ------------------------------------------------------------------------
/// obviously this whole file will goes into the DBJ+FWK too
/// 

/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
/// the mandatory and only initialization of the dbj simplelog
#include "dbj--simplelog/dbj_simple_log_host.h"
#ifdef _DEBUG
int dbj_simple_log_setup_ = ( DBJ_LOG_MT /*| DBJ_LOG_TESTING*/ ) ;
#else // ! _DEBUG
int dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_WITH_CONSOLE);
#endif // _DEBUG


// we can NOT mix utest and ubench 
//#ifdef DBJ_USE_UBENCH
//#ifdef DBJ_USE_UTEST
//#error Currently can not accomodate UBENCH and UTEST in the same program
//#endif 
//#endif 

// we will conditionaly compile for one or the other

#ifdef DBJ_USE_UBENCH
#include "ubut/ubench.h"
UBENCH_STATE();
#endif

#ifdef DBJ_USE_UTEST
#include "ubut/utest.h"
UTEST_STATE();
#endif

bool are_we_on_required_os(void);

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int program(int argc, char** argv)
{
#ifdef DBJ_USE_UBENCH

	UBUT_INFO(" ");
	UBUT_INFO("================================================================");
	UBUT_INFO("UBENCHES");
	UBUT_INFO("================================================================");
	UBUT_INFO(" ");

	(void)ubench_main(argc, argv);
#endif

#ifdef DBJ_USE_UTEST
	UBUT_INFO(" ");
	UBUT_INFO("================================================================");
	UBUT_INFO("UTESTS");
	UBUT_INFO("================================================================");
	UBUT_INFO(" ");

	(void)utest_main(argc, argv);
#endif

	UBUT_INFO(" ");
	UBUT_INFO("================================================================");
	UBUT_INFO("PROGRAM DONE");
	UBUT_INFO("================================================================");
	UBUT_INFO(" ");

	return EXIT_SUCCESS;
}
/// ------------------------------------------------------------------------
/// 
#include "dbj-fwk/win/windows_includer.h"
static bool are_we_on_required_os(void) {
// if the Windows version is equal to or
// greater than 10.0.14393 then ENABLE_VIRTUAL_TERMINAL_PROCESSING is
// supported.
// That is: cmd is capable of handling VT100 colour codes
	return is_win_ver_or_greater(10, 0, 14393);
}

#ifdef DBJ_AD_HOC_
#include <malloc.h>
/*
MSFT "extension" _malloca() must be paired with _freea()
*/
// Variable Length Static ARRay
#define VLS_ARR(T_, S_) (T_*)_alloca(S_ * sizeof(T_))
// Variable Length Static ARRay Pointer
#define VLS_ARP(T_) (T_)_alloca(sizeof(T_))

void function(int size_)
{
	// no need to free + can use runtime length
	// CAUTION! this is not immune to stack overflow
	// see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/resetstkoflw?view=msvc-160

#ifdef __clang__
	typedef char(*ptr_to_char_arr)[size_];
#else //! __clang__
	typedef char(*ptr_to_char_arr)[0xFF];
#endif // ! __clang__

	ptr_to_char_arr arrp  = VLS_ARP(ptr_to_char_arr);
	(*arrp)[size_ / 2] = '!';

	char* local_buf = VLS_ARR(char, size_);

	local_buf[size_ / 2] = '!';

	// _Static_assert( local_buf != NULL ) ;
}

#endif // DBJ_AD_HOC_