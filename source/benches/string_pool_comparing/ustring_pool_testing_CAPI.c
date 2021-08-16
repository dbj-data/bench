
#include "common/ht_testing_common.h"
#include "../../../ubut/ubench.h"


/*************************************************************************************************************/
// implementation is here,  but could be hidden in some c file

static bool is_hello_(const char* s) { assert(s);  return !strcmp(s, usmd.hello); }
static bool is_goodbye_(const char* s) { assert(s); return !strcmp(s, usmd.goodbye); }
static bool is_jello_(const char* s) { assert(s); return !strcmp(s, usmd.jello); }

struct us_testing_meta_data usmd = {
   .hello = "Hello",
   .goodbye = "Goodbye",
   .jello = "Jello",
   .is_hello = is_hello_ ,
   .is_goodbye = is_goodbye_ ,
   .is_jello = is_jello_
};

/*************************************************************************************************************/
inline const struct dbj_us_pool_interface* stb_pool()
{
	struct   dbj_us_pool_interface* stb_pool_instance = NULL;

	// make it once
	if (stb_pool_instance == NULL) {
		stb_pool_instance =
			(struct dbj_us_pool_interface*)uspool_factory(US_STB_POOL);
	}

	// reset it on each call
	stb_pool_instance->reset();
	return stb_pool_instance;
}


UBENCH(strpool, stb_pool_CAPI)
{
	const struct dbj_us_pool_interface* pimp_ = stb_pool();

	UST_VERIFY(pimp_);

	// notice the cast to another interface implementation
	test_common((const struct dbj_evergrowing_ustring_pool*)pimp_);
	// before next test
	pimp_->reset();
	test_removal(pimp_);
}

/*************************************************************************************************************/
inline const struct dbj_us_pool_interface* gustavson_pool()
{
	struct   dbj_us_pool_interface* gustavson_pool_instance = NULL;
	if (gustavson_pool_instance == NULL) {
		gustavson_pool_instance =
			(struct dbj_us_pool_interface*)uspool_factory(US_GUSTAVSON_POOL);
	}
	assert(gustavson_pool_instance);
	gustavson_pool_instance->reset();
	return gustavson_pool_instance;
}


UBENCH(strpool, gustavson_pool_CAPI)
{
	const struct dbj_us_pool_interface* pimp_ = gustavson_pool();

	UST_VERIFY(pimp_);

	// notice the cast to another interface implementation
	test_common((const struct dbj_evergrowing_ustring_pool*)pimp_);
	// before next test
	pimp_->reset();
	test_removal(pimp_);
}