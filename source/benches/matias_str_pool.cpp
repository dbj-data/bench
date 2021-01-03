#define _CRT_SECURE_NO_WARNINGS 1

#include "../../ubut/ubench.h"
#include "../../dbj-fwk/wallofmacros.h"

#define STRPOOL_IMPLEMENTATION
#include <mg/strpool.h>
#include <dbj/dbj_common.h>

extern "C" {

	struct string_ {
		size_t size;
		char data[0xFF];
	};

	static struct string_  counter_to_string(size_t size_arg_)
	{
		struct string_ retval = { 0, {0} };

		// warning: 0 produces "" (that has lenght 1), aka empty string
		size_arg_ += 1;

		// retval.data = (char*)calloc(retval.size, sizeof(char));
		(void)snprintf(retval.data, 0xFF, "%zu", size_arg_);
		return retval;
	}
}


constexpr auto storage_size_ = 0xFF;
/*
* driver requires string storage to be visible in the scope
* bot appender and remover do require
*/
inline auto driver = [](auto appender_, auto remover_)
{
	for (auto j = 0U; j < 1; ++j)
	{
		remover_(
			// add storage_size_ count of unique strings
			// and then remove the last one added
			appender_()
		);
	}
};

UBENCH(hashtable, gustavson_string_pool)
{
	strpool_config_t conf = strpool_default_config;
	//conf.ignore_case = true;

	strpool_t pool;
	strpool_init(&pool, &conf);

	STRPOOL_U64 str_handle = 0U;

	driver(
		// appender
		[&](void) noexcept -> size_t {

			DBJ_REPEAT(storage_size_)
			{
				struct string_ string_ = counter_to_string(dbj_repeat_counter_);
				str_handle =
					strpool_inject(&pool, 
						string_.data, 
						(int)strlen(string_.data)
				);
			}
			// return the last one hash id
			return str_handle;
		},

		// remover
			[&](STRPOOL_U64 str_handle) noexcept {

			strpool_discard(&pool, str_handle);
		}
		);
				strpool_term(&pool);
}