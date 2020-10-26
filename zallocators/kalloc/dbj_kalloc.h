#ifndef DBJ_KMEM_INC
#define DBJ_KMEM_INC
/*
dbj kalloc front
*/



#include "kalloc.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef __clang__
	void on_exit_release_kmem_pointer();

	inline void* dbj_k_init_()
	{
          atexit(on_exit_release_kmem_pointer);
		  return km_init() ;
	}

	inline void* k_memory_()
	{
		static void* k_memory_single_ = dbj_k_init_();
		return k_memory_single_;
	}

	inline void on_exit_release_kmem_pointer()
	{
		km_destroy(k_memory_());
	}

#define DBJ_KALLOC(T_,CNT_) (T_*)kcalloc( k_memory_(), CNT_, sizeof(T_))
#define DBJ_KREALLOC(P_, S_) krealloc( k_memory_() , P_, S_ )
#define DBJ_KFREE( P_ ) kfree( k_memory_(), (void *)P_  )

#else  // __clang__

    extern __thread void* k_memory_single_p ;

#define DBJ_KALLOC(T_,CNT_) (T_*)kcalloc( k_memory_single_p, CNT_, sizeof(T_))
#define DBJ_KREALLOC(P_, S_) krealloc( k_memory_single_p , P_, S_ )
#define DBJ_KFREE( P_ ) kfree( k_memory_single_p, (void *)P_  )


#endif // __clang__

#ifdef __cplusplus
} // "C"
#endif

/// ---------------------------------------------------------------------
/// this is "macros only" vector in C
/// #include "kalloc/kvec.h"

#endif // DBJ_KMEM_INC
