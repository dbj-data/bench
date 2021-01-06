
// the architecture
// pools are divided in to categories
// woth or without removal ability

// if pool can not be removed from
// there are no invalid handles
// (unless purposefully made and sumbited)
// that also makes posible to use `views` as handles
// e.g std::string_view, or even just const char * ptr_ 
template< typename engine_type >
struct evergrowing_ustring_pool_interface
{
	using type = evergrowing_ustring_pool_interface;
	using handle = typename engine_type::handle;

	// this implies engine defauly ctor
	engine_type engine{};

	handle add(const char* str_) noexcept {
		return engine.add(str_);
	}

	// returns nullptr if not found
	const char* cstring(handle h_) noexcept {
		return engine.cstring(h_);
	}

	size_t count() noexcept {
		return engine.count();
	}
}; // ustring_pool_interface

template< typename engine_type >
struct ustring_pool_interface final : public evergrowing_ustring_pool_interface<engine_type>
{
	using parent = evergrowing_ustring_pool_interface<engine_type>;

	// ability of removal implies handle can be "dangling"
	// "dangling" is a handle to the removed item
	// 
	// returns true if found, 
	// after this the handle becomes invalid aka "dangling handle"
	bool remove(parent::handle h_) noexcept {
		return parent::engine.remove(h_);
	}


}; // ustring_pool_interface


#include <dbj/dbj_ustrings.h>
#include <uthash/uthash.h>


#include <ctime>
#include <cassert>
#include <string>
#include <string_view>
#include <set>

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
			return itm_->key;
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

struct gustavson_pool_ final
{
	using type = gustavson_pool_;
	using handle = STRPOOL_U64;

	strpool_t pool{};

	explicit gustavson_pool_() noexcept {
		static strpool_config_t conf = strpool_default_config;
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
		return h.data();
	}
	bool remove(handle h) noexcept {
		Item* item{};
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
		Item* item{}, * tmp{};
		HASH_ITER(hh, set_, item, tmp) {
			delete item;
		}
#ifdef _MSC_VER
#ifdef NDEBUG
		HASH_CLEAR(hh, set_);
#endif
#else  // ! _MSC_VER
		HASH_CLEAR(hh, set_);
#endif // _MSC_VER
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

using namespace std::string_view_literals;

struct meta final {
	constexpr static auto hello = "hello"sv;
	constexpr static auto goodbye = "goodbye"sv;
	constexpr static auto jello = "jello"sv;

	constexpr static bool is_hello(const char* specimen_) { return hello == specimen_; }
	constexpr static bool is_goodbye(const char* specimen_) { return goodbye == specimen_; }
	constexpr static bool is_jello(const char* specimen_) { return jello == specimen_; }

	constexpr static bool match(const char* specimen_) {
		if (!specimen_) return false;
		if (is_hello(specimen_)) return true;
		if (is_goodbye(specimen_)) return true;
		if (is_jello(specimen_)) return true;
		return false;
	}
};

// common means no removal called
// return handles obtained
// notice how types above have no mechanisms to get  handles to the callers
// explicitly, but instead of complicating them we solve that on the calling side
template<typename pool_type>
inline auto test_common(pool_type& pool)
{

	auto hello_h = pool.add(meta::hello.data());
	auto goodbye_h = pool.add(meta::goodbye.data());

	// test the retrieval
	assert(meta::is_hello(pool.cstring(hello_h)));
	assert(meta::is_goodbye(pool.cstring(goodbye_h)));

	// add two more hello's
	(void)pool.add(meta::hello.data());
	(void)pool.add(meta::hello.data());
	// count must stay the same
	assert(pool.count() == 2);
	// add new 
	auto jello_h = pool.add(meta::jello.data());
	// count must have incremented 
	assert(pool.count() == 3);

	struct handles_ final {
		typename pool_type::handle hello;
		typename pool_type::handle goodbye;
		typename pool_type::handle jello;
	};

	return handles_{ hello_h , goodbye_h, jello_h };
};

template<typename the_pool_type >
inline void test_evergrowing(void) noexcept
{
	evergrowing_ustring_pool_interface<the_pool_type> pool;
	(void)test_common(pool);
}

template<class the_pool_type >
inline void test_removal(the_pool_type& sp) noexcept
{
	ustring_pool_interface<the_pool_type> pool{};

	auto [hello_h, goodbye_h, jello_h] = test_common(pool);

	bool was_there = pool.remove(hello_h);
	assert(was_there);
	// check if "hello" has been removed
	assert(pool.cstring(hello_h) == nullptr);
	assert(pool.count() == 2);

	// check if the rest has stayed
	assert(meta::is_goodbye(pool.cstring(goodbye_h)));
	assert(meta::is_jello(pool.cstring(jello_h)));
	assert(pool.count() == 2);

	// try removing the removed one
	was_there = pool.remove(hello_h);
	assert(false == was_there);
	// now remove the rest
	was_there = pool.remove(goodbye_h);
	assert(was_there);
	was_there = pool.remove(jello_h);
	assert(was_there);
	assert(pool.count() == 0);
}


/****************************************************************************************/

#include "../ubut/ubench.h"

UBENCH(strpool_evergrowing, set_of_strings) {
	test_evergrowing<strings_set_pool>();
}

UBENCH(strpool_evergrowing, uthash_string_pool) {
	test_evergrowing<uthash_pool>();
}

UBENCH(strpool_evergrowing, uthash_using_vector) {
	test_evergrowing<uthash_vector_pool>();
}

UBENCH(strpool_evergrowing, uthash_using_unique_ptr) {
	test_evergrowing<uthash_up_pool>();
}

UBENCH(strpool, gustavson_string_pool) {
	gustavson_pool_ pool{};
	(void)test_removal(pool);
}

UBENCH(strpool, stb_pool_bench) {
	stb_pool_ pool{};
	(void)test_removal(pool);
}

UBENCH(strpool, dbj_unique_strings) {
	dbj_ustrings pool{};
	(void)test_removal(pool);
}


// Explicits
//template struct ustring_pool_interface<strings_set_pool>;
//template struct ustring_pool_interface<uthash_pool>;
//template struct ustring_pool_interface<dbj_ustrings>;
//template struct ustring_pool_interface<uthash_vector_pool>;
//template struct ustring_pool_interface<uthash_up_pool>;
//template struct ustring_pool_interface<gustavson_pool_>;
//template struct ustring_pool_interface<stb_pool_>;

