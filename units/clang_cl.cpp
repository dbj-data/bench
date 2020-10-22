#include <assert.h>

#include "../utest.h/utest.h"
#include "../ubench.h/ubench.h"
#include "../fwk/nanoclib.h"

static char *data_ = NULL;

UBENCH(clang_cl, sampler) { UBENCH_DO_NOTHING(data_ ? strchr(data_, 'r') : 0); }

void construct( void ) ;
UBENCH_INITIALIZER (construct) { data_ = (char *)DBJ_FLT_PROMPT("data"); }

#ifdef __clang__
// it seems this will be visited only
// if we link with the static runtime lib
void destruct(void) __attribute__((destructor));
static void destruct(void) { data_ = NULL; }
#endif // __clang__

UTEST(dbj, utest ) {
 ASSERT_EQ(42, 42);
 ASSERT_EQ(42, 13);
}
