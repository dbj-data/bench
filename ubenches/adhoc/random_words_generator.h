#pragma once
#include <array>
#include <algorithm>
#include <string_view>
#include <ctime>
#include <cmath>
#include <random>

#ifdef _DEBUG
#include <typeinfo>       // operator typeid
#endif

namespace dbj_adhoc
{
	/*
	produce random words but just a bit more normal looking
	or not, or just numbers, or just vowels ... whatever you like
	*/
	using namespace std;
	using namespace std::string_view_literals;

	int random_in_range( int min_, int max_ )
	{
		random_device dev;
		mt19937 rng(dev());
		uniform_int_distribution<std::mt19937::result_type> 
			dist_(min_, max_); // distribution in range [min, max]
		return dist_(rng);
	}

	template <typename CT> struct meta_data final { };

	template < > struct meta_data<char> final
	{
		constexpr static auto digits{ "0123456789"sv };
		constexpr static auto alpha{ "bcdfghjklmnpqrstvwxyz"sv }; // no vowels
		constexpr static auto vowels{ "aeiou"sv };
	};

	template < > struct meta_data <wchar_t> final
	{
		constexpr static auto digits{ L"0123456789"sv };
		constexpr static auto alpha{ L"bcdfghjklmnpqrstvwxyz"sv }; // no vowels
		constexpr static auto vowels{ L"aeiou"sv };
	};

	// needs to be transformable to/from char
	struct zulu final { 
		using char_type = char; 
		char_type char_ ; 
		zulu() : char_('?') {}
		zulu(char_type new_char_) : char_(new_char_) {}
		operator const char () const { return char_; }
		operator char () { return char_; }
	};

	template < > struct meta_data<zulu> final
	{
		constexpr static auto digits{ "0123456789"sv };
		constexpr static auto alpha{ "bcdfghjklmnpqrstvw"sv }; // no vowels
		constexpr static auto vowels{ "aeiou"sv };
	};


	template<typename SEQ>
	constexpr size_t top_index(SEQ const& sv_) { return sv_.size() - 1U; }

	constexpr inline size_t size_one = size_t(1U);

	template <typename CT>
	inline  CT random_digit () noexcept
	{
		static auto & digits_ = meta_data<CT>::digits;
		static size_t top_idx_ = top_index(digits_);
		return digits_[random_in_range( 0, top_idx_ )];
	}

	template <typename CT>
	 inline  CT random_alpha () noexcept
	{
#ifdef _DEBUG
		 const char* typename_ = typeid(CT).name();
#endif
		static auto& alpha_ = meta_data<CT>::alpha;
		static size_t top_idx_ = top_index(alpha_);
		return alpha_[random_in_range(0, top_idx_)];
	}

	template <typename CT>
	 inline  CT random_vowel () noexcept
	{
		static auto& vowels_ = meta_data<CT>::vowels;
		static size_t top_idx_ = top_index(vowels_);

#ifdef _DEBUG
		_ASSERTE(top_idx_ ==  (vowels_.size() -1));
#endif // _DEBUG
		return vowels_[random_in_range(0, top_idx_)];
	};

	enum class kind
	{
		digits,
		alpha,
		vowels,
		alpha_vowels,
		alpha_vowels_digits,
		zulu, /* has special meta data */
	};

	/*
		make random words of the "kind" enum above allows
		default is leters and vowels
		to make things simple and fast
		use std::array as return type
		*/
	template <typename T, size_t L>
	inline array<T, L> &
		random_word(array<T, L>& carr, kind which_ = kind::alpha_vowels)
	{
		//static_assert (
		//	std::is_same_v<T, char> || std::is_same_v<T, wchar_t>
		//	, "only non reference and non const, char or wchar_t allowed"
		//);

		std::size_t N = L - 1;

		switch (which_)
		{
		case kind::digits:
			for (auto k = 0U; k < N; k++)
				carr[k] = random_digit<T>();
			break;
		case kind::alpha:
			for (auto k = 0U; k < N; k++)
				carr[k] = random_alpha<T>();
			break;
		case kind::vowels:
			for (auto k = 0U; k < N; k++)
				carr[k] = random_vowel<T>();
			break;
		case kind::alpha_vowels_digits:
			for (auto k = 0U; k < N; k++)
			{
				carr[k] = random_alpha<T>();
				k++;
				if (k > N)
					break;
				carr[k] = random_vowel<T>();
				k++;
				if (k > N)
					break;
				carr[k] = random_digit<T>();
			}
			break;
		default: // kind::alpha_vowels:
			for (auto k = 0U; k < N; k++)
			{
				carr[k] = random_vowel <T>();
				k++;
				if (k > N)	break;
				carr[k] = random_alpha <T>();
			}
		break;
		};
		carr[N] = T('\0');
		return carr;
	}

	extern "C" inline void test_random_words_generator(void)
	{
		static int do_it_n_times = 0xF;
		static int done_it_n_times = 0;

		constexpr auto word_length = 8 ;
		std::array<zulu, word_length>  word;

			const auto rword = random_word(word ).data();
			// using dbj--simplelog
			if (done_it_n_times++ < do_it_n_times)
			UBUT_INFO(" %3d: Random Word: '%s'", done_it_n_times, rword);
	}

} // namespace dbj_adhoc
