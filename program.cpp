
#include "ubench.h/ubench.h"
#include "utest.h/utest.h"

 UBENCH_STATE();
 UTEST_STATE();
// user code start here
// this is called from framework
// framework is where SE handling is implemented
extern "C" int program (int argc , char ** argv ) 
{
            utest_main(argc, argv);
     return ubench_main(argc, argv);
}