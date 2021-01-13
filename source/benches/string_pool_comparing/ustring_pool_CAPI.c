#include "../../ustring_pool_interface_CAPI.h"


/****************************************************************************************/
// #define STB_DS_IMPLEMENTATION
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

const char* stb_pool_cstring(dbj_handle key_)
{
	int it = stbds_hmgeti(stb_pool, key_);

	if (it == -1) return NULL;

	struct stb_pool_item el = stb_pool[it];
	return el.value;
}

bool stb_pool_remove(dbj_handle key_)
{
	//	If 'key' is in the hashmap, deletes its entry and returns 1.
//	Otherwise returns 0.
	return hmdel(stb_pool, key_);
}


size_t stb_pool_count() { return hmlenu(stb_pool); }

/* not part of interface , aka "private" *******************************************************************/
static struct dbj_us_pool_interface* instance_ = NULL;

static struct dbj_us_pool_interface* stb_pool_construct()
{
	if (!instance_) {
		instance_ =
			(struct dbj_us_pool_interface*)calloc(1, sizeof(struct dbj_us_pool_interface));

		assert(instance_);

		instance_->add = stb_pool_add;
		instance_->cstring = stb_pool_cstring;
		instance_->count = stb_pool_count;
		instance_->remove = stb_pool_remove;
	}

	// before you leave, erase the previous hash map
	// otherwise this will be a single global us pool
	// which is sometimes a "good thing"
	if (stb_pool != NULL) {
		hmfree(stb_pool);
		stb_pool = NULL;
	}

	return instance_;
}

// clang destruction
__attribute__((destructor))
static void stb_pool_destruct(void)
{
	if (instance_)
	{
		hmfree(stb_pool);
		stb_pool = NULL;
		free(instance_);
		instance_ = NULL;
	}
}

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
	default:
	{
		perror(__FILE__ "\nImplementation not found?");
		assert(0);
	}
	}
	return NULL; // in release return null if interface not found
}

