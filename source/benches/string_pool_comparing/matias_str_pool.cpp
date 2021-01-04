
#include "string_pool_common.h"
#include "string_pool_common.hpp"

#define STRPOOL_IMPLEMENTATION
#include <mg/strpool.h>


UBENCH(strpool, gustavson_string_pool)
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
				struct string_ string_ = pool_next_string(dbj_repeat_counter_);
				str_handle =
					strpool_inject(&pool, 
						string_.data, 
						(int)string_.size
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