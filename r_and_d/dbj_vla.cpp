
#include <assert.h>
#include <crtdbg.h>
#include <malloc.h>
/*
no need to free + can use runtime length
CAUTION! this is not immune to stack overflow
DBJ: using SEH one can deal with stack overflow in a C/C++ code on Windows.
see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/resetstkoflw?view=msvc-160

MSFT "extension" _malloca() must be paired with _freea(), thus it is a no go

*/
// Variable Length Static ARRay Pointer
#define DBJ_STALLOC(T_) (T_)_alloca(sizeof(T_))

// Variable Length Static ARRay
#define DBJ_VLS_ARR(T_, S_) (T_*)_alloca(S_ * sizeof(T_))


extern "C" void dbj_vla_user(int size_)
{
#ifdef __clang__
/*here is how one uses VLA typedef in case of clang*/
	typedef char(*ptr_to_char_arr)[size_];
#else  // ! clang
	typedef char(*ptr_to_char_arr)[0xFF];
#endif // ! clang

    // stack based pointer to array
	ptr_to_char_arr arrp = (ptr_to_char_arr)_alloca(sizeof(ptr_to_char_arr));
	(*arrp)[size_ / 2] = '!';

    // stack based char pointer as array of size_
	char* local_buf = DBJ_VLS_ARR(char, size_);
	       local_buf[size_ / 2] = '!';

	// no freeing here is necessary
}

