
/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
#include "dbj-fwk/meta.h"
#include "dbj-fwk/win/win_cli_args.h"

#include "ubut/ubench.h"
UBENCH_STATE();

#include "ubut/utest.h"
UTEST_STATE();

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

