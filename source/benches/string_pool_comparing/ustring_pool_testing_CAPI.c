
#include "../../ht_testing_common.h"
#include <ubut/ubench.h>



/*************************************************************************************************************/
inline const struct dbj_us_pool_interface* stb_pool()
{
	struct   dbj_us_pool_interface* stb_pool_instance = NULL;
	if (stb_pool_instance == NULL) {
		stb_pool_instance =
			(struct dbj_us_pool_interface*)uspool_factory(US_STB_POOL);
	}
	return stb_pool_instance;
}


UBENCH(strpool, std_pool_CAPI)
{
	const struct dbj_us_pool_interface* pimp_ = stb_pool();

	UST_VERIFY(pimp_);

	// notice the cast to another interface implementation
	test_common((const struct dbj_evergrowing_ustring_pool*)pimp_);

	test_removal(pimp_);
}