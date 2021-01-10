#pragma once
#include "../../ustring_pool_interface.h"
#include "../ubut/ubench.h"
#include <slot/slot_array.h> // // by Allan Deutsch
#include <slot/slot_map.h> // by Arthur O'Dwyer
#include <slot/packed_array.h> // by Dusan B. Jovanovic
#include <string>


// unique strings pool
// based on slot_array
struct slot_arr_pool final
{
	using type = slot_arr_pool;
	using pool_type = ADL::slot_array<std::string>;
	using handle = unsigned int;

	pool_type pool{};

	explicit slot_arr_pool() noexcept = default;
	~slot_arr_pool() noexcept = default;

	handle add(const char* str_) noexcept
	{
		for (auto&& it : pool)
		{
			if (it == str_)
			{
				std::string lvalue{ str_ };
				return pool.get_ID(lvalue);
			}
		}
		// not found -- insert
		return	pool.get_ID(pool.alloc(str_));
	}

	// returns nullptr if not found
	const char* cstring(handle key_) noexcept
	{
		pool_type::value_type* str = pool.get_safely(key_);

		if (str) return str->c_str();
		return nullptr;
	}

	// returns true if found
	bool remove(handle key_) noexcept {

		pool_type::value_type* str = pool.get_safely(key_);

		if (str) {
			pool.remove_if([&](pool_type::value_type const& el) { return el == *str; });
			return true;
		}
		return false;
	}

	size_t count() noexcept {
		return pool.size();
	}
}; // slot_arr_pool

/****************************************************************************************/
struct slot_map_pool final
{
	using type = slot_map_pool;
	using pool_type = stdext::slot_map<std::string>;
	using handle = typename pool_type::key_type;

	pool_type pool{};

	explicit slot_map_pool() noexcept = default;
	~slot_map_pool() noexcept = default;

	handle add(const char* str_) noexcept {
		// this is not unique insert
		return pool.insert(str_);
	}

	// returns nullptr if not found
	const char* cstring(handle key_) noexcept
	{
		//fast is dangerous
		return pool[key_].c_str();
#if 0
		// the slow version
		auto it = pool.find(key_);
		if (it != pool.end()) return it->c_str();
		return nullptr;
#endif // 0
	}

	// returns true if found
	bool remove(handle key_) noexcept {
		return pool.erase(key_);
	}

	size_t count() noexcept {
		return pool.slot_count();
	}
}; // slot_map_pool


/*
at the bottom define the BENCH-es
all here and all in one place
this we can add/remove them

this is used from all_in_one.cpp in this same folder
*/
/*

slot_map on its own has no unique insert

UBENCH(strpool, slot_map_pool_bench) {
	test_removal<slot_map_pool>();
}
*/

UBENCH(strpool, slot_array_pool_bench) {
	test_removal<slot_arr_pool>();
}
