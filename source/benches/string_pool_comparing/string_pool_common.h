#pragma once
#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

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
			inline uint32_t paul_hsieh_hash(const char* data, int len) {
				uint32_t hash = len, tmp;
				int rem;
		
				if (len <= 0 || data == NULL) return 0;
		
				rem = len & 3;
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


	/* https://stackoverflow.com/a/17554531 */
	static unsigned int rand_interval(unsigned int min, unsigned int max)
	{
		unsigned int r;
		const unsigned int range = 1 + max - min;
		const unsigned int buckets = RAND_MAX / range;
		const unsigned int limit = buckets * range;
		do	{	r = rand();	} while (r >= limit);
		return min + (r / buckets);
	}

    /*
    common things made for comparing various string pooling solutions
    */
    enum { common_string_data_size_ = 0xFF };
    struct string_
    {
        size_t size; // the strlen result
        char data[common_string_data_size_];
    };

    static struct string_ counter_to_string(size_t size_arg_)
    {
        struct string_ retval = {0, {0}};
        // warning: 0 produces "" (that has lenght 1), aka empty string
        // thus we start from 1
        size_arg_ += 1;
        (void)snprintf(retval.data, common_string_data_size_, "%zu", size_arg_);
        retval.size = strlen(retval.data);
        return retval;
    }

	inline char make_random_char() {
		return (char)rand_interval(64, 126);
	}

	static struct string_  make_random_string(size_t size_arg_)
	{
		struct string_ retval = { 0, {0} };
		assert(size_arg_ > 0);
		srand((unsigned)time(0));
		retval.size = size_arg_ + 1;

		for (size_t k = 0U; k < size_arg_; ++k)
			retval.data[k] = make_random_char();

		retval.data[size_arg_] = '\0';
		return retval;
	}

#ifdef __cplusplus
} //	extern "C" 
#endif

