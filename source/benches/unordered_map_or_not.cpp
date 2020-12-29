#include "../../ubut/ubench.h"
#include "../../dbj-fwk/wallofmacros.h"

#define _CRT_SECURE_NO_WARNINGS 1

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <dbj/dbj_string_pointers.h>  // dbj::uniq_string_pointers 

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern "C" {

	struct string_ {
		size_t size;
		char data[0xFF];
	};

#if 0
	/* https://stackoverflow.com/a/17554531 */
	static unsigned int rand_interval(unsigned int min, unsigned int max)
	{
		int r;
		const unsigned int range = 1 + max - min;
		const unsigned int buckets = RAND_MAX / range;
		const unsigned int limit = buckets * range;

		do
		{
			r = rand();
		} while (r >= limit);

		return min + (r / buckets);
	}

	inline char random_char() {
		return (char)rand_interval(64, 126);
	}

	static struct string_  make_random_string(size_t size_arg_)
	{
		struct string_ retval = { 0, {0} };
		assert(size_arg_ > 0);
		srand(time(0));
		retval.size = size_arg_ + 1;
		// retval.data = (char*)calloc((size_arg_ + 1), sizeof(char) );
		// assert(retval.data);

		for (size_t k = 0U; k < size_arg_; ++k)
			retval.data[k] = random_char();

		retval.data[size_arg_] = '\0';
		return retval;
	}
#endif // 0

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

// ad-hoc usage of std::unordered_map

UBENCH(hashtable_implementation, unordered_map)
{
	using std_hash_table =
		std::unordered_map<  std::size_t, dbj::string_ptr >;

	std_hash_table strings_ht_;

	driver(
		// appender
		[&](void) -> size_t {

			static size_t hash_ = 0U; //  std::hash< decltype (strng_) >{}(strng_);

			DBJ_REPEAT(storage_size_) {

				struct string_ string_ = counter_to_string(dbj_repeat_counter_);

				// debug time mem check
				assert('*' == (string_.data[string_.size / 2] = '*'));

				strings_ht_.emplace(
					hash_ = std::hash< const char* >{}(string_.data),
					dbj::string_ptr_make(string_.data)
				);
			}
			return hash_;
		},
		// remover
			[&](size_t hash_) {

			for (auto it = strings_ht_.begin(); it != strings_ht_.end(); ++it) {
				if (it->first == hash_) {
					strings_ht_.erase(it);
					break;
				}
			}
		}
		);
}

UBENCH(hashtable_implementation, dbj_string_uniq_ptr)
{
	dbj::string_pointers  usstore_;

	driver(
		// appender
		[&](void) noexcept -> size_t {

			DBJ_REPEAT(storage_size_)
			{
				struct string_ string_ = counter_to_string(dbj_repeat_counter_);
				// debug time mem check
				// assert( '*' == ( string_.data[string_.size / 2] = '*' )  );
				(void)assign(usstore_, string_.data);
			}
			// return the last one hash id
			return usstore_.strings.back().H;
		},

		// remover
			[&](size_t hash_uid_) noexcept {

			// calling remove slows things down
			remove(usstore_, hash_uid_);
		}
		);
}


#include "uthash_cpp_usage.h"

UBENCH(hashtable_implementation, uthash_cpp_interface)
{
	namespace HT = dbj::string_hash_table;
	typename  HT::type* sht_ = NULL;

	driver(
		// appender
		[&](void) noexcept -> int {

			int last_added_{};
			DBJ_REPEAT(storage_size_)
			{
				sht_ = HT::assign(sht_, counter_to_string(dbj_repeat_counter_).data);
				last_added_ = sht_->uid;
			}
			// return the last ones hash id
			return last_added_;
		},

		// remover
			[&](int last_) noexcept {
			sht_ = HT::remove(sht_, last_);
		}
		);

#ifdef _DEBUG
	auto item_count = HASH_CNT(hh, sht_);
	DBJ_UNUSED(item_count);
#endif

	sht_ =  HT::clear(sht_);
}