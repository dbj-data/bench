# Comparing String Pools
>
> &copy 2020 by dbj@dbj.org -- https://dbj.org/license_dbj
> 

Relevant code is [here](https://github.com/dbj-data/dbj-bench/tree/master/source/benches/string_pool_comparing).

Build: 
- cl.exe in release mode. 
- `/D_HAS_EXCEPTIONS=0` 
  - also puts MS STL in the "[SEH](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=msvc-160) mode"
-  `/GR-` for no RTTI. 
-  Linking with runtime static lib `/MT`

The results using my [bench](https://github.com/dbj-data/dbj-bench) , basically my variant of [sheredom ubench](https://github.com/sheredom/ubench.h).
```
12:30:06 INFO [----------]Running 6 benchmarks.
12:30:06 INFO [ RUN      ]strpool.uthash_string_pool
12:30:06 INFO [      OK  ]strpool.uthash_string_pool (mean 1.376us, confidence interval +- 1.402546%)
12:30:06 INFO [ RUN      ]strpool.dbj_unique_strings
12:30:06 INFO [      OK  ]strpool.dbj_unique_strings (mean 0.861us, confidence interval +- 3.797177%)
12:30:06 INFO [ RUN      ]strpool.uthash_using_unique_ptr
12:30:06 INFO [      OK  ]strpool.uthash_using_unique_ptr (mean 1.255us, confidence interval +- 7.770992%)
12:30:06 INFO [ RUN      ]strpool.uthash_using_vector
12:30:07 INFO [      OK  ]strpool.uthash_using_vector (mean 1.326us, confidence interval +- 0.571929%)
12:30:07 INFO [ RUN      ]strpool.gustavson_string_pool
12:30:07 INFO [      OK  ]strpool.gustavson_string_pool (mean 162.167us, confidence interval +- 3.682658%)
12:30:07 INFO [ RUN      ]strpool.set_of_strings
12:30:07 INFO [      OK  ]strpool.set_of_strings (mean 0.977us, confidence interval +- 2.246290%)
12:30:07 INFO [----------]6 benchmarks ran.
12:30:07 INFO [  PASSED  ]6 benchmarks.
```

Benchmarks are [here](all_in_one.cpp). Behind a common interface and passed through a common test.

After lengthy attempts I could not find faster solution than `dbj::ustrings` (`ustring_uptr` above).  `uthash` comes close second. But. 
I have decided to develop in very simple C++, interlaced with a lot o C11/17 constructs. 

C++ is is use mainly because of RAII abilities. And. Because MS STL has already developed all, I need **without** forcing me to use C++ exceptions, iostreams, `<system_error>`, RTTI and the rest. That is possible because MS STL builds and works wihtout  C++ exceptions present and with keywords try/throw/catch removed from the language. 

[SEH](https://docs.microsoft.com/en-us/cpp/cpp/structured-exception-handling-c-cpp?view=msvc-160) is used in that mode. SEH + VisualStudio + "minidumps", are giving me very poerfull debugging tools for C++ Windows development. 

The `dbj::ustrings` is the proof (I hope) of the validity of my architectural decision. Here is the synopsis.

First here is DBJ system wide string pointer as the fundamental building block type.
```cpp
using string_ptr = std::unique_ptr<char[]>;

// null or empty string
inline bool is_empty(const string_ptr& text) noexcept;

constexpr inline bool operator==(const string_ptr& lhs, const  string_ptr& rhs) noexcept;

struct is_equal final {
	constexpr bool operator()(const string_ptr& lhs, const  string_ptr& rhs) const noexcept;
};

// Equivalent to strdup but produces dbj::string_ptr
string_ptr string_ptr_make(const char* text) noexcept;
```
Custom hash specialization is injected in `std::`
```cpp
namespace std
{
	template<> struct hash<dbj::string_ptr> final
	{
		std::size_t operator()(dbj::string_ptr const& sp_) const noexcept;
	};
}
```
And lastly, the storage of unique strings.
```cpp
struct ustrings;
struct ustrings final
{
	struct node final
	{
		size_t H{}; // hash
		string_ptr ptr{}; // uniq pointer to char[]

		node() = default;

		// can not be copied.
		node(const node&) = delete;
		node& operator=(const node&) = delete;

		// can be moved.
		node(node&&) noexcept = default;
		node& operator=(node&&) noexcept = default;
	};

	using type = ustrings;
	using value_type = typename std::vector<node>;
	using value_type_it = typename value_type::iterator;

    // available to be used by future required extensions
    // that is opposite of adding methods to ustrings
	value_type strings;

	// hash will be calculated if not done before
    // std::hash<dbj::string_ptr> is used in here
	static node make_node(const char* cp_, size_t hash_done_before_ = 0U);

	ustrings() noexcept ;

	// ustrings objects can not be copied.
	ustrings(const ustrings&) = delete;
	ustrings& operator=(const ustrings&) = delete;

	// ustrings objects can be moved.
	ustrings(ustrings&&) noexcept = default;
	ustrings& operator=(ustrings&&) noexcept = default;

	~ustrings() noexcept;

	void clear() noexcept;

	// assign new node if not found
	// return hash
	friend size_t assign(type& usstore_, const char* text) noexcept;

	// remove if found and return true
	// otherwise return false
	friend bool remove(type& usstore_, size_t needle_) noexcept;
}; // ustrings

```
I have found (for above), vector is faster than list. list has `remove_if()` but still above, the current `remove()` [implementation](https://github.com/dbj-data/dbj/blob/bb64275db0b778f16a70f449693da6f14117ec07/dbj_string_pointers.h) is plenty fast.

I do sincerely hope I am using uthash and friends as they should be.

I have added UBENCH to [Artur's good solutions](https://godbolt.org/z/bPh6ss)



