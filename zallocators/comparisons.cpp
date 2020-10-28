/// (c) 2020 by dbj.org

#include "../dbj-fwk/nanoclib.h"
#include "dbj_heap_alloc.h"

#include <time.h>

#ifdef DBJ_USE_UTEST
// #include "../utest/utest.h"
#endif

#ifdef DBJ_USE_UBENCH
#include "../ubench/ubench.h"
#endif

/// ---------------------------------------------------------------------

#include "kalloc/dbj_kalloc.h"

#if _HAS_EXCEPTIONS
// nvwa is a law obiding c++ citizen
#include "nvwa/fixed_mem_pool.h"
#include "nvwa/static_mem_pool.h"
#endif // _HAS_EXCEPTIONS

//#include "shoshnikov_pool_allocator/shoshnikov_pool_allocator.h"
// #include "dbj_pool_allocator/dbj_shoshnikov_pool_allocator.h"
// #include "dbj_pool_allocator/pool_allocator_sampling.h"
/// ---------------------------------------------------------------------
/// nedmalloc primary purpose is multithreaded applications
/// it is also notoriously difficult to use in its raw form
///
#define DBJ_USES_NEDMALLOC
#ifdef DBJ_USES_NEDMALLOC
#define NEDMALLOC_DEBUG 0
#undef ENABLE_LOGGING /* 0xffffffff  */
#define NEDMALLOC_TESTLOGENTRY 0
#define NO_NED_NAMESPACE
#include "nedmalloc/nedmalloc.h"
#endif // DBJ_USES_NEDMALLOC

/// ---------------------------------------------------------------------

using test_array_type = double ;
constexpr int test_array_size = 0xFFFFF; 

/// ---------------------------------------------------------------------
inline auto randomizer = [](int max_ = 0xFF, int min_ = 1) -> test_array_type {
	static auto _ = [] {
		srand((unsigned)time(NULL));
		return true;
	}();

	return test_array_type(rand() % max_ + min_);
};
/// ---------------------------------------------------------------------
inline auto meta_driver = [](auto aloka, auto dealoka) {
	test_array_type *array_ = (test_array_type *)aloka(test_array_size);
	DBJ_ASSERT(array_);
	// for (int k = 0; k < (test_array_size / 4); ++k)
	// {
	// 	array_[k] = randomizer();
	// }
	dealoka(array_);
};

/// compare memory mechatronics

// ----------------------------------------------------------
/*
 kalloc appears to be "curiously slower" ... it really shouldn't be
 it is complex C so I had no time to dive in to investigate
*/
UBENCH(allocators, kmem)
{
	meta_driver(
		[&](size_t sze_) { return DBJ_KALLOC(test_array_type, test_array_size); },
		[&](test_array_type *array_) { DBJ_KFREE(array_); });
}

#if _HAS_EXCEPTIONS
// ----------------------------------------------------------
UBENCH(allocators, static_nvwa_pool)
{
	using nvwa_pool = nvwa::static_mem_pool<test_array_size>;

	meta_driver(
		[&](size_t sze_) { return (test_array_type *)nvwa_pool::instance_known().allocate(); },
		[&](test_array_type *array_) { nvwa_pool::instance_known().deallocate(array_); });
}
// ----------------------------------------------------------
UBENCH(allocators, fixed_mem_pool)
{
	using nvwa_pool = nvwa::fixed_mem_pool<test_array_type>;
	static nvwa_pool nvwa;
	nvwa_pool::initialize(test_array_size);
	_ASSERTE(true == nvwa.is_initialized());

	meta_driver(
		[&](size_t sze_) { return (test_array_type *)nvwa.allocate(); },
		[&](test_array_type *array_) { nvwa.deallocate(array_); });
	nvwa_pool::deinitialize();
}
#endif // _HAS_EXCEPTIONS                                            \
	   // ---------------------------------------------------------- \
	   // mental note: take this out of nanolib !
#if 0		
		/*
		this allocator does not free memory taken from OS.
		in this scenario it will allocate a block size = 4 * test_array_size
		4 * 40 * 1000000 = just above 15MB
		which is a lot of heap reserved by one function
		That makes it kind-of-a difficult for other to cohabit with it ...

		I have changed it only to use HeapAlloc / HeapFree
		*/
		{
			static dbj::nanolib::PoolAllocator  tpa(4 /* chunks per block */);

			meta_driver(
				[&](size_t sze_) { return  (int*)tpa.allocate(test_array_size); },
				[&](int* array_) { tpa.deallocate((void*)array_); }
			);
		}
#endif // 0 \
	// ----------------------------------------------------------
#if 0
		UBENCH( allocators, dbj_pool_allocator )
		{
			static dbj::shohnikov::dbj_pool_allocator  dbj_pool(
				dbj::shohnikov::legal_block_size::_4
				, test_array_size
			);

			meta_driver(
				[&](size_t sze_) { return  (int*)dbj_pool.allocate(); },
				[&](int* array_) { dbj_pool.deallocate((void*)array_); }
			);
		}
#endif // 0
// ----------------------------------------------------------
UBENCH(allocators, straight_calloc_free)
{
	meta_driver(
		[&](size_t sze_) { return calloc(test_array_size, sizeof(test_array_type) ); },
		[&](test_array_type *array_) { free(array_); });
}
// ----------------------------------------------------------
UBENCH(allocators, dbj_nano_calloc_free)
{
	meta_driver(
		[&](size_t sze_) { return DBJ_CALLOC(test_array_size, test_array_type); },
		[&](test_array_type *array_) { DBJ_FREE(array_); });
}
// ----------------------------------------------------------
UBENCH(allocators, standard_new_delete)
{
	meta_driver(
		[&](size_t sze_) { return new test_array_type[test_array_size]; },
		[&](test_array_type *array_) { delete[] array_; });
}
// ----------------------------------------------------------
#ifdef DBJ_USES_NEDMALLOC
UBENCH(allocators, ned_14)
{
	meta_driver(
		[&](size_t sze_) { return (test_array_type *)::nedcalloc(test_array_size, sizeof(test_array_type)); },
		[&](test_array_type *array_) { ::nedfree((void *)array_); });
}
#endif // DBJ_USES_NEDMALLOC
// ----------------------------------------------------------
#ifdef DBJ_TESTING_NED_POOL
dbj::collector coll_ned_14_pool("NED14 Pool");
// currently, I probably have no clue
// what are the good values here
{
	static nedpool *pool_ = nedcreatepool(2 * test_array_size, 0);

	meta_driver(
		coll_ned_14_pool,
		[&](size_t sze_) { return (int *)::nedpcalloc(pool_, test_array_size, sizeof(int)); },
		[&](int *array_) { ::nedpfree(pool_, (void *)array_); });

	neddestroypool(pool_);
	// also not sure if this is necessary
	neddisablethreadcache(0);
}
#endif
