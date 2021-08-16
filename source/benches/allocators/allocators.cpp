/// (c) 2020 by dbj.org

#include "../../../ubut/ubench.h"

#include "dbj_heap_alloc.h"
#include "../../../dbj-fwk/wallofmacros.h"

/// ---------------------------------------------------------------------
#include "kalloc/dbj_kalloc.h"

using test_array_type = double;
constexpr int test_array_size = 0xFFFFF;

/// ---------------------------------------------------------------------
static inline auto meta_driver = [](auto aloka, auto dealoka) {
	test_array_type* array_ = (test_array_type*)aloka(test_array_size);
	DBJ_ASSERT(array_);
	dealoka(array_);
};

/// compare allocators, mechatronics for

// ----------------------------------------------------------
/*
 kalloc appears to be "curiously slower" ... it really shouldn't be
 it is complex C so I had no time to dive in to investigate
*/
UBENCH(allocators, kmem)
{
	meta_driver(
		[&](size_t sze_) { (void)sze_;  return DBJ_KALLOC(test_array_type, test_array_size); },
		[&](test_array_type* array_) { DBJ_KFREE(array_); });
}

// ----------------------------------------------------------
UBENCH(allocators, straight_calloc_free)
{
	meta_driver(
		[&](size_t sze_) { (void)sze_;  return calloc(test_array_size, sizeof(test_array_type)); },
		[&](test_array_type* array_) { free(array_); });
}
// ----------------------------------------------------------
UBENCH(allocators, dbj_nano_calloc_free)
{
	meta_driver(
		[&](size_t sze_) { (void)sze_; return DBJ_CALLOC(test_array_size, test_array_type); },
		[&](test_array_type* array_) { DBJ_FREE(array_); });
}
// ----------------------------------------------------------
UBENCH(allocators, standard_new_delete)
{
	meta_driver(
		[&](size_t sze_) { (void)sze_; return new test_array_type[test_array_size]; },
		[&](test_array_type* array_) { delete[] array_; });
}
// ----------------------------------------------------------
#ifdef DBJ_USES_NEDMALLOC

/// ---------------------------------------------------------------------
/// nedmalloc primary purpose is multithreaded applications
/// it is also notoriously difficult to use in its raw form
///
#define NEDMALLOC_DEBUG 0
#undef ENABLE_LOGGING /* 0xffffffff  */
#define NEDMALLOC_TESTLOGENTRY 0
#define NO_NED_NAMESPACE
#include "nedmalloc/nedmalloc.h"


UBENCH(allocators, ned_14)
{
	meta_driver(
		[&](size_t sze_) { (void)sze_; return (test_array_type*)::nedcalloc(test_array_size, sizeof(test_array_type)); },
		[&](test_array_type* array_) { (void)sze_; ::nedfree((void*)array_); });
}
#endif // DBJ_USES_NEDMALLOC
