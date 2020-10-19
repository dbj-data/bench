#include "../ubench.h/ubench.h"

struct common_data final {
     inline static long loop_size = 0xFFFFUL ;
     inline static const char * small_string = "Small";
     inline static const char * large_string = 
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large "
    "Large Large Large Large Large Large Large Large " ;
}; // common_data

static auto once = [&]() ->bool
{ 
    printf(
        "\n\nLoop size: %6lld\nSmall string length: %6zu\nLarge string length: %6zu\n\n", 
       common_data::loop_size ,
       strlen( common_data::small_string) ,
       strlen( common_data::large_string) 
       );
    return true ;
}();

#include <vector>
#include <string>

UBENCH( std_, std_vector_small_string ){
     std::vector<std::string> vs_ ;

     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.push_back( common_data::small_string );
     }
     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.pop_back();
     }
}

UBENCH( std_, std_vector_large_string ){
     std::vector<std::string> vs_ ;

     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.push_back( common_data::large_string );
     }
     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.pop_back();
     }
}

#include <eastl/vector.h>
#include <eastl/string.h>

UBENCH( dbjeastl, vector_small_string ){
     eastl::vector<eastl::string> vs_ ;

     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.push_back( common_data::small_string );
     }
     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.pop_back();
     }
}

UBENCH( dbjeastl, vector_large_string ){
     eastl::vector<eastl::string> vs_ ;

     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.push_back( common_data::large_string );
     }
     for (auto k = 0UL; k < common_data::loop_size; ++k )
     {
         vs_.pop_back();
     }
}
