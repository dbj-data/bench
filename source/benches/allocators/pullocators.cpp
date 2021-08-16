/// (c) 2020 by dbj.org
/// comparing allocation pools 

#include "../../../ubut/ubench.h"
#include "../../../dbj-fwk/wallofmacros.h"

/// ---------------------------------------------------------------------
#include "Kenwright/kenwright_mempool.h"

/// compare pullocators, mechatronics for

struct defaul_allocator final {
	void* allocate(size_t sz) { return ::operator new(sz); }
	void  deallocate(void* p) { ::operator delete(p); }
};

using test_array_type = double;
constexpr int test_array_size = 0xFFFFF;

template<typename allocator = defaul_allocator >
struct X final
{
	static void* operator new(size_t sz)
	{
		return   allocator::allocate(sz);
	}

	static void* operator new[](size_t sz)
	{
		return   allocator::allocate(sz);
	}

		static void operator delete(void* ptr) noexcept
	{
		return   allocator::deallocate(ptr);
	}

	static void operator delete [](void* ptr) noexcept
	{
		return   allocator::deallocate(ptr);
	}
}; // X

/// ---------------------------------------------------------------------
inline auto meta_driver = [](auto aloka, auto dealoka) {
	test_array_type* array_ = (test_array_type*)aloka(test_array_size);
	DBJ_ASSERT(array_);
	dealoka(array_);
};

// ----------------------------------------------------------
UBENCH(pullocators, kenwright_pool)
{
	// just to give it some size
	static auto sizeOfEachBlock = sizeof(test_array_type[0xF]);
	static auto numOfBlocks = test_array_size;
	// m_memStart = new uchar[m_sizeOfEachBlock * m_numOfBlocks];
	static kenwright::pool pool_(sizeOfEachBlock, numOfBlocks);

	meta_driver(
		[&](size_t sze_) { return pool_.Allocate(); },
		[&](test_array_type* p_) { pool_.DeAllocate(p_); });
}
// ----------------------------------------------------------
UBENCH(pullocators, default_pool)
{
	static defaul_allocator dflt;

	meta_driver(
		[&](size_t sze_) { return dflt.allocate(sze_); },
		[&](test_array_type* p_) { dflt.deallocate(p_); });
}

// ----------------------------------------------------------
// nvwa is a law obiding c++ citizen
#ifdef _CPPUNWIND
#include "nvwa/fixed_mem_pool.h"
#include "nvwa/static_mem_pool.h"

namespace {

	UBENCH(pullocators, nvwa_static_pool)
	{
		using nvwa_pool = nvwa::static_mem_pool<test_array_size>;

		meta_driver(
			[&](size_t sze_) { return (test_array_type*)nvwa_pool::instance_known().allocate(); },
			[&](test_array_type* array_) { nvwa_pool::instance_known().deallocate(array_); });
	}

	struct guardian final {

		using nvwa_pool = nvwa::fixed_mem_pool<test_array_type>;
		inline static nvwa_pool nvwa;

		guardian() {
			nvwa_pool::initialize(test_array_size);
			_ASSERTE(true == nvwa.is_initialized());
		}
		~guardian() {
			if (nvwa.is_initialized())
				nvwa_pool::deinitialize();
		}
	};

	inline guardian on_off_;

	UBENCH(pullocators, nvwa_fixed_mem_pool)
	{
		meta_driver(
			[&](size_t sze_) { return (test_array_type*)guardian::nvwa.allocate(); },
			[&](test_array_type* array_) { guardian::nvwa.deallocate(array_); });
	}
}
#endif // _CPPUNWIND
// ---------------------------------------------------------- 
#ifdef DBJ_TESTING_NED_POOL

// currently, I probably have no clue
// what are the good values here
UBENCH(pullocators, ned14_mem_pool)
{
	static nedpool* pool_ = nedcreatepool(2 * test_array_size, 0);

	meta_driver(
		[&](size_t sze_) { return (int*)::nedpcalloc(pool_, test_array_size, sizeof(int)); },
		[&](int* array_) { ::nedpfree(pool_, (void*)array_); });

	neddestroypool(pool_);
	// also not sure if this is necessary
	neddisablethreadcache(0);
}
#endif

