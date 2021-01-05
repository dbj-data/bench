
// the interface
template< typename engine_type >
struct ustring_pool_interface final
{
	using type = ustring_pool_interface;
	using handle = typename engine_type::handle;

	engine_type engine{};

	handle add(const char* str_) noexcept {
		return engine.add(str_);
	}

	// returns nullptr if not found
	const char* cstring(handle h_) noexcept {
		return engine.cstring(h_);
	}

	// returns true if found
	bool remove(handle h_) noexcept {
		return engine.remove(h_);
	}

	size_t count() noexcept {
		return engine.count();
	}
}; // ustring_pool_interface


#include <dbj/dbj_ustrings.h>
#include <uthash/uthash.h>


#include <cassert>
#include <string>
#include <string_view>
#include <set>

/****************************************************************************************/
#undef STRPOOL_IMPLEMENTATION
#define STRPOOL_IMPLEMENTATION
#include <mg/strpool.h>

struct gustavson_pool_ final
{
	using type = gustavson_pool_;
	using handle = STRPOOL_U64;

	strpool_t pool{};

	explicit gustavson_pool_() noexcept {
		strpool_config_t conf = strpool_default_config;
		strpool_init(&pool, &conf);
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


/****************************************************************************************/
// the handle type 
// in here is different vs the other
// two solutions bellow
// (c) 2021 by dbj@dbj.org
struct dbj_ustrings final {
	using handle = size_t;
	using store_t = dbj::ustrings;
	store_t store_{};

	handle add(const char* s) noexcept {
		return store_t::assign(store_, s);
	}

	const char* cstring(handle h) const noexcept {
		for (auto&& node_ : store_.strings)
		{
			if (h == node_.H)
			{
				return node_.ptr.get();
			}
		}
		return nullptr;
	}

	bool remove(handle h) noexcept {
		return store_t::remove(store_, h);
	}
	size_t count() const noexcept {
		return store_.strings.size();
	}
}; // dbj_ustrings

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer

// different handle type for 
// Arthur's solutions
// it is std::string_view
// thus hash value is never exposed

struct strings_set_pool final {
	using handle = std::string_view;
	std::set<std::string, std::less<> > set_;
	handle add(const char* s) {
		// inserted, just ignore it
		auto [it, inserted] = set_.emplace(s);
		return *it;
	}
	const char* cstring(handle h) {
		return h.data();
	}
	bool remove(handle h) {
		// no sanity check?
		set_.erase(set_.find(h));
		return true;
	}
	size_t count() const {
		return set_.size();
	}
};

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer
struct uthash_pool final {
	using handle = std::string_view;
	struct Item {
		std::string data;
		UT_hash_handle hh;
	};
	Item* set_ = nullptr;
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
		return h.data();
	}
	bool remove(handle h) noexcept {
		Item* item = nullptr;
		HASH_FIND_STR(set_, h.data(), item);
		if (item) {
			HASH_DEL(set_, item);
			delete item;
			return true;
		}
		return false;
	}
	int count() const {
		return HASH_COUNT(set_);
	}
	~uthash_pool() {
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
			HASH_ADD_STR(set_, data.data(), item);
		}
		return item->data.data();
	}
	const char* cstring(handle h) {
		return h.data();
	}
	bool remove(handle h) {
		Item* item = nullptr;
		HASH_FIND_STR(set_, h.data(), item);
		if (item) {
			HASH_DEL(set_, item);
			delete item;
			return true;
		}
		return false;
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

	bool remove(handle h) noexcept {
		Item* item = nullptr;
		HASH_FIND_STR(set_, h.data(), item);
		if (item) {
			HASH_DEL(set_, item);
			delete item;
			return true;
		}
		return false;
	}
	int count() const {
		return HASH_COUNT(set_);
	}
	uthash_up_pool() noexcept = default;
	~uthash_up_pool() noexcept {
		//Item* item;
		//Item* tmp;
		// HASH_ITER(hh, set_, item, tmp) {
		//	 is this required?? -> delete item;
		// }
		HASH_CLEAR(hh, set_);
	}
};

/****************************************************************************************/
// one test covers all
// regardless of the fact
// handles are different types
template<class the_pool>
inline void test() noexcept
{

	// MSVC warning C4455: 
	// 'operator ""sv': literal suffix identifiers that do not start with an underscore are reserved
#pragma warning( suppress: 4455 )
	using std::operator""sv;

	ustring_pool_interface<the_pool> sp{};

	auto hello = sp.add("hello");
	auto goodbye = sp.add("goodbye");
	// NDEBUG removes assert
	assert(sp.cstring(hello) == "hello"sv);
	assert(sp.cstring(goodbye) == "goodbye"sv);
	assert(sp.count() == 2);

	sp.remove(hello);
	// check if "hello" has been removed
	assert(sp.count() == 1);
	// check if "goodbye" has stayed
	assert(sp.cstring(goodbye) == "goodbye"sv);

	// check another "goodbye"
	auto goodbye2 = sp.add("goodbye");
	assert(sp.cstring(goodbye2) == "goodbye"sv);
	// has not been added
	assert(sp.count() == 1);
}

/****************************************************************************************/

#include "../ubut/ubench.h"

UBENCH(strpool, set_of_strings) {
	test<strings_set_pool>();
}

UBENCH(strpool, uthash_string_pool) {
	test<uthash_pool>();
}

UBENCH(strpool, dbj_unique_strings) {
	test<dbj_ustrings>();
}

UBENCH(strpool, uthash_using_vector) {
	test<uthash_vector_pool>();
}

UBENCH(strpool, uthash_using_unique_ptr) {
	test<uthash_up_pool>();
}

UBENCH(strpool, gustavson_string_pool) {
	test<gustavson_pool_>();
}


// Explicits
template struct ustring_pool_interface<strings_set_pool>;
template struct ustring_pool_interface<uthash_pool>;
template struct ustring_pool_interface<dbj_ustrings>;
template struct ustring_pool_interface<uthash_vector_pool>;
template struct ustring_pool_interface<uthash_up_pool>;
template struct ustring_pool_interface<gustavson_pool_>;

