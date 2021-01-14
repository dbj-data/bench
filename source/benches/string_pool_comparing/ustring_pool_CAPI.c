#include "../../ustring_pool_interface_CAPI.h"

/****************************************************************************************/
#pragma region STB DS pool

#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

// the implementation
enum { stb_pool_value_max_len = 0xFF };

static size_t stb_pool_seed() { const /*time_t*/ __int64 seed_ = time(0); return seed_; }

// hash value for a string.
static size_t stb_pool_string_hash(char* str_) { return stbds_hash_string(str_, stb_pool_seed()); }

struct stb_pool_item { size_t key; char* value; };

static struct stb_pool_item* stb_pool = NULL;

static dbj_handle stb_pool_add(const char* str_)
{
	dbj_handle hash = stb_pool_string_hash((char*)str_);
	struct stb_pool_item* itm_ = hmgetp_null(stb_pool, hash);

	if (!itm_) {

		struct  stb_pool_item* t = stb_pool;
		dbj_handle k = hash;
		char* v = (char*)str_;

		(t) = stbds_hmput_key_wrapper((t), sizeof * (t), (void*)STBDS_ADDRESSOF((t)->key, (k)), sizeof(t)->key, 0);
		(t)[stbds_temp((t)-1)].key = (k);
		(t)[stbds_temp((t)-1)].value = (v);
	}

	return hash;
}

static const char* stb_pool_cstring(dbj_handle key_)
{
	int it = stbds_hmgeti(stb_pool, key_);

	if (it == -1) return NULL;

	struct stb_pool_item el = stb_pool[it];
	return el.value;
}

static bool stb_pool_remove(dbj_handle key_)
{
	//	If 'key' is in the hashmap, deletes its entry and returns 1.
//	Otherwise returns 0.
	return stbds_hmdel(stb_pool, key_);
}

static size_t stb_pool_count() {

#ifdef _DEBUG
	if (stb_pool) {
		stbds_array_header* head = stbds_header((stb_pool)-1);
		return head->length - 1;
	}
	else {
		return 0;
	}
#else
	return stbds_hmlenu(stb_pool);
#endif
}

static void stb_pool_reset() {

	// erase the previous hash map
	// otherwise this will be a single global us gustavson_pool_
	// which is sometimes a "good thing"
	if (stb_pool) {
		stbds_hmfree(stb_pool);
		stb_pool = NULL;
	}
}

/* not part of interface , aka "private" *******************************************************************/
static struct dbj_us_pool_interface* stb_pool_instance_ = NULL;

static struct dbj_us_pool_interface* stb_pool_construct()
{
	if (!stb_pool_instance_) {
		stb_pool_instance_ =
			(struct dbj_us_pool_interface*)calloc(1, sizeof(struct dbj_us_pool_interface));

		assert(stb_pool_instance_);

		stb_pool_instance_->add = stb_pool_add;
		stb_pool_instance_->cstring = stb_pool_cstring;
		stb_pool_instance_->count = stb_pool_count;
		stb_pool_instance_->remove = stb_pool_remove;
		stb_pool_instance_->reset = stb_pool_reset;
	}

	return stb_pool_instance_;
}

// clang destruction
__attribute__((destructor))
static void stb_pool_destruct(void)
{
	if (stb_pool_instance_)
	{
		hmfree(stb_pool);
		stb_pool = NULL;
		free(stb_pool_instance_);
		stb_pool_instance_ = NULL;
	}
}

#pragma endregion

/****************************************************************************************/
#pragma region Gustavson unique strings pool 
// gustavson has a complete implementation. but.
// it required effort to configure it 
// otherwise it is extremely slow
// and it seems it does not work properly?
// has to be investigated
// its private hash function looks simplistic?

#define STRPOOL_IMPLEMENTATION
#include <mg/strpool.h>

// this cofig was found to be necessary 
// so that gustavson gustavson_pool_ has decent speed
//  strpool_default_config provokes MUCH worse results?
static strpool_config_t const gustavson_strpool_config =
{
	/* memctx         = */ 0,
	/* ignore_case    = */ 0,
	/* counter_bits   = */ 0xF,
	/* index_bits     = */ 0xF,
	/* entry_capacity = */ 0xF,
	/* block_capacity = */ 0xF,
	/* block_size     = */ 0xF,
	/* min_length     = */ 0xF
};

// gustavson handle type is STRPOOL_U64;
// dbj_handle is size_t

static strpool_t gustavson_pool_;

static dbj_handle gustavson_add(const char* str_)
{
#ifdef _DEBUG
	size_t dummy = gustavson_pool_.entry_count;
	(void)dummy;
#endif
	return strpool_inject(&gustavson_pool_,
		str_,
		(int)strlen(str_)
	);
}

// returns nullptr if not found
static const char* gustavson_cstring(dbj_handle h_) {
	return strpool_cstr(&gustavson_pool_, h_);
}

// returns true if found
static bool gustavson_remove(dbj_handle h_) {

	if (NULL == strpool_cstr(&gustavson_pool_, h_)) return false;
	strpool_discard(&gustavson_pool_, h_);
	return true;
}

static size_t gustavson_count()
{
	return gustavson_pool_.entry_count;
}

static void gustavson_reset() {
	// erase the previous gustavson hash map
	// otherwise this will be a single global ever growing pool
	// which is sometimes a "good thing" but not now
	strpool_term(&gustavson_pool_);
	strpool_init(&gustavson_pool_, &gustavson_strpool_config);
}

static struct dbj_us_pool_interface* gustavson_instance_ = NULL;

static struct dbj_us_pool_interface* gustavson_construct()
{
	if (!gustavson_instance_) {
		gustavson_instance_ =
			(struct dbj_us_pool_interface*)calloc(1, sizeof(struct dbj_us_pool_interface));

		assert(gustavson_instance_);

		gustavson_instance_->add = gustavson_add;
		gustavson_instance_->cstring = gustavson_cstring;
		gustavson_instance_->count = gustavson_count;
		gustavson_instance_->remove = gustavson_remove;
		gustavson_instance_->reset = gustavson_reset;
	}

	return gustavson_instance_;
}

// clang destruction
__attribute__((destructor))
static void gustavson_destruct(void)
{
	if (gustavson_instance_)
	{
		strpool_term(&gustavson_pool_);
		free(gustavson_instance_);
		gustavson_instance_ = NULL;
	}
}

#undef STRPOOL_IMPLEMENTATION
#pragma endregion 
/****************************************************************************************************************/
// we could make a shared single instance
// static dbj_evergrowing_ustring_pool singleinst_ { . . . };

// or we could make a new instance on heap on each call
// which might turn complicated and not just becaue of freeing
// but also because of "instance based data" mechanism potentialy required
// dbj_evergrowing_ustring_pool * new_inst_ 
//       = (dbj_evergrowing_ustring_pool *)malloc( sizeof(dbj_evergrowing_ustring_pool)  ) ;
// now use the construct method  which in turn will require a "data pointer" to the per-instance data struct

// we CAN NOT simply create in o stack here and retun a copy
// becuase we need to return void *
// which is required to allow to type agnostic factory

// master factory 
// typedef enum implementations_registry { STB_POOL } implementations_registry;

void* uspool_factory(uspool_implementations_registry which_)
{
	switch (which_) {
	case US_STB_POOL:
	{
		return stb_pool_construct();
	}
	break;
	case US_GUSTAVSON_POOL:
	{
		return gustavson_construct();
	}
	break;
	default:
	{
		perror(__FILE__ "\nImplementation not found?");
		assert(0);
	}
	}
	return NULL; // in release return null if interface not found
}

