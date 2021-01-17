#pragma once
/*
* (c) 2021 by dbj@dbj.org  -- https://dbj.org/dbj_license
*
* USPOOL == Unique String POOL
*/
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
// (c) 2021 by dbj@dbj.org https://dbj.org/license_dbj

// CAPI == C API
#ifdef  __cplusplus
extern "C" {
#endif //  __cplusplus

	// Strictly using clang-cl.exe and C11/C17
#ifndef __clang__
#error __FILE__ is clang only 
#endif // ! __clang__

#pragma clang system_header

// the architecture
// handle is a number
// handle is not a pointer
// dangling handle can be deduced
// dangling pointer can not
// handle does not have to be freed
// pointer must be freed
	typedef size_t dbj_handle;

	// we use handle based API's to collections of things
	// one handle is used to manage one thing from one collection
	// simple example:
	// if collection is an array
	// handle is index on that array

	// in here we are interested in pools of unique things
	// pools are divided in two categories:
	// with or without removal ability

	// if pool insertions discards duplicates
	// that pools size will not grow
	// not as much as pool of duplicates
	// also
	// if pool can not be removed from
	// there are no invalid handles
	// (unless purposefully made and sumbited)
	// that also makes posible to use `views` as handles
	// e.g std::string_view, or even just const char * ptr_ 
	// this makes for very simple and fast pools

	struct dbj_us_pool_data;

	struct dbj_evergrowing_ustring_pool
	{
		// if single shared instance 
		void (*reset)(void);
		// does not insert if already exist
		// return handle to the entry made 
		// or found
		dbj_handle(*add)(const char*);

		// returns nullptr if not found
		const char* (*cstring)(dbj_handle);

		size_t(*count)();

	}; // dbj_evergrowing_ustring_pool


	 // this is still the pool of unique entries
	// but with the ability to remove them
	// thus the problem arises: the dangling handles
	struct dbj_us_pool_interface
	{
		// if single shared instance 
		void (*reset)(void);
		// we couild expose parent methods
		// nut we wont, one might just use c++
		// dbj_evergrowing_ustring_pool* (*parent)(void);
		// ditto
			// does not insert if already exist
		// return handle to the entry made 
		// or found
		dbj_handle(*add)(const char*);

		// returns nullptr if not found
		const char* (*cstring)(dbj_handle);

		size_t(*count)();

		// added method
		// ability of removal implies handle can be "dangling"
		// "dangling" is a handle to the removed item
		// 
		// returns true if found, 
		// after this the handle becomes invalid aka "dangling handle"
		// funny thing is: if we maintain internally unique
		// evergrowing pool of dangling
		// handles we can use that to a great effect
		bool (*remove)(dbj_handle);

	}; // dbj_us_pool_interface

	// TODO: locking policy. NULL policy does no locking.

	// master factory 
	typedef enum uspool_implementations_registry {
		US_STB_POOL, US_GUSTAVSON_POOL, US_UTHASH_POOL
	} uspool_implementations_registry;

	void* uspool_factory(uspool_implementations_registry);

#ifdef  __cplusplus
} // extern "C"
#endif //  __cplusplus

