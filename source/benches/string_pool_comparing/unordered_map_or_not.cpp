#include "string_pool_common.h"
#include "string_pool_common.hpp"

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <dbj/dbj_string_pointers.h>  // dbj::uniq_string_pointers 

// ad-hoc usage of std::unordered_map

UBENCH(strpool, unordered_map)
{
	using std_hash_table =
		std::unordered_map<  std::size_t, dbj::string_ptr >;

	std_hash_table strings_ht_;

	driver(
		// appender
		[&](void) -> size_t {

			static size_t hash_ = 0U; //  std::hash< decltype (strng_) >{}(strng_);

			DBJ_REPEAT(storage_size_) {

				struct string_ string_ = pool_next_string(dbj_repeat_counter_);

				strings_ht_.emplace(
					hash_ = std::hash< const char* >{}(string_.data),
					dbj::string_ptr_make(string_.data)
				);
			}
			return hash_;
		},
		// remover
			[&](size_t hash_) {
			// remove_if() can not be used on associative containers
			// ditto
			for (auto it = strings_ht_.begin(); it != strings_ht_.end(); ++it) {
				if (it->first == hash_) {
					strings_ht_.erase(it);
					break;
				}
			}
		}
		);
}

UBENCH(strpool, ustring_uptr)
{
	dbj::ustrings  usstore_;

	driver(
		// appender
		[&](void) noexcept -> size_t {

			DBJ_REPEAT(storage_size_)
			{
				struct string_ string_ = pool_next_string(dbj_repeat_counter_);
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

UBENCH(strpool, uthash_cpp_interface)
{
	namespace HT = dbj::string_hash_table;
	typename  HT::type* sht_ = NULL;

	driver(
		// appender
		[&](void) noexcept -> int {

			int last_added_{};
			DBJ_REPEAT(storage_size_)
			{
				sht_ = HT::assign(sht_, pool_next_string(dbj_repeat_counter_).data);
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