#include "../../ht_testing_common.h"
#include "slot_array_map_ustrings.h"
#include "ustring_pool_using_c_apis.h"
#include "uthash_ustrings.h"
#include <dbj/dbj_ustrings.h>
#include <ubut/ubench.h>

/****************************************************************************************/
// (c) 2021 by dbj@dbj.org
struct dbj_uss final {
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
}; // dbj_uss

/****************************************************************************************/
#include "../../loki_assoc_vector.h"

struct loki_assoc_vector_pool final
{
	using pool_type = Loki::AssocVector<size_t, std::string>;
	using handle = pool_type::key_type;

	pool_type pool_{};

	handle add(const char* s)
	{
		static std::hash<std::string_view> hash_fun;
		//
		size_t hash = hash_fun(s);
		auto it = pool_.find(hash);
		// already in here
		if (it != pool_.end())  return hash;
		// new unique entry
		pool_[hash] = pool_type::mapped_type(s);
		return hash;
	}
	const char* cstring(handle h) {
		auto it = pool_.find(h);
		if (it == pool_.end())  return nullptr;
		return (*it).second.c_str();
	}

	bool remove(handle h) noexcept {
		return pool_.erase(h); // returns 0 or 1
	}

	size_t count() const {
		return pool_.size();
	}
};
/****************************************************************************************/
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
/*  (c) 2021 by dbj@dbj.org -- unique string with unordered_set
	https://godbolt.org/z/Kjjfcq

#include <unordered_set>

struct S final {
	size_t hash ;
	std::string name;
};
inline bool operator==(const S& lhs, const S& rhs) noexcept {
	return lhs.hash == rhs.hash;
}

// custom hash
struct MyHash
{
	std::size_t operator()(S const & s) const noexcept
	{
		return const_cast<S&>(s).hash = std::hash<std::string>{}(s.name);
		// return s.hash ;
	}
};

using set_of_unique_names =  std::unordered_set<S, MyHash> ;

size_t add (set_of_unique_names & names, const char * name_) {
	auto R = names.emplace( S{0, std::string(name_) });
	return R.first->hash;
}

bool find
(set_of_unique_names const & names, std::string & retval, size_t hash_) {
for(auto&& s: names)
if (s.hash == hash_) {
	retval = s.name ;
	return true;
}
return false ;
}

int main()
{
	set_of_unique_names names;
	auto h1 = add( names, "Rodriguez" );
	auto h2 = add( names, "Mickey" );
	auto h3 = add( names, "Leela" );
	auto h4 = add( names, "Mouse" );
	auto h5 = add( names, "Leela" );

	std::string rezult;
	 if ( find(names, rezult, h1 ) )
	std::cout << "Found: " << rezult << ", by id: " << h1 << "\n";


std::cout << "\nSet size:" << names.size() << "\n";
	for(auto&& s: names)  std::cout << s << '\n';
}
*/

/****************************************************************************************/
// (c) 2021 by Arthur O'Dwyer

// not-a-handle type for 
// Arthur's solutions
// it is std::string_view
// not a handle actually but a "pointer"

struct strings_set_pool final {
	using handle = std::string_view;
	std::set<std::string, std::less<> > set_;
	handle add(const char* s) {
		// just ignore the inserted
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
	test_removal<dbj_uss>();
}

UBENCH(strpool, loki_assoc_vector_pool) {
	test_removal<loki_assoc_vector_pool>();
}

