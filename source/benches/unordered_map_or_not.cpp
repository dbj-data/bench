#include "../../ubut/ubench.h"
#include "../../dbj-fwk/wallofmacros.h"

#define  SPECIMEN "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
                  "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" 

#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <dbj/dbj_string_pointers.h>  // dbj::uniq_string_pointers 

constexpr auto storage_size_ = 0xFFFF ;
/*
* driver requires string storage to be visible in the scope
* bot appender and remover do require
*/
inline auto driver = [](auto appender_, auto remover_)
{
	for (auto j = 0U; j < 1 ; ++j)
	{
		remover_(
			appender_(SPECIMEN)
		);
	}
};

using std_hash_table =
std::unordered_map<  std::size_t, dbj::string_ptr >;

static std_hash_table strings_ht_;

UBENCH(hashtable_implementation, unordered_map)
{
	driver(
		// appender
		[](auto strng_) -> size_t {

			static size_t hash_ = 0U; //  std::hash< decltype (strng_) >{}(strng_);

			DBJ_REPEAT(storage_size_) {
				strings_ht_.emplace(
					hash_ = std::hash< decltype (strng_) >{}(strng_),
					dbj::string_ptr_make(strng_)
				);
			}
			return hash_;
		},
		// remover
			[](size_t hash_) {

			for (auto it = strings_ht_.begin(); it != strings_ht_.end(); ++it) {
				if (it->first == hash_) {
					strings_ht_.erase(it);
					break;
				}
			}
		}
		);
}

static dbj::string_pointers  usstore_;

UBENCH(hashtable_implementation, dbj_string_uniq_ptr)
{
	driver(
		[](auto strng_) -> dbj::string_ptr& {

			typename dbj::string_pointers::value_type::value_type  & str_ptr_ 
				= dbj::string_ptr{} ;

			DBJ_REPEAT(storage_size_) {
				str_ptr_ =  std::move( assign(usstore_, strng_) ) ;
			}

			return str_ptr_;
		},
		[](dbj::string_ptr& sp_) {

#if 0
			// calling remove slows things down
			bool rez_ = remove(usstore_, sp_);
			assert(rez_);
#endif
			auto index_ = 0U;
			const auto begin_ = usstore_.strings.begin();
			for (auto&& it_ : usstore_.strings)
			{
				if (sp_ == it_)
				{
					usstore_.strings.erase(begin_ + index_);
					break;
				}
				index_ += 1;
			}
#if 0
			// this is faster than  bool rez_ = remove(usstore_, sp_);
			// but not as fast as above 
			DBJ_REMOVE_FIRST_IF(usstore_.strings, [&](dbj::string_ptr const& it_) { return it_ == sp_;  });
#endif 
		}
	);
	}

/*
 [----------]Running 2 benchmarks.
 [ RUN      ]hashtable_implementation.unordered_map
 [      OK  ]hashtable_implementation.unordered_map (mean 92.822us, confidence interval +- 0.887484%)
 [ RUN      ]hashtable_implementation.dbj_string_uniq_ptr
 [      OK  ]hashtable_implementation.dbj_string_uniq_ptr (mean 63.020us, confidence interval +- 0.673364%)
 [----------]2 benchmarks ran.
 [  PASSED  ]2 benchmarks.
*/