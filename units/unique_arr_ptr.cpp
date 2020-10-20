#include "../ubench.h/ubench.h"
#include <memory>
#include <EASTL/unique_ptr.h>

// cheating, but EASTL is still almost as fast
namespace dbj {

template<typename T >
struct arr_ptr final 
{
     using value_type = eastl::remove_extent_t<T>;

    size_t size_{} ;
    value_type * data_{} ;

    value_type & operator [] (int idx_ ) {
        return data_[ idx_ % size_ ] ;
    }

    inline static const size_t value_type_size = sizeof(value_type) ;

    arr_ptr( ) noexcept : data_(nullptr)  {}

    arr_ptr( short size ) noexcept 
    : size_(size), data_( (value_type *)calloc(size_, value_type_size ) )  
    {}

   ~arr_ptr( ) noexcept {
       if ( this->data_ != nullptr ) 
           free(data_);
   }
    arr_ptr( arr_ptr const & ) = delete ;
    arr_ptr & operator = ( arr_ptr const & ) = delete ;

    arr_ptr( arr_ptr && other_) noexcept 
    : data_( other_.data_ )
    {
          other_.data_ = nullptr ;
    }
    
    arr_ptr & operator = ( arr_ptr && other_ ) noexcept 
    {
        if ( this != & other_ ) {
        free(this->data_) ;
        this->data_ = other_.data_ ;
        other_.data_ = nullptr ;
        }
    }
};

template <class T_, 
eastl::enable_if_t<eastl::is_array_v<T_> && eastl::extent_v<T_> == 0, int> = 0
>
[[nodiscard]] arr_ptr< T_ > make_arr_unique(size_t sze) {
  return arr_ptr<T_>(sze);
}

} // namespace dbj

extern "C" {
    
static struct { 
    size_t SZE; 
    size_t LOP; 
} common_data = {0xFF, 0xF };

}

auto loopy_schmoopy = [&](auto cback) {
  for (auto k = 0L; k < common_data.LOP; k++) {
    auto uap_ = cback();
  }
};

UBENCH(unique_ptr, dbj) {
  // dbj::arr_ptr<char> ap =  dbj::arr_ptr<char>( (char*)calloc(0xFF,
  // sizeof(char) ) ) ;
  loopy_schmoopy([] { return dbj::make_arr_unique<char[]>(common_data.SZE); });
}

UBENCH(unique_ptr, std_lib) {
  loopy_schmoopy([] { return std::make_unique<char[]>(common_data.SZE); });
}

UBENCH(unique_ptr, eastl) {

  loopy_schmoopy([] { return eastl::make_unique<char[]>(common_data.SZE); });
}
