// (c) 2021 by dbj@dbj.org
#define UBENCH_IMPLEMENTATION
#include "../ubut/ubench.h"

UBENCH_STATE;

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int bench_program(int argc, char** argv)
{
	return ubench_main(argc, argv);
} // program


