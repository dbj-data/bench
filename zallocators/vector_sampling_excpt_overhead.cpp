/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../ubench/ubench.h"

#include <string>
#include <stdexcept>

///-----------------------------------------------
using test_array_type = std::string;
static const test_array_type test_array_element = std::string(__FILE__);
///-----------------------------------------------
// on bad index ATL does ATL_ASSERT on debug build
// so lets do only release builds
#ifndef _DEBUG
#if __has_include(<atlsimpcoll.h>)

#include <atlsimpcoll.h>
using atl_vec = ATL::CSimpleArray<test_array_type>;

static void atl_simple_array() {
#if _HAS_EXCEPTIONS
	try {
#endif // _HAS_EXCEPTIONS
		atl_vec array_;
		// add 3 elements
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		array_.Add(test_array_element);
		// use illegal index
		(void)array_[9];
#if _HAS_EXCEPTIONS
	}
	catch ( ...) {
    }
#endif // _HAS_EXCEPTIONS

}

UBENCH(bad_index_vector, atl_simple_arr)
{
	// SEH is always available but not mixed
	// in the same function
	// with C++ exceptions
	__try {
		atl_simple_array();
	}
	__except ( 
		GetExceptionCode() == EXCEPTION_ARRAY_BOUNDS_EXCEEDED 
		? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_EXECUTION )
	{
	// RELEASE build
	// on bad index ATL raises SE with id: EXCEPTION_ARRAY_BOUNDS_EXCEEDED
	// which is defined in minwinbase.h
	// AFAICS, ATL in that scenario never throws C++ exception ...
	(void)42 ;
	}
}
#endif // __has_include(<atlsimpcoll.h>)
#endif // NDEBUG
///-----------------------------------------------
#include <vector>

static const std::string  empty_std_string{} ;

static void mst_stl_bad_index_vector () {
#if _HAS_EXCEPTIONS
	try {
#endif
		std::vector<test_array_type> array_{};
		array_.push_back(empty_std_string);
		array_.push_back(empty_std_string);
		array_.push_back(empty_std_string);
		// use illegal index
		(void)array_.at(9);
#if _HAS_EXCEPTIONS
	} catch (std::out_of_range & ) {}
#endif
}

UBENCH(bad_index_vector, ms_stl_vec_)
{
	__try {
		mst_stl_bad_index_vector();
	} __except (EXCEPTION_EXECUTE_HANDLER) { }

}
///-----------------------------------------------
/// EASTL2020 CORE

/// otherwise EASTL will call debug break
/// which will attach to the debuger if one is running
/// or will exit the app if debugger is not running
/// thus, EASTL doe not use SEH
/// it exists if c++ exceptions as switched of
#if _HAS_EXCEPTIONS == 1

#include <EASTL/vector.h>
#include <EASTL/string.h>

static const eastl::string
   eastl_test_array_element = eastl::string(__FILE__);

UBENCH(bad_index_vector, eastl_vec_)
{
  try {
	eastl::vector<eastl::string> array_{};
	array_.push_back(eastl_test_array_element);
	array_.push_back(eastl_test_array_element);
	array_.push_back(eastl_test_array_element);
	// use illegal index
	// EASTL simply does no index checks inside operator[]
	// if there are no C++ exceptions
	// EASTL does EASTL_FAIL_MSG()
	// which boils down to OutputDebugStringA()
	// but only if debugger is present
	// if it is not, I do not see anything happening?
	// so the result bellow will provoke very bad things
	(void)array_.at(9);

	} catch ( std::out_of_range & x) 
	{
		// yes EASTL will use std:: exceptions
		(void)x.what();
	}
}

#endif // _HAS_EXCEPTIONS

