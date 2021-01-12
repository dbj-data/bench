
/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
#include "../dbj-fwk/meta.h"
#include "../dbj-fwk/win/win_cli_args.h"

#include <ubut/utest.h>

UTEST_STATE;

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int test_program(int argc, char** argv)
{
	return utest_main(argc, argv);
} // program




