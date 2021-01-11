/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include <ubut/ubench.h>

constexpr static int vector_size = 0xFFF;
static char buffer[vector_size] = { '?' };
///-----------------------------------------------
/// driver driving vector types instance from bellow
/// for a good hammering we pass and return by value
/// then we make a vector of a size N
/// then we remove the element in the middle N/4 times
/// minimal N must be vector_size
template<
	typename VectorType_,
	typename VectorElementType_,
	typename middleman_remover >
	static VectorType_
	hammer_of_thor
	(middleman_remover remover_, VectorElementType_ const& default_element, size_t N)
{
	_ASSERTE(N == vector_size);

	VectorType_ vector_;

	for (int k = 0; k < N; ++k)
		vector_.push_back(default_element);

	for (int k = 0; k < N / 4; ++k)
		// arg by value, return by value
		vector_ = remover_(vector_);

	return vector_;
}

///-----------------------------------------------
#include <vector>
#include <string>

namespace ms_stl_sampling {

	using std::vector;
	using std::string;

	UBENCH(vector_of_0xFFF_strings_each_0xFFF_chars, ms_stl)
	{
		auto rezult_v_ =
			hammer_of_thor< std::vector<std::string> >(
				[](auto v_)
				{
					v_.erase(v_.begin() + (v_.size() / 2));
					return v_;
				}
				, std::string{ buffer }
					, vector_size
					);
	}
} // namespace ms_stl_sampling 

///-----------------------------------------------
#if __has_include(<EASTL/vector.h>)
	/// EASTL2020 DBJ version with "better" allocator design
#include   <EASTL/vector.h>
#include   <EASTL/string.h>
namespace EASTL_sampling {

	UBENCH(vector_of_0xFFF_strings_each_0xFFF_chars, eastl)
	{
		auto rezult_v_ =
			hammer_of_thor<eastl::vector<eastl::string> >(
				[](auto v_)
				{
					v_.erase(v_.begin() + (v_.size() / 2));
					return v_;
				}
				, eastl::string{ buffer }
					, vector_size
					);
	}

} // namespace EASTL_sampling

#endif // __has_include(EASTL/vector.h)

///-----------------------------------------------
 // USING_VECTORS_WITH_STD_INTERFACE
#if USING_VECTORS_WITH_NOT_STD_INTERFACE

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

#endif // USING_VECTORS_WITH_NOT_STD_INTERFACE
