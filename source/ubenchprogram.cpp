
/// ------------------------------------------------------------------------
/// this goes to DBJ+FWK main.cpp
/// since it knows is it on gui or console windows app
/// 
#include "../dbj-fwk/meta.h"
#include "../dbj-fwk/win/win_cli_args.h"

#include "../ubut/ubench.h"

UBENCH_STATE;

// user code start here
// this is called from framework
// framework is where SEH is managed
extern "C" int bench_program(int argc, char** argv)
{
	return ubench_main(argc, argv);
} // program


