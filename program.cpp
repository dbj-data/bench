
#include "units/testu.h"
#include "units/test11.h"

 UBENCH_STATE();
// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
     return ubench_main(argc, argv);
}