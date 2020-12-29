
#include "../ubut/ubench.h"
#include <assert.h>

#ifdef __cplusplus

#include "random_words_generator.h"


UBENCH(adhoc, random_words)
{
	dbj_adhoc::test_random_words_generator();
}

#endif // __cplusplus

//#ifdef _DEBUG
//#define HASH_DEBUG = 1
//#endif // _DEBUG
// #define HASH_BLOOM 27
#include <uthash/uthash.h>

enum { string_data_max_length = 0xFF };

struct string_ {
	int id;            /* we'll use this field as the key */
	char data[string_data_max_length];
	UT_hash_handle hh; /* makes this structure hashable */
};


UBENCH(hash_table, uthash_primary)
{
	struct string_* strings_ = NULL;

	for (int k = 0; k < 0xFF; ++k)
	{
		struct string_* str_ = (struct string_*)calloc(1, sizeof(struct string_));
		str_->id = k;
		_itoa_s(k, str_->data, string_data_max_length - 1, 10);
		HASH_ADD_INT(strings_, id, str_);
	}

	int k = 200;
	struct string_* str_ = NULL;
	HASH_FIND_INT(strings_, &k, str_);
	if (str_) {
		HASH_DEL(strings_, str_);
		free(str_);
		str_ = NULL;
	}

	HASH_CLEAR(hh, strings_);
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
#undef utarray_erase_element
#define utarray_erase_element(a,ep_) do {                                     \
  int pos = utarray_eltidx(a, ep_);                                           \
  assert(pos > -1 );                                                          \
  assert((a)->i > pos);                                                       \
  if ((a)->icd.dtor) {                                                        \
      (a)->icd.dtor( ep_);                                                    \
  }                                                                           \
  int len = 1;                                                                \
  if ((a)->i > ((pos) + (len))) {                                             \
    memmove(_utarray_eltptr(a, pos), _utarray_eltptr(a, (pos) + (len)),       \
            ((a)->i - ((pos) + (len))) * (a)->icd.sz);                        \
  }                                                                           \
  (a)->i -= (len);                                                            \
} while(0)

#endif // UTARRAY_DBJ_EXTENSIONS

typedef struct {
	int hash;
	char* data;
} uniq_string_t;

// utarray_find_if will be passed and using this
inline bool uniq_string_t_is_equal(int hash_, uniq_string_t* specimen_)
{
	return specimen_->hash == hash_;
}

inline void uniq_string_copy(void* _dst, const void* _src) {
	uniq_string_t* dst = (uniq_string_t*)_dst, * src = (uniq_string_t*)_src;
	dst->hash = src->hash;
	dst->data = src->data ? _strdup(src->data) : NULL;
}

inline void uniq_string_dtor(void* _elt) {
	uniq_string_t* elt = (uniq_string_t*)_elt;
	if (elt->data) free(elt->data);
}

//inline void intchar_print(UT_array* uniq_strings_) {
//
//	uniq_string_t* unique_str_ptr_ = NULL;
//
//	while ((unique_str_ptr_ = (uniq_string_t*)utarray_next(uniq_strings_, unique_str_ptr_))) {
//		printf("%d %s\n", unique_str_ptr_->hash, (unique_str_ptr_->data ? unique_str_ptr_->data : "null"));
//	}
//}

inline int intsort(const void* a, const void* b) {
	uniq_string_t unique_str_a = *(const uniq_string_t*)a;
	uniq_string_t unique_str_b = *(const uniq_string_t*)b;
	int _a = unique_str_a.hash;
	int _b = unique_str_b.hash;
	return (_a < _b) ? -1 : (_a > _b);
}

UT_array* uniq_strings_ = NULL;

UT_icd uniq_string_t_icd = { sizeof(uniq_string_t), NULL, uniq_string_copy, uniq_string_dtor };

/*
 * make sure unique strings are added only
 * below we try 255 times to add these 10 strings
 */

char* strings_to_store[] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten"};

inline UT_array* populate_strings_array(uniq_string_t specimen_) {

	utarray_new(uniq_strings_, &uniq_string_t_icd); // mandatory init

	int string_idx_ = 0;
	// make 255 entries "0" ... "254"
	for ( int k = 0; k < 0xFF; ++k)
	{
		specimen_.hash = k;
		// there is a copy function used by  uniq_string_t_icd 
		specimen_.data = strings_to_store[(string_idx_++) % 10 ];

		// first check if it does exist
		uniq_string_t* existing_ = NULL ;
		utarray_find_if(uniq_strings_, existing_, 15, uniq_string_t_is_equal);

		if (! existing_)
			utarray_push_back(uniq_strings_, & specimen_);
	}

	return uniq_strings_;
}

UBENCH(hash_table, utarray_c_user)
{
	uniq_string_t specimen_ = { 0, NULL };

	uniq_strings_ = populate_strings_array(specimen_);

	uniq_string_t* unique_str_ptr_ = NULL;

#ifdef UTARRAY_DBJ_EXTENSIONS

	// DBJ extension
	// no sort necessary before find and free

	utarray_find_if(uniq_strings_, unique_str_ptr_, 15, uniq_string_t_is_equal);
	// unique_str_ptr_ will point to the result
	assert(unique_str_ptr_);
	utarray_erase_element(uniq_strings_, unique_str_ptr_);
	// destructed but not freed
	// free(unique_str_ptr_);
	unique_str_ptr_ = NULL;

#else
	// utarray by default has to be sorted before it is searched
	// abd that is a killer feature in the sense of
	// kill it , do not use it
	utarray_sort(uniq_strings_, intsort);

	// by deault need to make a node to find entry 
	specimen_->hash = 0xF;
	specimen_->data = NULL;

	// in this case result must be 15/"15"
	unique_str_ptr_ = utarray_find(uniq_strings_, specimen_, intsort);
	// get index of element from pointer
	int idx_ = utarray_eltidx(uniq_strings_, unique_str_ptr_);
	// in here hash == data thus 
	assert(idx_ == atoi(unique_str_ptr_->data));

	// lastly erase it by index
	// utarray_erase(UT_array *a,int pos,int len)
	utarray_erase(uniq_strings_, idx_, 1);
#endif // ! UTARRAY_DBJ_EXTENSIONS


	// this is calling utarray_done
	// so DO NOT free the array elements pointers before !
	utarray_free(uniq_strings_);
	uniq_strings_ = NULL;
}