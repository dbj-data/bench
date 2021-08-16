// -*- Mode: C++; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*-
// vim:tabstop=4:shiftwidth=4:expandtab:

/*
 * Copyright (C) 2004-2019 Wu Yongwei <wuyongwei at gmail dot com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any
 * damages arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute
 * it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must
 *    not claim that you wrote the original software.  If you use this
 *    software in a product, an acknowledgement in the product
 *    documentation would be appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must
 *    not be misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source
 *    distribution.
 *
 * This file is part of Stones of Nvwa:
 *      https://github.com/adah1972/nvwa
 *
 */

/**
 * @file  mem_pool_base.h
 *
 * Header file for the memory pool base.
 *
 * @date  2019-08-22
 */

// DBJ made it all inline
// DBJ NOTE: there is no object level locking in here, but there is in offsprings

#ifndef NVWA_MEM_POOL_BASE_H
#define NVWA_MEM_POOL_BASE_H

#include "dbj_win_alloc.h" 

#include "_nvwa.h"              // NVWA_NAMESPACE_*

NVWA_NAMESPACE_BEGIN

/* Defines macros to abstract system memory routines */
# ifdef _MEM_POOL_USE_MALLOC
#   define _MEM_POOL_ALLOCATE(_Sz)    malloc(_Sz)
#   define _MEM_POOL_DEALLOCATE(_Ptr) free(_Ptr)
# else
#   define _MEM_POOL_ALLOCATE(_Sz)    ::operator new((_Sz), std::nothrow)
#   define _MEM_POOL_DEALLOCATE(_Ptr) ::operator delete(_Ptr)
# endif

// DBJ added -- in case of Windows build just ignore whats being done and do it again the windows way
#undef _MEM_POOL_ALLOCATE
#undef _MEM_POOL_DEALLOCATE
#define _MEM_POOL_ALLOCATE(SZ_)    DBJ_WIN_ALLOCATE(SZ_)
#define _MEM_POOL_DEALLOCATE(PT_)  DBJ_WIN_DEALLOCATE(PT_)


/**
 * Base class for memory pools.
 */
struct mem_pool_base {

    virtual /* Empty base destructor. */
    ~mem_pool_base() noexcept
    {
    }
    virtual void recycle() = 0;
    /**
     * Allocates memory from the run-time system.
     *
     * @param size  size of the memory to allocate in bytes
     * @return      pointer to allocated memory block if successful; or
     *              null if memory allocation fails
     */
    static void* alloc_sys(size_t size)
    {
        return _MEM_POOL_ALLOCATE(size);
    }

    /**
     * Frees memory and returns it to the run-time system.
     *
     * @param ptr  pointer to the memory block previously allocated
     */
    static void dealloc_sys(void* ptr)
    {
        _MEM_POOL_DEALLOCATE(ptr);
#ifdef _DEBUG
        ptr = nullptr;
#endif // _DEBUG
    }

    /** Structure to store the next available memory block. */
    struct _Block_list {
        // DBJ added the initializer {}
        _Block_list* _M_next{};   ///< Pointer to the next memory block
    };
};

NVWA_NAMESPACE_END

#endif // NVWA_MEM_POOL_BASE_H
