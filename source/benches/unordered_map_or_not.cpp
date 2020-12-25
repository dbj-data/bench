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

constexpr auto inner_loop_size = 1;
/*
* driver requires string storage to be visible in the scope
* bot appender and remover do require
*/
inline auto driver = []( auto appender_ , auto remover_) 
{
        for (auto j = 0U; j < inner_loop_size ; ++j)
        {
            remover_(
                appender_(SPECIMEN)
            );
        }
};

using std_hash_table = 
   std::unordered_map<  std::size_t , dbj::string_ptr >;

static std_hash_table strings_ht_;

UBENCH(hashtable_implementation, unordered_map)
{
    driver(
        // appender
        []( auto strng_) -> size_t {  

            // auto hash_ = std::hash< std::string_view >{}(std::string_view{ strng_ });
            size_t hash_ = std::hash< decltype (strng_) >{}( strng_ );
            strings_ht_.emplace(
                 hash_ ,
                dbj::string_ptr_make(strng_)
            );
            return hash_;
        },
        // remover
        [](size_t hash_) {
            for (auto it = strings_ht_.begin(); it != strings_ht_.end(); ++it ) {
                if ( it->first == hash_ ) {
                    strings_ht_.erase(it);
                    break;
                }
            }
        }
    );
}


static dbj::string_pointers  usstore_;

UBENCH(hashtable_implementation, dbj_string_uniq_ptr )
{
    driver(
        [](auto strng_) -> dbj::string_ptr & { return usstore_.assign(strng_) ; },
        [](dbj::string_ptr & sp_) {

            // calling remove slows things down two times
            // bool rez_ = remove( unique_strings_storage_, sp_);
            // assert( rez_ );

            auto index_ = 0U;
            for (auto && it_  :  usstore_.strings )
            {
                if ( sp_ ==  it_ )
                {
                    usstore_.strings.erase(usstore_.strings.begin() + index_);
                }

                index_ += 1;
            }
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