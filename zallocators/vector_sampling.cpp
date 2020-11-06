/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../ubench/ubench.h"

///-----------------------------------------------
constexpr auto test_array_size = 1;  // 0xFFFF;
struct buffer_type { char data[0xFF]; };
using test_array_type = buffer_type ;
static const test_array_type test_array_element = { {'?'} };

///-----------------------------------------------
#if __has_include(<atlcoll.h>)

#include <atlcoll.h>

UBENCH(vector_dynamic_push, atl_arr)
{
	using atl_arr = ATL::CAtlArray<test_array_type>;

	atl_arr array_;
	// inner loop
	for (int count_ = 0; count_ < test_array_size; ++count_)
	{
		array_.Add(test_array_element);
	}
}
#endif // __has_include(<atlsimpcoll.h>)

///-----------------------------------------------
#if __has_include(<atlsimpcoll.h>)

#include <atlsimpcoll.h>

UBENCH(vector_dynamic_push, atl_simple_arr)
{
	using atl_vec = ATL::CSimpleArray<test_array_type>;

	atl_vec array_;
	// inner loop
	for (int count_ = 0; count_ < test_array_size; ++count_)
	{
		array_.Add(test_array_element);
	}
}
#endif // __has_include(<atlsimpcoll.h>)
///-----------------------------------------------
#include <vector>

UBENCH(vector_dynamic_push, ms_stl_vec_)
{
	std::vector<test_array_type> array_{};
	for (int count_ = 0; count_ < test_array_size; ++count_)
		array_.push_back(test_array_element);
}
///-----------------------------------------------
/// EASTL2020 CORE

#include   <EASTL/vector.h>

UBENCH(vector_dynamic_push, eastl_vec_)
{
	eastl::vector<test_array_type> array_{};
	for (int count_ = 0; count_ < test_array_size; ++count_)
		array_.push_back(test_array_element);
}


