
#include "../ubench.h/ubench.h"

int dbj_main (int argc, char ** argv) ;

 UBENCH_STATE();

int main (int argc, char ** argv)  
{
#if defined(_WIN32_WINNT_WIN10) && (_WIN32_WINNT >= _WIN32_WINNT_WIN10)		
        // and ... this is it ... really
        // ps: make sure it is not empty string!
        system(" "); 
#else
 // no op
#endif
 return dbj_main (argc, argv) ;
}


