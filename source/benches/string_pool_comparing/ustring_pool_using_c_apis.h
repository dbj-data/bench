#pragma once

#include "../../ustring_pool_interface.h"
#include "../ubut/ubench.h"
#include <ctime>

/****************************************************************************************/
#define STB_DS_IMPLEMENTATION
#include <stb/stb_ds.h>

struct stb_pool_ final
{
	using type = stb_pool_;
	using handle = char*;

	constexpr static inline size_t value_max_len = 0xFF;

	// value is the hash but do we need it at all?
	struct item { char* key; size_t value; };

	item* pool{};

	static size_t seed() { static /*time_t*/ __int64 seed_ = time(0); return seed_; }

	// hash value for a string.
	static size_t string_hash(char* str_) {
		return stbds_hash_string(str_, type::seed());
	}

	explicit stb_pool_() noexcept {
	}

	~stb_pool_() noexcept {
		shfree(pool);
	}


	handle add(const char* str_) noexcept {

		item* itm_ = shgetp_null(pool, str_);

		static int whatever = 42; /*string_hash((char*)str_)*/

		if (!itm_) {
			//	Inserts a <key, value> pair into the hashmap.If the key is already
			//	present in the hashmap, updates its value.
			size_t control_ = shput(pool, (char*)str_, whatever);
			(void)control_;
		}
		return (handle)str_;
	}

	// returns nullptr if not found
	const char* cstring(handle key_) noexcept {
		item* itm_ = shgetp_null(pool, key_);
		if (itm_)
			return key_;
		return nullptr;
	}

	// returns true if found
	bool remove(handle key_) noexcept {
		//	If 'key' is in the hashmap, deletes its entry and returns 1.
		//	Otherwise returns 0.
		return shdel(pool, key_);
	}

	size_t count() noexcept {
		return shlenu(pool);
	}
}; // stb_pool_

#undef STB_DS_IMPLEMENTATION
/****************************************************************************************/
#define STRPOOL_IMPLEMENTATION
#include <mg/strpool.h>

namespace gustavson {
	inline strpool_config_t const strpool_config =
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

	//  strpool_default_config provokes MUCH worse results?
}

struct gustavson_pool_ final
{
	using type = gustavson_pool_;
	using handle = STRPOOL_U64;

	strpool_t pool{};

	explicit gustavson_pool_() noexcept
	{
		strpool_init(&pool, &gustavson::strpool_config);
	}

	~gustavson_pool_() noexcept {
		strpool_term(&pool);
	}


	handle add(const char* str_) noexcept {
		return strpool_inject(&pool,
			str_,
			(int)strlen(str_)
		);
	}

	// returns nullptr if not found
	const char* cstring(handle h_) const noexcept {
		return strpool_cstr(&pool, h_);
	}

	// returns true if found
	bool remove(handle h_) noexcept {
		strpool_discard(&pool, h_);
		return true;
	}

	size_t count() noexcept {
		// char* strpool_collate(strpool_t const* pool, int* count)
		int count_ = 0;
		(void)strpool_collate(&pool, &count_);
		return (size_t)count_;
	}
}; // gustavson_pool_

#undef STRPOOL_IMPLEMENTATION
/*
at the bottom define the BENCH-es
all here and all in one place
this we can add/remove them

this is used from all_in_one.cpp in this same folder
*/

// gustavson has a complete implementation. but.
// it required effort to configure it 
// otherwise it is extremely slow
// and it seems it does not work properly?
// has to be investigated
// it private hash function looks simplistic?
#if 0
UBENCH(strpool, gustavson_string_pool) {
	test_removal<gustavson_pool_>();
}
#endif // 0

// stb pool works ok and is not slow
// and it has removal 
UBENCH(strpool, stb_pool_bench) {
	test_removal<stb_pool_>();
}