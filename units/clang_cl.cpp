
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

// it seems this will be visited only 
// if we link with the static runtime lib
static void destruct( void )
{
    data_ = NULL ;
}

#endif // __clang__

void before_main ( void ) ;

// #pragma clang diagnostic push
// #pragma clang diagnostic ignored "-W#pragma-messages"

// compiled with clang-cl on WIN
// clang-cl is part of Visual Studio 2019

// _MSC_VER and __clang__ are defined
#ifdef _MSC_VER
#pragma message "_MSC_VER defined"
#endif // _MSC_VER

#ifdef __clang__
#pragma message "__clang__ defined"
#endif // __clang__

// #pragma clang diagnostic pop

// this is cl.exe initializer, not clang-cl.exe initializer
UBENCH_INITIALIZER( before_main )
{
    data_ = (char *)"when is this?" ;

}