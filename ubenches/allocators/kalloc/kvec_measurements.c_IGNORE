#include "../../ubench/ubench.h"

/*
note for c++ addicionados

kvec is good only for  c++ scalars, not objects

std::string * arr[0xFF] ;

above arr is stack space for 254 unintialized slots of the sizeof(std::string) size
thus:

inside kvec this happens on push() , simplified:

// first push makes arr of string pointers of size 1
// arr[0] slot is made but it does not contain std::string instance
arr = (string*)realloc(arr, sizeof(string) * 1 );
// SEGV or simillar
arr[0] = std::string("oops");

hat line will call `operator = ()` on an unitialized string on the left side

*/

#include "kvec.h"

typedef struct test_array_type_ {
    char data[0xFF];
} test_array_type ;

#define test_array_size 0xFFFF

test_array_type test_array_element = {0} ;
///-----------------------------------------------
UBENCH(kvec_init_resize_destroy, kvec_)
{
	kvec_t(test_array_type) array_;
	kv_init(array_);
	kv_resize(test_array_type, array_, test_array_size);
	kv_destroy(array_);
}
///-----------------------------------------------
UBENCH(kvec_dynamic_push, kvec_)
{
	kvec_t(test_array_type) array_;
	kv_init(array_);
	for (int count_ = 0; count_ < test_array_size; ++count_)
		kv_push(test_array_type, array_, test_array_element);
	kv_destroy(array_);
}
