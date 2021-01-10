#pragma once

// (c) 2021 by dbj@dbj.org https://dbj.org/license_dbj
// the architecture
// pools are divided in to categories
// woth or without removal ability

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
namespace dbj {
	template< typename engine_type >
	struct evergrowing_ustring_pool_interface
	{
		using type = evergrowing_ustring_pool_interface;
		using handle = typename engine_type::handle;

		// this implies engine default ctor
		// has to exist
		engine_type engine{};

		// must not insert if already exist
		// return ahndle to the entry made 
		// or just found
		// thus there is no concept of
		// invalid handle coming out of here
		handle add(const char* str_) noexcept {
			return engine.add(str_);
		}

		// returns nullptr if not found
		const char* cstring(handle h_) noexcept {
			return engine.cstring(h_);
		}

		size_t count() noexcept {
			return engine.count();
		}
	}; // ustring_pool_interface

	// this is still the pool of unique entries
	// but with the ability to remove them
	// thus the problem arises:
	// dangling handles
	template< typename engine_type >
	struct ustring_pool_interface final
		: public evergrowing_ustring_pool_interface<engine_type>
	{
		using parent = evergrowing_ustring_pool_interface<engine_type>;

		// ability of removal implies handle can be "dangling"
		// "dangling" is a handle to the removed item
		// 
		// returns true if found, 
		// after this the handle becomes invalid aka "dangling handle"
		// funny thing is: if we maintain internally unique
		// evergrowing pool of dangling
		// handles we can use that to a great effect
		bool remove(typename parent::handle h_) noexcept {
			return parent::engine.remove(h_);
		}
	}; // ustring_pool_interface

	// TODO: locking policy. NULL policy does no locking.

} // namespace dbj 
