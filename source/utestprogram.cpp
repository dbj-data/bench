/// ------------------------------------------------------------------------
#define UTEST_IMPLEMENTATION
#include "../ubut/utest.h"

UTEST_STATE;

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int test_program(int argc, char** argv)
{
	return utest_main(argc, argv);
} // program




