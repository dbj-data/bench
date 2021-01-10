#include "../../ht_testing_common.h"
#include "slot_array_map_ustrings.h"
#include "ustring_pool_using_c_apis.h"
#include "uthash_ustrings.h"
#include <dbj/dbj_ustrings.h>
#include "../ubut/ubench.h"



/****************************************************************************************/
// (c) 2021 by dbj@dbj.org
struct dbj_ustrings final {
	using handle = size_t;
	using store_t = dbj::ustrings;
	store_t store_{};

	handle add(const char* s) noexcept {
		return store_t::assign(store_, s);
	}

	//if handle is not valid null pointer is returned
	const char* cstring(handle h) const noexcept
	{
		for (auto&& node_ : store_.strings)
		{
			if (h == node_.H)
			{
				return node_.ptr.get();
			}
		}
		return nullptr;
	}

	bool remove(handle h) noexcept {
		return store_t::remove(store_, h);
	}
	size_t count() const noexcept {
		return store_.strings.size();
	}
}; // dbj_ustrings

// evergrowing ustring pool based on 
// vector of uniq_ptr<char[]>
// handle is the index of the vector
// (c) 2021 by dbj.org
struct dbj_extreme final {
	using handle = size_t;
	using store_t = std::vector<dbj::string_ptr>;
	store_t store_{};

	handle add(const char* str_) noexcept {

		handle idx = 0;
		for (auto&& el_ : store_)
		{
			if (!strcmp(el_.get(), str_))
				return idx;
			idx++;
		}

		// else element not found
		store_.push_back(dbj::string_ptr_make(str_));
		return store_.size() - 1;
	}

	//handle is the vector index
	// there is no removing thus indexes are unchanged
	const char* cstring(handle h_) const noexcept
	{
		return store_[h_].get();
	}

	size_t count() const noexcept {
		return store_.size();
	}
}; // dbj_extreme

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer

// different handle type for 
// Arthur's solutions
// it is std::string_view
// not a handle actually but a view


struct strings_set_pool final {
	using handle = std::string_view;
	std::set<std::string, std::less<> > set_;
	handle add(const char* s) {
		// inserted, just ignore it
		auto [it, inserted] = set_.emplace(s);
		return *it;
	}
	const char* cstring(handle h) {
		// handle should be always valid
		// as there is no removal
		return h.data();
	}
	size_t count() const {
		return set_.size();
	}
};


/****************************************************************************************/

UBENCH(strpool_evergrowing, set_of_strings) {
	test_evergrowing<strings_set_pool>();
}

UBENCH(strpool_evergrowing, dbj_extreme_solution) {
	test_evergrowing<dbj_extreme>();
}

UBENCH(strpool, dbj_unique_strings) {
	test_removal<dbj_ustrings>();
}




