#pragma once

#include <dbj/dbj_common.h>

#include "../../../ubut/ubench.h"
#include "../../../dbj-fwk/wallofmacros.h"

#ifdef __cplusplus
namespace
{
    constexpr auto storage_size_ = 0xFF;
    /*
* driver requires string storage to be visible in the scope
* both appender and remover do require
*/
    inline auto driver = [](auto appender_, auto remover_) {
        for (auto j = 0U; j < 1; ++j)
        {
            remover_(
                // add storage_size_ count of unique strings
                // and then remove the last one added
                appender_());
        }
    };

    // called from inside appender
    // here we can change where is the string coming from
    inline struct string_ pool_next_string (
        // optional if called from inside some loop 
        size_t some_counter_ = 0 ) noexcept
    {
        return  counter_to_string(some_counter_);
    }

} // namespace
#endif // __cplusplus