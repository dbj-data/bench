#ifndef TH_TESTING_COMMON_INC
#define TH_TESTING_COMMON_INC

/****************************************************************************************/
// testing stuff 
// all implementations must go through here
// all using the same testing metadata

#include <assert.h>
#include <string.h>
#include <stdio.h>

#undef  PRINTF
#define PRINTF(...) 	fprintf(stderr, __VA_ARGS__);  

#undef  DBJ_NME_1
#undef  DBJ_NME_2
#undef  DBJ_NME_3

#define DBJ_NME_1(x, y) x##y
#define DBJ_NME_2(x, y) DBJ_NME_1(x, y)
#define DBJ_NME_3(x)    DBJ_NME_2(x, __COUNTER__)

// UST_VERIFY == Uniqaue Strings Testing  Verify

#undef UST_VERIFY
#define UST_VERIFY(x) do {          \
  bool rzlt = (x) ;					\
  if (! rzlt) {                     \
	PRINTF("\nNot true: %s", #x);   \
	PRINTF("\n%s(%4u)\n", __FILE__, __LINE__);      \
	exit(0) ;                       \
  }                                 \
} while(0)


#ifndef  __cplusplus

#include "ustring_pool_interface_CAPI.h"

typedef struct us_testing_meta_data
{
	const char* const hello;
	const char* const goodbye;
	const char* const jello;

	bool (*is_hello)  (const char*);
	bool (*is_goodbye)(const char*);
	bool (*is_jello)  (const char*);
} us_testing_meta_data;

extern struct us_testing_meta_data usmd;

// implementation is here,  but could be hidden in some c file

static bool is_hello(const char* s) { assert(s);  return !strcmp(s, usmd.hello); }
static bool is_goodbye(const char* s) { assert(s); return !strcmp(s, usmd.goodbye); }
static bool is_jello(const char* s) { assert(s); return !strcmp(s, usmd.jello); }

static us_testing_meta_data usmd = {
   .hello = "Hello",
   .goodbye = "Goodbye",
   .jello = "Jello",
   .is_hello = is_hello ,
   .is_goodbye = is_goodbye ,
   .is_jello = is_jello
};

inline void test_common(const struct dbj_evergrowing_ustring_pool* pool)
{
	dbj_handle hello_h = pool->add(usmd.hello);
	dbj_handle goodbye_h = pool->add(usmd.goodbye);

	// test the retrieval
	UST_VERIFY(usmd.is_hello(pool->cstring(hello_h)));
	UST_VERIFY(usmd.is_goodbye(pool->cstring(goodbye_h)));

	// add two more hello's
	(void)pool->add(usmd.hello);
	(void)pool->add(usmd.hello);
	// count must stay the same
	UST_VERIFY(pool->count() == 2);
	// add new 
	pool->add(usmd.jello);
	// count must have incremented 
	UST_VERIFY(pool->count() == 3);
};

inline void test_removal(const struct dbj_us_pool_interface* pool)
{
	dbj_handle hello_h = pool->add(usmd.hello);
	dbj_handle goodbye_h = pool->add(usmd.goodbye);
	dbj_handle jello_h = pool->add(usmd.jello);

	UST_VERIFY(pool->remove(hello_h));
	// check if "hello" has been removed
	UST_VERIFY(pool->cstring(hello_h) == NULL);
	UST_VERIFY(pool->count() == 2);

	// check if the rest has stayed
	UST_VERIFY(usmd.is_goodbye(pool->cstring(goodbye_h)));
	UST_VERIFY(usmd.is_jello(pool->cstring(jello_h)));
	UST_VERIFY(pool->count() == 2);

	// try removing the removed one
	UST_VERIFY(false == pool->remove(hello_h));
	// now remove the rest
	UST_VERIFY(pool->remove(goodbye_h));
	UST_VERIFY(pool->remove(jello_h));
	UST_VERIFY(pool->count() == 0);
}

#endif // ! __cplusplus

#ifdef __cplusplus

#include <string_view>
#include <set>
#include <array>
#include <string>


#include "ustring_pool_interface.h"

namespace /*ustring_pool_testing*/ {

#undef UST_VERIFY
#define UST_VERIFY(x) do {          \
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
			UST_VERIFY(match(pool, hello_h));
			UST_VERIFY(match(pool, goodbye_h));

			// add two more hello's
			(void)pool.add(tmd::hello.data());
			(void)pool.add(tmd::hello.data());
			// count must stay the same
			UST_VERIFY(pool.count() == 2);
			// add new 
			/*auto jello_h = */pool.add(tmd::jello.data());
			// count must have incremented 
			UST_VERIFY(pool.count() == 3);
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

		UST_VERIFY(pool.remove(hello_h));
		// check if "hello" has been removed
		UST_VERIFY(pool.cstring(hello_h) == nullptr);
		UST_VERIFY(pool.count() == 2);

		using namespace detail;

		// check if the rest has stayed
		UST_VERIFY(match(pool, goodbye_h));
		UST_VERIFY(match(pool, jello_h));
		UST_VERIFY(pool.count() == 2);

		// try removing the removed one
		UST_VERIFY(false == pool.remove(hello_h));
		// now remove the rest
		UST_VERIFY(pool.remove(goodbye_h));
		UST_VERIFY(pool.remove(jello_h));
		UST_VERIFY(pool.count() == 0);
	}

#undef UST_VERIFY

} // namespace ustring_pool_testing

#endif // __cplusplus

#endif // !TH_TESTING_COMMON_INC
