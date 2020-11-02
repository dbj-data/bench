/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../ubench/ubench.h"

#include <string>
///-----------------------------------------------
constexpr auto test_array_size = 0xFFFF;
using test_array_type = std::string;
static const test_array_type test_array_element = std::string(__FILE__);
///-----------------------------------------------
#if __has_include(<atlsimpcoll.h>)

#include <atlsimpcoll.h>
using atl_vec = ATL::CSimpleArray<test_array_type>;

UBENCH(vector_dynamic_push, atl_simple_arr)
{
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

UBENCH(vector_dynamic_push, std_vec_)
{
	std::vector<test_array_type> array_{};
	for (int count_ = 0; count_ < test_array_size; ++count_)
		array_.push_back(test_array_element);
}
///-----------------------------------------------
/// EASTL2020 CORE

#include <EASTL/vector.h>
#include <EASTL/string.h>

static const eastl::string
   eastl_test_array_element = eastl::string(__FILE__);

UBENCH(vector_dynamic_push, eastl_vec_)
{
	eastl::vector<eastl::string> array_{};
	for (int count_ = 0; count_ < test_array_size; ++count_)
		array_.push_back(eastl_test_array_element);
}


