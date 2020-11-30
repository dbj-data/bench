
/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
#include "dbj-fwk/meta.h"
#include "dbj-fwk/win/win_cli_args.h"

#include "ubut/ubench.h"

UBENCH_STATE ;


#include "ubut/utest.h"

UTEST_STATE ;

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int program(int argc, char** argv)
{
	// logic is this
	// if DBJ_CL_ARG_IGNORE_UBENCH argument is NOT given result is proceed
	// if it is given callback provided will return stop and flow will pass this
	if(app_args_result::proceed == app_args_callback_(DBJ_CL_ARG_IGNORE_UBENCH, app_args_stop ))
	{
		UBUT_INFO(" ");
		UBUT_INFO("================================================================");
		UBUT_INFO("UBENCHES");
		UBUT_INFO("================================================================");
		UBUT_INFO(" ");

		(void)ubench_main(argc, argv);
	}

	if (app_args_result::proceed == app_args_callback_(DBJ_CL_ARG_IGNORE_UTEST, app_args_stop))
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
	UBUT_INFO("%s %s DONE", DBJ_APP_NAME, DBJ_APP_VERSION);
	UBUT_INFO("================================================================");
	UBUT_INFO(" ");

	return EXIT_SUCCESS;
} // program

// this is used from DBJ+FWK if we compile it that way
// EASTL specific
#ifdef DBJ_FWK_EASTL_DIRECT_DEPENDANCY

#include <EABase/config/eacompiler.h>
#include <EASTL/internal/config.h>

extern "C" app_args_result show_eastl_compile_time_defines( void ) {

	DBJ_INFO(": ");

#if EA_COMPILER_NO_UNWIND
	DBJ_INFO(": EA_COMPILER_NO_UNWIND  == %d ", (EA_COMPILER_NO_UNWIND == 1));
#endif //! EA_COMPILER_NO_UNWIND 

#if EA_COMPILER_NO_EXCEPTIONS
	DBJ_INFO(": EA_COMPILER_NO_EXCEPTIONS  == %d ", (EA_COMPILER_NO_EXCEPTIONS == 1));
#endif //! EA_COMPILER_NO_EXCEPTIONS 

#ifdef EASTL_EXCEPTIONS_ENABLED
	DBJ_INFO(": EASTL_EXCEPTIONS_ENABLED  == %d ", (EASTL_EXCEPTIONS_ENABLED == 1));
#endif //! EASTL_EXCEPTIONS_ENABLED 

#ifdef EASTL_RTTI_ENABLED
	DBJ_INFO(": EASTL_RTTI_ENABLED  == %d ", (EASTL_RTTI_ENABLED == 1));
#endif //! EASTL_RTTI_ENABLED 

	DBJ_INFO(": ");

	return app_args_result::proceed;
}

#endif // DBJ_FWK_EASTL_DIRECT_DEPENDANCY
