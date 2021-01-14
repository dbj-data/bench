
#include "../../ht_testing_common.h"
#include <ubut/ubench.h>



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