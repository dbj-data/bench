
#include "units/comonu.h"
#include "fwk/dbj_kernel.h"

// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
     return ubench_main(argc, argv);
}