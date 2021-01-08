#pragma once

// (c) 2021 by dbj@dbj.org https://dbj.org/license_dbj
// the architecture
// pools are divided in to categories
// woth or without removal ability

// if pool insertions discards duplicates
// that pools size will not grow
// not as much as pool of duplicates
// also
// if pool can not be removed from
// there are no invalid handles
// (unless purposefully made and sumbited)
// that also makes posible to use `views` as handles
// e.g std::string_view, or even just const char * ptr_ 
// this makes for very simple and fast pools
namespace dbj {
	template< typename engine_type >
	struct evergrowing_ustring_pool_interface
	{
		using type = evergrowing_ustring_pool_interface;
		using handle = typename engine_type::handle;

		// this implies engine default ctor
		// has to exist
		engine_type engine{};

		// must not insert if already exist
		// return ahndle to the entry made 
		// or just found
		// thus there is no concept of
		// invalid handle coming out of here
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

	// this is still the pool of unique entries
	// but with the ability to remove them
	// thus the problem arises:
	// dangling handles
	template< typename engine_type >
	struct ustring_pool_interface final
		: public evergrowing_ustring_pool_interface<engine_type>
	{
		using parent = evergrowing_ustring_pool_interface<engine_type>;

		// ability of removal implies handle can be "dangling"
		// "dangling" is a handle to the removed item
		// 
		// returns true if found, 
		// after this the handle becomes invalid aka "dangling handle"
		// funny thing is: if we maintain internally unique
		// evergrowing pool of dangling
		// handles we can use that to a great effect
		bool remove(parent::handle h_) noexcept {
			return parent::engine.remove(h_);
		}
	}; // ustring_pool_interface

	// TODO: locking policy. NULL policy does no locking.

} // namespace dbj 

/****************************************************************************************/
// testing stuff 
// all implementations must go through here
// all using the same testing metadata

#include <cassert>
#include <string>
#include <string_view>
#include <set>
#include <array>

namespace /*ustring_pool_testing*/ {
#undef  PRINTF
#define PRINTF(...) 	fprintf(stderr, __VA_ARGS__);  

#define DBJ_NME_1(x, y) x##y
#define DBJ_NME_2(x, y) DBJ_NME_1(x, y)
#define DBJ_NME_3(x)    DBJ_NME_2(x, __COUNTER__)

#undef DBJ_VERIFY
#define DBJ_VERIFY(x) do {          \
  bool rzlt = (x) ;					\
  if (! rzlt) {                     \
	PRINTF("\nNot true: %s", #x);   \
	PRINTF("\nPool type: %s", typeid(pool).name()); \
	PRINTF("\n%s()", __func__);     \
	PRINTF("\n%s(%4u)\n", __FILE__, __LINE__);      \
	exit(0) ;                       \
  }                                 \
} while(0)
	// one test covers all
	// regardless of the fact
	// handles are different types

	using namespace std::string_view_literals;

	struct testing_meta_data final
	{
		using type = testing_meta_data;
		constexpr static auto hello = "hello"sv;
		constexpr static auto goodbye = "goodbye"sv;
		constexpr static auto jello = "jello"sv;

		constexpr static bool is_hello(const char* specimen_) { return hello == specimen_; }
		constexpr static bool is_goodbye(const char* specimen_) { return goodbye == specimen_; }
		constexpr static bool is_jello(const char* specimen_) { return jello == specimen_; }
	};

	using tmd = testing_meta_data;

	namespace detail {

		// test the pool to char * retrieval
		// compare to testing_meta_data data
		template<typename pool, typename handle >
		inline bool match(pool& pool_, handle handle_) {
			const char* specimen_ = pool_.cstring(handle_);
			if (!specimen_) return false;
			if (tmd::hello == specimen_) return true;
			if (tmd::goodbye == specimen_) return true;
			if (tmd::jello == specimen_) return true;
			return false;
		}

		// common means no removal functionality
		template<typename pool_type>
		inline void test_common(pool_type& pool) noexcept
		{
			auto hello_h = pool.add(tmd::hello.data());
			auto goodbye_h = pool.add(tmd::goodbye.data());

			// test the retrieval
			DBJ_VERIFY(match(pool, hello_h));
			DBJ_VERIFY(match(pool, goodbye_h));

			// add two more hello's
			(void)pool.add(tmd::hello.data());
			(void)pool.add(tmd::hello.data());
			// count must stay the same
			DBJ_VERIFY(pool.count() == 2);
			// add new 
			auto jello_h = pool.add(tmd::jello.data());
			// count must have incremented 
			DBJ_VERIFY(pool.count() == 3);
		};
	} // nspace 

	template<typename the_pool_type >
	inline void test_evergrowing(void) noexcept
	{
		dbj::evergrowing_ustring_pool_interface<the_pool_type> pool;
		detail::test_common(pool);
		// "interface" destructor is called here
		// *not* the pool type destructor
		// that is decoupling and encapsulation in action
	}

	template<class the_pool_type >
	inline void test_removal(void) noexcept
	{
		dbj::ustring_pool_interface<the_pool_type> pool{};
		detail::test_common(pool);

		auto hello_h = pool.add(tmd::hello.data());
		auto goodbye_h = pool.add(tmd::goodbye.data());
		auto jello_h = pool.add(tmd::jello.data());

		DBJ_VERIFY(pool.remove(hello_h));
		// check if "hello" has been removed
		DBJ_VERIFY(pool.cstring(hello_h) == nullptr);
		DBJ_VERIFY(pool.count() == 2);

		using namespace detail;

		// check if the rest has stayed
		DBJ_VERIFY(match(pool, goodbye_h));
		DBJ_VERIFY(match(pool, jello_h));
		DBJ_VERIFY(pool.count() == 2);

		// try removing the removed one
		DBJ_VERIFY(false == pool.remove(hello_h));
		// now remove the rest
		DBJ_VERIFY(pool.remove(goodbye_h));
		DBJ_VERIFY(pool.remove(jello_h));
		DBJ_VERIFY(pool.count() == 0);
	}

} // namespace ustring_pool_testing