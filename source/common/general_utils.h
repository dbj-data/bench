#pragma once
#ifndef STRING_POOL_COMMON_INC
#define STRING_POOL_COMMON_INC

#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>
#include <random>

#undef DBJ_REPEAT
#define DBJ_REPEAT(N) for (size_t dbj_repeat_counter_ = 0; dbj_repeat_counter_ < (size_t)(N); ++dbj_repeat_counter_)

#ifdef __cplusplus
extern "C" {
#endif

	/*
	general utilities
	move possibly into dbj top level headers
	*/

#pragma region 	Paul Hsieh Fast Hash

	// Jenkins is here: http://www.burtleburtle.net/bob/hash/spooky.html

	// https://gist.github.com/CedricGuillemet/4978020

#undef get16bits
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
		+(uint32_t)(((const uint8_t *)(d))[0]) )
		//
		// super hash function by Paul Hsieh
		//
	inline uint32_t paul_hsieh_hash(const char* data, size_t len) {
		uint32_t hash = (uint32_t)len, tmp;
		int rem;

		if (len <= 0 || data == NULL) return 0;

		rem = (int)(len & 3);
		len >>= 2;

		/* Main loop */
		for (; len > 0; len--) {
			hash += get16bits(data);
			tmp = (get16bits(data + 2) << 11) ^ hash;
			hash = (hash << 16) ^ tmp;
			data += 2 * sizeof(uint16_t);
			hash += hash >> 11;
		}

		/* Handle end cases */
		switch (rem) {
		case 3: hash += get16bits(data);
			hash ^= hash << 16;
			hash ^= data[sizeof(uint16_t)] << 18;
			hash += hash >> 11;
			break;
		case 2: hash += get16bits(data);
			hash ^= hash << 11;
			hash += hash >> 17;
			break;
		case 1: hash += *data;
			hash ^= hash << 10;
			hash += hash >> 1;
		}

		/* Force "avalanching" of final 127 bits */
		hash ^= hash << 3;
		hash += hash >> 5;
		hash ^= hash << 4;
		hash += hash >> 17;
		hash ^= hash << 25;
		hash += hash >> 6;

		return hash;
	} // paul_hsieh_hash

#undef get16bits
#pragma endregion 


/* https://stackoverflow.com/a/17554531
*
* dbj : RAND_MAX is SHRT_MAX is 32767
* thus be carefull  max <  RAND_MAX
*/
	static unsigned int rand_interval(const unsigned short min, const unsigned short max)
	{
		constexpr unsigned short RANDYMAXY{ RAND_MAX };
		assert(RANDYMAXY > 0);
		assert(RANDYMAXY > max);

		assert(max > min);
		if (min == max) return min;

		unsigned int r;
		const unsigned int range = 1 + max - min;
		const unsigned int buckets = RANDYMAXY / range;
		assert(buckets > 0);
		const unsigned int limit = buckets * range;
		assert(limit > 0);

		do {
			r = std::rand();
		} while (r >= limit);

		return min + (r / buckets);
	}

	/*
	common things made for comparing various string pooling solutions
	*/
	enum { dbj_string_data_size_ = 0xFF };
	struct dbj_string_
	{
		size_t size; // the strlen result
		char data[dbj_string_data_size_];
	};

#if 0
	static struct dbj_string_ counter_to_string(size_t counter_)
	{
		struct dbj_string_ retval = { 0, {0} };
		// warning: counter_ == 0 produces "" aka empty string
		// thus we do +1
		(void)snprintf(retval.data, dbj_string_data_size_, "%zu", (counter_ + 1));
		retval.size = strlen(retval.data);
		return retval;
	}

	inline char make_random_char() {
		return (char)rand_interval(64, 126);
	}

	static struct dbj_string_  make_random_string(size_t size_arg_)
	{
		struct dbj_string_ retval = { 0, {0} };
		assert(size_arg_ > 0);
		srand((unsigned)time(0));
		retval.size = size_arg_ + 1;

		for (size_t k = 0U; k < size_arg_; ++k)
			retval.data[k] = make_random_char();

		retval.data[size_arg_] = '\0';
		return retval;
	}
#endif // 0
#ifdef __cplusplus
} //	extern "C" 
#endif

#ifdef __cplusplus

#include <cstdint>
#include <limits>

//

namespace {

	static constexpr std::uint64_t time_to_seed()
	{
		std::uint64_t shifted = 0;

		for (const auto c : __TIME__)
		{
			shifted <<= 8;
			shifted |= c;
		}

		return shifted;
	}

	// can't find the author?
	template<std::uint32_t S, std::uint32_t A = 16807UL, std::uint32_t C = 0UL, std::uint32_t M = (1UL << 31) - 1>
	struct LinearGenerator final {
		constexpr static const std::uint32_t state = ((std::uint64_t)S * A + C) % M;
		constexpr static const std::uint32_t value = state;

#if LINEAR_GENERATOR_LEAPFROG
		typedef LinearGenerator<state> next;
		struct Split { // Leapfrog
			typedef LinearGenerator< state, A* A, 0, M> Gen1;
			typedef LinearGenerator<next::state, A* A, 0, M> Gen2;
		};
#endif // LINEAR_GENERATOR_LEAPFROG

	};

	using DBJ_CT_RND = LinearGenerator<std::uint32_t(time_to_seed())>;

} // nspace as
#endif // __cplusplus

/* usage:

#include <stdio.h>
int main()
{
  constexpr auto r2 = DBJ_CT_RND::value;
}
*/



#endif  // STRING_POOL_COMMON_INC
