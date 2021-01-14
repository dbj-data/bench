/*
* judging by this measurements heap allocation or stack allocation are equal in performance
*
* it is only that stack allocation is totaly safe, i.e no leaks are possible
*
* but why VLA at all? See this:
https://gustedt.wordpress.com/2011/01/09/dont-be-afraid-of-variably-modified-types/
https://gustedt.wordpress.com/2014/09/08/dont-use-fake-matrices/
https://gustedt.wordpress.com/2011/01/13/vla-as-function-arguments/
*/
#include "../general_utils.h"
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
#include <ubut/ubench.h>
#include <dbj/dbj_common.h>
/*
no need to free + can use runtime length
CAUTION! this is not immune to stack overflow
DBJ: using SEH one can deal with stack overflow in a C/C++ code on Windows.
see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/resetstkoflw?view=msvc-160

MSFT "extension" _malloca() must be paired with _freea(), thus it is a no go

*/
// Stack space Pointer
#define DBJ_STALLOC(T_) (T_*)_alloca(sizeof(T_))

// stack space Variable length ARRay
// not bigger than 0xFFFF
#define DBJ_VLARR(T_, S_) ( assert(S_ < 0xFFFF),  (T_*)_alloca(S_ * sizeof(T_)))

/// @brief testing the dbj vla helpers
/// @param size_ of the memory allocated on the stack
/// @return void
extern "C" void dbj_vla_user(const size_t size_)
{
#ifdef __clang__
	/*
	here is how one uses VLA typedef in case of clang
	cl.exe can not VLA typedefs and likely will never be able to
	*/
	typedef char(*ptr_to_char_arr)[size_];
	// stack based pointer to array
	ptr_to_char_arr arrp = (ptr_to_char_arr)DBJ_VLARR(char, size_);
	(*arrp)[size_ / 2] = '!';
#else  // ! clang
	// no size indication == problem
	typedef char* ptr_to_char_arr;
	// stack based char pointer as array of size_
	char* local_buf = DBJ_VLARR(char, size_);
	local_buf[size_ / 2] = '!';
#endif // ! clang

	// no freeing here necessary
}

extern "C" void dbj_heap_user(const size_t size_)
{
	// no size indication == problem
	typedef char* ptr_to_char_arr;

	// stack based pointer to array
	ptr_to_char_arr arrp = (ptr_to_char_arr)calloc(size_, sizeof(char));
	arrp[size_ / 2] = '!';

	free(arrp);

}

static auto ct_random(void) {
	static bool do_once_and_only_here_ = []() { srand((unsigned)time(0)); return true;  } ();
	(void)do_once_and_only_here_;
	return rand_interval(1, RAND_MAX - 1);
	// return compiletime_pseudo_random::get_random(index) % 0xFFFF;
}

UBENCH(adhoc, dbj_vla) {
	DBJ_REPEAT(0xFF) {
		dbj_vla_user(ct_random());
	}
}

UBENCH(adhoc, dbj_heap) {
	DBJ_REPEAT(0xFF) {
		dbj_heap_user(ct_random());
	}
}
