
#define _CRT_SECURE_NO_WARNINGS 1

#pragma warning( push )
#pragma warning( disable : 6308)
// #pragma warning( supress : 6308)

#include <ubut/utest.h>
#include "../ht_testing_common.h"

// #if __STDC_VERSION__ >= 201112L || __cplusplus >= 201103L
// C11 and C++11
// #endif // __STDC_VERSION__ >= 201112L || __cplusplus >= 201103L

#include "../source/loki_assoc_vector.h"
#include <slot/packed_array.h>
#include <string>

using tmd = testing_meta_data;

UTEST(handle_solutions, loki_assoc_vector)
{
	Loki::AssocVector< size_t, std::string > lav;

	lav[0] = "A"; // eh?
	lav[1] = "B"; // eh?
	lav[2] = "C"; // eh?

	ASSERT_EQ(lav.size(), 3);

	lav[0] = "A"; // eh?
	lav[1] = "B"; // eh?
	lav[2] = "C"; // eh?

	ASSERT_EQ(lav.size(), 3);
}

//UTEST(handle_solutions, packed_aray_dbj) {
//
//	using packed_array_of_string = dbj::packed_array<std::string>;
//
//	packed_array_of_string pas_;
//
//	//// it_type is actually Handle id but users need not know
//	//packed_array_of_string::id_type id_ = pas_.new_element();
//
//	//// the ref to the newly allocated element
//	//packed_array_of_string::value_type& vr_ = pas_.lookup(id_);
//
//	//// use the string that is inside packed array
//	//vr_ = tmd::hello;
//
//	//// compare the result of lookup
//	//ASSERT_TRUE(pas_.lookup(id_) == tmd::hello);
//	//// there should be 1
//	//ASSERT_TRUE(pas_.size() == 1);
//
//}

#pragma warning( pop )
