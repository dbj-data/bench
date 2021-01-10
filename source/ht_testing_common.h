#pragma once

#include "ustring_pool_interface.h"

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
			/*auto jello_h = */pool.add(tmd::jello.data());
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