/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../../ubench/ubench.h"

#include <vector>
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
#ifdef MEASURE_NOT_DYNAMIC_PUSH
UBENCH(vector_init_resize_destroy, dbj_kvec_)
{
	dbj_kvec_t(test_array_type) array_;
	dbj_kv_init(array_);
	dbj_kv_resize(test_array_type, array_, test_array_size);
	dbj_kv_destroy(array_);
}
#endif
///-----------------------------------------------
#ifdef MEASURE_NOT_DYNAMIC_PUSH
UBENCH(vector_create_resize_leave, std_vec_)
{

	std::vector<test_array_type> array_{};
	array_.resize(test_array_size);
}
#endif
///-----------------------------------------------
UBENCH(vector_dynamic_push, std_vec_)
{

	std::vector<test_array_type> array_{};
	for (int count_ = 0; count_ < test_array_size; ++count_)
		array_.push_back(test_array_element);
}
///-----------------------------------------------
#include "kvec.h"
#include <type_traits>
///-----------------------------------------------
#ifdef MEASURE_NOT_DYNAMIC_PUSH
UBENCH(vector_init_resize_destroy, kvec_)
{
	if constexpr( std::is_scalar_v<test_array_type> ) 
	{
	dbj_kvec_t(test_array_type) array_;
	kv_init(array_);
	kv_resize(test_array_type, array_, test_array_size);
	kv_destroy(array_);
	}
}
#endif
///-----------------------------------------------
UBENCH(vector_dynamic_push, kvec_)
{
	if constexpr( std::is_scalar_v<test_array_type> ) 
	{
	kvec_t(test_array_type) array_;
	kv_init(array_);
	for (int count_ = 0; count_ < test_array_size; ++count_)
		kv_push(test_array_type, array_, test_array_element);
	kv_destroy(array_);
	}
}
