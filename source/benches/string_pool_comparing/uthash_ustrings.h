#pragma once
#include "../../ustring_pool_interface.h"
#include <ubut/ubench.h>

#include <dbj/dbj_ustrings.h> // dbj::string_ptr


/*
at the bottom define the BENCH-es
all here and all in one place
this we can add/remove them

this is used from all_in_one.cpp in this same folder
*/

/*
uthash has a problem when deleting
Atrthur has self-inflicted it on his plate to fix that
*/
#if 0
UBENCH(strpool_evergrowing, uthash_string_pool) {
	test_evergrowing<uthash_pool>();
}

UBENCH(strpool_evergrowing, uthash_using_vector) {
	test_evergrowing<uthash_vector_pool>();
}

UBENCH(strpool_evergrowing, uthash_using_unique_ptr) {
	test_evergrowing<uthash_up_pool>();
}
#endif // 0
