#ifndef NDEBUG
#define DBJ_VFY(X) do {if (false == (X)) fprintf(stderr, "\nERROR! %s(%d) : %s has failed", __FILE__, __LINE__, #X ); exit(0); } while(0)
#else // NDEBUG ala release
#define DBJ_VFY(X) ((void)((X)))
#endif // NDEBUG

#include "../ubut/ubench.h"
#include <assert.h>

#include "string_pool_common.h"
#include <uthash/uthash.h>

enum { uth_string_data_max_length = 0xFF, strpool_size = 0xFF };

struct uth_string_ {
	int id;            /* we'll use this field as the key */
	char data[uth_string_data_max_length];
	UT_hash_handle hh; /* makes this structure hashable */
};

// serious cheat ;)
static struct uth_string_ uth_string_pool[strpool_size + 1] = { {0, {0}} };
size_t uth_string_pool_walker = 0;

// note for c++ eggheads: we do not use heap, we cheat
inline struct uth_string_  * uth_string_make( int number_ ) {
	struct uth_string_ * str_ = & uth_string_pool[(uth_string_pool_walker++) % strpool_size] ;
	str_->id = number_;
	_itoa_s(number_, str_->data, uth_string_data_max_length - 1, 10);
	return str_;
}

UBENCH(strpool, uthash_int_struct)
{
	struct uth_string_* strings_ = NULL;

	for (int k = 0; k < strpool_size ; ++k)
	{
		struct uth_string_* str_ = NULL;
		HASH_FIND_INT(strings_, &k, str_);
		if (! str_) {
			struct uth_string_* lvalue_ = uth_string_make(k);
			HASH_ADD_INT(strings_, id, lvalue_);
		}
	}

	int k = 200;
	struct uth_string_* str_ = NULL;
	HASH_FIND_INT(strings_, &k, str_);
	if (str_) {
		HASH_DEL(strings_, str_);
		// must not: free(str_); as it is not made on the heap
		str_ = NULL;
	}

	HASH_CLEAR(hh, strings_);
}


/*
* store the same struct as above but now "data" is the key member
*/


UBENCH(strpool, uthash_string_as_key )
{
	struct uth_string_ * str_, * users = NULL;

	for (int k = 0; k < strpool_size ; ++k)
	{
		struct uth_string_ * lvalue_ = uth_string_make(k);

		str_ = lvalue_ ;
		HASH_FIND_STR(users, lvalue_->data, str_ );

		if (! str_) {
			HASH_ADD_STR(users, data, lvalue_);
		}
	}

#ifdef _DEBUG
	int count_ = HASH_COUNT(users);
#endif
	// remove 
	HASH_FIND_STR(users, "254", str_);
	if (str_) {
		HASH_DEL(users, str_);
		// must not: free(str_); as it is made on stack
	}
#ifdef _DEBUG
	count_ = HASH_COUNT(users);
#endif
	struct uth_string_* tmp = NULL;
	/* free the rest of hash table contents */
	HASH_ITER(hh, users, str_, tmp) {
		HASH_DEL(users, str_);
		// must not: free(str_); as it is made on stack
	}
#ifdef _DEBUG
	count_ = HASH_COUNT(users);
#endif
}

/*
-----------------------------------------------------------------------------------------------------------
-----------------------------------------------------------------------------------------------------------
*/
#include <uthash/utarray.h>

#define UTARRAY_DBJ_EXTENSIONS

#ifdef  UTARRAY_DBJ_EXTENSIONS

// DBJ extension
// no sort necessary before find and free

#undef utarray_find_if
#define utarray_find_if(arr_, result_ptr, needle_value_ , comparator_ ) \
	do {\
	result_ptr = NULL;\
    while ((result_ptr = utarray_next(arr_, result_ptr))) if ( comparator_( needle_value_, result_ptr)  ) break ;\
	} while (0)

// do not free the element 
// utarray_free(uniq_strings_); frees the elements


#endif // UTARRAY_DBJ_EXTENSIONS

/*  storing same as above
struct uth_string_ {
	int id;         
	char data[uth_string_data_max_length];
	UT_hash_handle hh; 
};
*/

// utarray_find_if will be passed and using this
inline bool uniq_string_t_hash_is_equal(int hash_, struct uth_string_* specimen_)
{
	return specimen_->id == hash_;
}

// utarray_find_if will be passed and using this
inline bool uniq_string_t_data_is_equal(const char * needle_, struct uth_string_* specimen_)
{
	return ! strcmp( specimen_->data, needle_ );
}

inline void uniq_string_copy(void* _dst, const void* _src) {
	struct uth_string_* dst = (struct uth_string_*)_dst, * src = (struct uth_string_*)_src;
	dst->id = src->id;
	strncpy(dst->data, src->data, uth_string_data_max_length );
}

inline void uniq_string_dtor(void* _elt) {
	struct uth_string_* elt = (struct uth_string_*)_elt;
	elt->id = 0;
	// remember: no heap allocation used
	elt->data[0] = '\0';
}

//inline void intchar_print(UT_array* uniq_strings_) {
//
//	uth_string_* unique_str_ptr_ = NULL;
//
//	while ((unique_str_ptr_ = (uth_string_*)utarray_next(uniq_strings_, unique_str_ptr_))) {
//		printf("%d %s\n", unique_str_ptr_->hash, (unique_str_ptr_->data ? unique_str_ptr_->data : "null"));
//	}
//}

inline int intsort(const void* a, const void* b) {
	struct uth_string_ unique_str_a = *(const struct uth_string_*)a;
	struct uth_string_ unique_str_b = *(const struct uth_string_*)b;
	int _a = unique_str_a.id;
	int _b = unique_str_b.id;
	return (_a < _b) ? -1 : (_a > _b);
}

UT_array* uniq_strings_ = NULL;

UT_icd uniq_string_t_icd = { sizeof(struct uth_string_), NULL, uniq_string_copy, uniq_string_dtor };

inline UT_array* populate_strings_array(void) {

	utarray_new(uniq_strings_, &uniq_string_t_icd); // mandatory init

	int string_idx_ = 0;
	// make 255 entries "0" ... "254"
	for ( int k = 0; k < strpool_size ; ++k)
	{
		struct uth_string_* specimen_ = uth_string_make(k);
		
		/* using The Paul Hsieh hash macro 
		*  from uthash
		*/
		HASH_SFH(specimen_->data, strlen(specimen_->data), specimen_->id);
		specimen_->id = abs(specimen_->id);
		assert(specimen_->id > 0);

		// first check if it does exist
		struct uth_string_* existing_ = NULL ;
		utarray_find_if(uniq_strings_, existing_, specimen_->id, uniq_string_t_hash_is_equal);

		if (! existing_)
			utarray_push_back(uniq_strings_, & specimen_);
	}

	return uniq_strings_;
}

UBENCH(strpool, utarray_c_user)
{
	struct uth_string_ specimen_ = { 0, {0} };

	uniq_strings_ = populate_strings_array();

	// DBJ extension
	// no sort necessary before find and free
	struct uth_string_* unique_str_ptr_ = NULL;

	utarray_find_if(uniq_strings_, unique_str_ptr_, "nine", uniq_string_t_data_is_equal);

	// unique_str_ptr_ will point to the result
	if (unique_str_ptr_) {

#ifdef _DEBUG
	size_t pos = utarray_eltidx(uniq_strings_, unique_str_ptr_);
#endif

#undef utarray_erase_element
#define utarray_erase_element(arr_,elm_ptr_) \
do { \
  size_t  pos = utarray_eltidx(arr_, elm_ptr_), len = 1;  \
  if ((arr_)->icd.dtor) { (arr_)->icd.dtor( elm_ptr_); } \
  if ((arr_)->i > ((pos) + (len))) \
	memmove( elm_ptr_, elm_ptr_ + 1, ((arr_)->i - (int)((pos) + (len))) * (arr_)->icd.sz);\
  (arr_)->i -= (int)(len);\
} while (0)

	  utarray_erase_element(uniq_strings_, unique_str_ptr_);

		// destructed but not freed
		// free(unique_str_ptr_);
		unique_str_ptr_ = NULL;
	}

	// this is calling utarray_done
	// so DO NOT free the array elements pointers before !
	utarray_free(uniq_strings_);
	uniq_strings_ = NULL;
}