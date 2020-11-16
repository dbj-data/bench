


#include <malloc.h>
/*
MSFT "extension" _malloca() must be paired with _freea()
*/
// Variable Length Static ARRay
#define VLS_ARR(T_, S_) (T_*)_alloca(S_ * sizeof(T_))
// Variable Length Static ARRay Pointer
#define VLS_ARP(T_) (T_)_alloca(sizeof(T_))

void function(int size_)
{
	// no need to free + can use runtime length
	// CAUTION! this is not immune to stack overflow
	// see https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/resetstkoflw?view=msvc-160

#ifdef __clang__
	typedef char(*ptr_to_char_arr)[size_];
#else //! __clang__
	typedef char(*ptr_to_char_arr)[0xFF];
#endif // ! __clang__

	ptr_to_char_arr arrp = VLS_ARP(ptr_to_char_arr);
	(*arrp)[size_ / 2] = '!';

	char* local_buf = VLS_ARR(char, size_);

	local_buf[size_ / 2] = '!';

	// _Static_assert( local_buf != NULL ) ;
}

