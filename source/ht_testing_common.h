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
#define PRINTF(...)	fprintf(stderr, __VA_ARGS__);  

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


#include "ustring_pool_interface_CAPI.h"

#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus

	// same for C or C++ code
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

	inline bool match(const char* specimen_) {
		if (!specimen_) return false;
		if (usmd.is_hello(specimen_))	return true;
		if (usmd.is_goodbye(specimen_)) return true;
		if (usmd.is_jello(specimen_))	return true;
		return false;
	}

	//C API for testing

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

#ifdef  __cplusplus
} // extern "C" 
#endif //  __cplusplus


#ifdef __cplusplus

#include <string_view>
#include <set>
#include <array>
#include <string>


#include "ustring_pool_interface.h"

namespace /*ustring_pool_testing*/ {

#undef UST_VERIFY

#ifdef _DEBUG
#define UST_VERIFY _ASSERTE
#else // release 
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
#endif // release build

	namespace detail {

		// one test covers all
		// regardless of the fact
		// handles are different types

		// common means no removal functionality
		template<typename pool_type>
		inline void test_common(void) noexcept
		{
			pool_type pool{};

			auto hello_h = pool.add(usmd.hello);
			auto goodbye_h = pool.add(usmd.goodbye);

			// test the retrieval
			UST_VERIFY(match(pool.cstring(hello_h)));
			UST_VERIFY(match(pool.cstring(goodbye_h)));

			// add two more hello's
			(void)pool.add(usmd.hello);
			(void)pool.add(usmd.hello);
			// count must stay the same
			UST_VERIFY(pool.count() == 2);
			// add new 
			/*auto jello_h = */pool.add(usmd.jello);
			// count must have incremented 
			UST_VERIFY(pool.count() == 3);
		};
	} // nspace 

	template<typename the_pool_type >
	inline void test_evergrowing(void) noexcept
	{
		dbj::evergrowing_ustring_pool_interface<the_pool_type> pool;
		detail::test_common< dbj::evergrowing_ustring_pool_interface<the_pool_type> >();
		// "interface" destructor is called here
		// *not* the pool type destructor
		// that is decoupling and encapsulation in action
	}

	template<class the_pool_type >
	inline void test_removal(void) noexcept
	{
		using Ius_pool = dbj::ustring_pool_interface<the_pool_type>;
#ifdef _DEBUG
		bool stop_here{}; (void)stop_here;
#endif
		// common tests
		detail::test_common< Ius_pool >();
		// the rest is here
		Ius_pool pool{};

		auto hello_h = pool.add(usmd.hello);
		auto goodbye_h = pool.add(usmd.goodbye);
		auto jello_h = pool.add(usmd.jello);

		UST_VERIFY(pool.remove(hello_h));
		// check if "hello" has been removed
		UST_VERIFY(pool.cstring(hello_h) == nullptr);
		UST_VERIFY(pool.count() == 2);

		using namespace detail;

		// check if the rest has stayed
		UST_VERIFY(match(pool.cstring(goodbye_h)));
		UST_VERIFY(match(pool.cstring(jello_h)));
		UST_VERIFY(pool.count() == 2);

		// try removing the removed one
		UST_VERIFY(false == pool.remove(hello_h));
		// now remove the rest
		UST_VERIFY(pool.remove(goodbye_h));
		UST_VERIFY(pool.remove(jello_h));
		UST_VERIFY(pool.count() == 0);

		// Arthur O'Dwyer -- add / remove 1024 strings
		// DBJ - X renamed to SOME_STR_
		auto tostr = [](auto x) { return typeid(x).name(); };
#define SOME_STR_ tostr([]{})
#define X4 SOME_STR_, SOME_STR_, SOME_STR_, SOME_STR_
#define X16 X4, X4, X4, X4
#define X64 X16, X16, X16, X16
#define X256 X64, X64, X64, X64
#define X1024 X256, X256, X256, X256

		static const char* strings_[] = { X1024 };
		for (const char* s : strings_) {
			pool.add(s);
		}
		for (const char* s : strings_) {
			pool.add(s);
		}
		assert(pool.count() == 1024);
		for (const char* s : strings_) {
			pool.remove(pool.add(s));
		}
		assert(pool.count() == 0);
	} // test_removal
#undef SOME_STR_
#undef X4
#undef X16
#undef X64
#undef X256
#undef X1024

#undef UST_VERIFY

} // namespace ustring_pool_testing

#endif // __cplusplus

#endif // !TH_TESTING_COMMON_INC
