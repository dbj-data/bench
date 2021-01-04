#pragma once
#undef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

    /*
    general utilities
    move possibly into dbj top level headers
    */
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

