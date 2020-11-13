
/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
#include "dbj-fwk/meta.h"
#include "dbj-fwk/win/win_cli_args.h"

/// the mandatory and only initialization of the dbj simplelog
#include "dbj--simplelog/dbj_simple_log_host.h"
#ifdef _DEBUG
int dbj_simple_log_setup_ = ( DBJ_LOG_MT ) ;
#else // ! _DEBUG
int dbj_simple_log_setup_ = (DBJ_LOG_DEFAULT_WITH_CONSOLE);
#endif // _DEBUG

#include "ubut/ubench.h"
UBENCH_STATE();

#include "ubut/utest.h"
UTEST_STATE();

bool are_we_on_required_os(void);

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int program(int argc, char** argv)
{
	if(!app_args_callback_(DBJ_CL_ARG_IGNORE_UBENCH, nullptr ))
	{
		UBUT_INFO(" ");
		UBUT_INFO("================================================================");
		UBUT_INFO("UBENCHES");
		UBUT_INFO("================================================================");
		UBUT_INFO(" ");

		(void)ubench_main(argc, argv);
	}

	if (!app_args_callback_(DBJ_CL_ARG_IGNORE_UTEST, nullptr))
	{
		UBUT_INFO(" ");
		UBUT_INFO("================================================================");
		UBUT_INFO("UTESTS");
		UBUT_INFO("================================================================");
		UBUT_INFO(" ");

		(void)utest_main(argc, argv);
	}

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