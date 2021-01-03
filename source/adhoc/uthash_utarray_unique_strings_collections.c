
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


UBENCH(hash_table, uthash_string_struct)
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


enum { my_user_count = 3, name_max_len = 0xF };

struct my_struct {
	char name[name_max_len];             /* key (string is WITHIN the structure) */
	int id;
	UT_hash_handle hh;         /* makes this structure hashable */
};

static const char* my_names[my_user_count] = { "joe", "bob", "betty" };

UBENCH(hash_table, uthash_string_as_key )
{
	struct my_struct* str_, * tmp, * users = NULL;

	for (int k = 0; k < 0xFF; ++k) 
	{
		static char name_[name_max_len] = {0};
		(void)snprintf(name_, name_max_len, "%d", k);

		str_ = NULL;
			HASH_FIND_STR(users, name_, str_);
		if (! str_) {
			str_ = (struct my_struct*)malloc(sizeof * str_);
			strncpy(str_->name, name_, name_max_len );
			str_->id = k;
			HASH_ADD_STR(users, name, str_);
		}
	}

#ifdef _DEBUG
	int count_ = HASH_COUNT(users);
#endif
	// remove 
	HASH_FIND_STR(users, "254", str_);
	if (str_) {
		HASH_DEL(users, str_);
		free(str_);
	}
#ifdef _DEBUG
	count_ = HASH_COUNT(users);
#endif

	/* free the rest of hash table contents */
	HASH_ITER(hh, users, str_, tmp) {
		HASH_DEL(users, str_);
		free(str_);
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
#include <uthash/uthash.h>

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

#ifndef NDEBUG
#define DBJ_VFY(X) do {if (false == (X)) fprintf(stderr, "\nERROR! %s(%d) : %s has failed", __FILE__, __LINE__, #X ); exit(0); } while(0)
#else // NDEBUG ala release
#define DBJ_VFY(X) ((void)((X)))
#endif // NDEBUG

// do not free the element 
// utarray_free(uniq_strings_); frees the elements
#undef utarray_erase_element
#define utarray_erase_element(a,ep_) do {                                     \
  int pos = utarray_eltidx(a, ep_);                                           \
  DBJ_VFY(pos > -1 );                                                         \
  DBJ_VFY((a)->i > pos);                                                      \
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
inline bool uniq_string_t_hash_is_equal(int hash_, uniq_string_t* specimen_)
{
	return specimen_->hash == hash_;
}

// utarray_find_if will be passed and using this
inline bool uniq_string_t_data_is_equal(const char * needle_, uniq_string_t* specimen_)
{
	return ! strcmp( specimen_->data, needle_ );
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

		/* using The Paul Hsieh hash macro 
		*  from uthash
		*/
		HASH_SFH(specimen_.data, strlen(specimen_.data), specimen_.hash);
		specimen_.hash = abs(specimen_.hash);
		assert(specimen_.hash > 0);

		// first check if it does exist
		uniq_string_t* existing_ = NULL ;
		utarray_find_if(uniq_strings_, existing_, specimen_.hash, uniq_string_t_hash_is_equal);

		if (! existing_)
			utarray_push_back(uniq_strings_, & specimen_);
	}

	return uniq_strings_;
}

UBENCH(hash_table, utarray_c_user)
{
	uniq_string_t specimen_ = { 0, NULL };

	uniq_strings_ = populate_strings_array(specimen_);

	// DBJ extension
	// no sort necessary before find and free
	// get the fourth element
	uniq_string_t* unique_str_ptr_ = NULL;

	utarray_find_if(uniq_strings_, unique_str_ptr_, "nine", uniq_string_t_data_is_equal);

	// unique_str_ptr_ will point to the result
	if (unique_str_ptr_) {

#ifdef _DEBUG
	size_t pos = utarray_eltidx(uniq_strings_, unique_str_ptr_);
#endif

	UT_array* a = uniq_strings_;
	uniq_string_t* ep_ = unique_str_ptr_ ;
// #define utarray_erase_element(a,ep_) 
do {                                
  size_t  pos = utarray_eltidx(a, ep_);  
  bool b1 = pos > (-1);
  //DBJ_VFY( sgn(pos) );                                                       
  //DBJ_VFY((a)->i > pos);                                                    
  if ((a)->icd.dtor) {                                                      
      (a)->icd.dtor( ep_);                                                  
  }                                                                         
  size_t len = 1;
  if ((a)->i > ((pos) + (len))) {                                           
    memmove( ep_, ep_ + 1,     
            ((a)->i - (int)((pos) + (len))) * (a)->icd.sz);                      
  }                                                                         
  (a)->i -= (int)(len);                                                          
} while (0);

		// destructed but not freed
		// free(unique_str_ptr_);
		unique_str_ptr_ = NULL;
	}

	// this is calling utarray_done
	// so DO NOT free the array elements pointers before !
	utarray_free(uniq_strings_);
	uniq_strings_ = NULL;
}