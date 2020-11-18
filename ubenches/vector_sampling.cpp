/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../ubut/ubench.h"
// #include <vcruntime.h>

constexpr static int vector_size = 0xFF;

///-----------------------------------------------
/// driver driving vector types instance from bellow
/// for a good hammering we pass and return by value
/// then we make a vector of a size N
/// then we remove the element in the middle N/4 times
/// minimal N must be vector_size
template<
	typename VectorType_,
	typename VectorElementType_, size_t N,
	typename middleman_remover >
	static VectorType_
	hammer_of_thor
	(middleman_remover remover_)
{
	static_assert(N >= vector_size);

	VectorType_ vector_;

	VectorElementType_ native_arr_[N] = {};

	for (auto&& element_ : native_arr_)
	{
		vector_.push_back(element_);
	}

	for (int k = 0; k < N / 4; ++k)
		remover_(vector_);

	return vector_;
}
///-----------------------------------------------
 // USING_VECTORS_WITH_STD_INTERFACE
#if 0

#if __has_include(<atlcoll.h>)

#include <atlsimpcoll.h>
#include <atlcoll.h>

namespace atl_sampling {

	struct buffer_type { char data[0xFF]; };

	UBENCH(vectors, atl_arr)
	{
		// remove the one from the middle
		// no checks whatsoever is deliberate
		hammer_of_thor<ATL::CAtlArray<buffer_type>, buffer_type, vector_size>(
			[](auto& arr_)
			{
				arr_.RemoveAt(arr_.GetCount() / 2, 1);
			}
		);
	}

	UBENCH(vectors, atl_simple_arr)
	{
		using atl_vec = ATL::CSimpleArray<test_array_type>;
		atl_vec array_;
		array_.Add(test_array_element);
	}
} // namespace atl_sampling 

#endif // __has_include(<atlcoll.h>)

#endif // 0 USING_VECTORS_WITH_STD_INTERFACE

///-----------------------------------------------
#include <vector>
#include <string>

namespace ms_stl_sampling {

	using std::vector;
	using std::string;

	UBENCH(vectors, ms_stl)
	{
// remove the one from the middle
// no checks whatsoever is deliberate
		hammer_of_thor<std::vector<std::string>, std::string, vector_size>(
			[](auto& array_)
			{
				array_.erase(array_.begin() + (array_.size() / 2));
			}
		);
	}
} // namespace ms_stl_sampling 

///-----------------------------------------------
#if __has_include(<EASTL/vector.h>)
	/// EASTL2020 CORE
#include   <EASTL/vector.h>
#include   <EASTL/string.h>
namespace EASTL_sampling {

	UBENCH(vectors, eastl)
	{
		hammer_of_thor<eastl::vector<eastl::string>, eastl::string, vector_size>(
			[](auto& array_)
			{
				array_.erase(array_.begin() + (array_.size() / 2));
			}
		);
	}

} // namespace EASTL_sampling

#endif // __has_include(EASTL/vector.h)
