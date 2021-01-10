#pragma once
#include "../../ustring_pool_interface.h"
#include "../ubut/ubench.h"
#include <uthash/uthash.h>

#include <dbj/dbj_ustrings.h> // dbj::string_ptr
/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer
struct uthash_pool final {
	using handle = std::string_view;
	struct Item {
		std::string data;
		UT_hash_handle hh;
	};
	Item* set_{};
	handle add(const char* s) {
		Item* item;
		HASH_FIND_STR(set_, s, item);
		if (item == nullptr) {
			item = new Item{ s, {} };
			HASH_ADD_STR(set_, data.c_str(), item);
		}
		return item->data;
	}
	const char* cstring(handle h) {
		// handle should be always valid
		// as there is no removal
		return h.data();
	}

	int count() const {
		return HASH_COUNT(set_);
	}
	~uthash_pool() {
		Item* item{}, * tmp{};
		HASH_ITER(hh, set_, item, tmp) {
			delete item;
		}
		HASH_CLEAR(hh, set_);
	}
};

/****************************************************************************************/
// DBJ variant of Arthur O'Dwyer uthash_pool
struct uthash_vector_pool final {
	using handle = std::string_view;
	struct Item {
		std::vector<char> data{};
		UT_hash_handle hh;
	};
	Item* set_ = nullptr;
	handle add(const char* s) {
		Item* item;
		HASH_FIND_STR(set_, s, item);
		if (item == nullptr)
		{
			std::string_view sv(s);
			item = new Item{ {sv.begin(), sv.end()}, {} };
			// CAUTION! made as above data vector has no '\0' termination! ...ditto
			item->data.push_back('\0');
			HASH_ADD_STR(set_, data.data(), item);
		}
		return item->data.data();
	}
	const char* cstring(handle h) {
		return h.data();
	}

	int count() const {
		return HASH_COUNT(set_);
	}
	uthash_vector_pool() = default;
	~uthash_vector_pool() {
		Item* item;
		Item* tmp;
		HASH_ITER(hh, set_, item, tmp) {
			delete item;
		}
		HASH_CLEAR(hh, set_);
	}
};

/****************************************************************************************/
// DBJ variant ofArthur O'Dwyer uthash_pool
struct uthash_up_pool final {

	using handle = std::string_view;

	struct Item {
		dbj::string_ptr data{};
		UT_hash_handle hh;
	};

	Item* set_ = nullptr;

	handle add(const char* s) noexcept {
		Item* item;
		HASH_FIND_STR(set_, s, item);
		if (item == nullptr)
		{
			item = new Item{ dbj::string_ptr_make(s), {} };
			HASH_ADD_STR(set_, data.get(), item);
		}
		return item->data.get();
	}
	const char* cstring(handle h) const noexcept {
		return h.data();
	}

	int count() const {
		return HASH_COUNT(set_);
	}
	uthash_up_pool() noexcept = default;
	~uthash_up_pool() noexcept {
		Item* item;
		Item* tmp;
		HASH_ITER(hh, set_, item, tmp) {
			delete item;
		}
		HASH_CLEAR(hh, set_);
	}
};

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
