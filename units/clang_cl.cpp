#ifdef __clang__
#include <assert.h>

#include "../ubench.h/ubench.h"
#include "../fwk/nanoclib.h"

#include <stdio.h>
  
void construct() __attribute__((constructor));
void destruct() __attribute__((destructor));

static char * data_ = NULL ;
 
UBENCH(clang_cl, sampler)
{
    UBENCH_DO_NOTHING( data_ ? strchr( data_, 'r') : 0 ) ;
}
 
static void construct( void )
{ 
    data_ = (char *)DBJ_FLT_PROMPT("data") ;
}

static void destruct( void )
{
    data_ = NULL ;
    assert( data_ );
}

#endif // __clang__