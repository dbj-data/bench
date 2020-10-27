/// https://godbolt.org/z/TE74oK
/// https://github.com/attractivechaos/benchmarks

#include "../../ubench/ubench.h"
// #include "dbj_kalloc.h"
// #include "kvec.h"
#include <vector>

/* The MIT License

   Copyright (c) 2008, by Attractive Chaos <attractor@live.co.uk>

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/*
  An example:

#include "kvec.h"
int main() {
	kvec_t(int) array;
	kv_init(array);
	kv_push(int, array, 10); // append
	kv_a(int, array, 20) = 5; // dynamic
	kv_A(array, 20) = 4; // static
	kv_destroy(array);
	return 0;
}
*/

/*
  2008-09-22 (0.1.0):

	* The initial version.

*/

#ifndef AC_KVEC_H
#define AC_KVEC_H

#include <stdlib.h>

// linux godbolt version shows no deviations
// https://godbolt.org/z/34jxWW
//  windows clang shows abnormal slowdown 
// inside push() and pushp()
// DBJ changed the resize algo from
// (v).m = (v).m? (v).m<<1 : 2 ;
// to
// (v).m += kv_step 
// speed increase was dramatic
// space spend is far from dramatic
#define kv_step 0xF

#define kv_roundup32(x) (--(x), (x)|=(x)>>1, (x)|=(x)>>2, (x)|=(x)>>4, (x)|=(x)>>8, (x)|=(x)>>16, ++(x))

#define kvec_t(type) struct { size_t n, m; type *a; }
#define kv_init(v) ((v).n = (v).m = 0, (v).a = 0)
#define kv_destroy(v) free((v).a)
#define kv_A(v, i) ((v).a[(i)])
#define kv_pop(v) ((v).a[--(v).n])
#define kv_size(v) ((v).n)
#define kv_max(v) ((v).m)

#define kv_resize(type, v, s)  ((v).m = (s), (v).a = (type*)realloc((v).a, sizeof(type) * (v).m))

#define kv_copy(type, v1, v0) do {							\
		if ((v1).m < (v0).n) kv_resize(type, v1, (v0).n);	\
		(v1).n = (v0).n;									\
		memcpy((v1).a, (v0).a, sizeof(type) * (v0).n);		\
	} while (0)												\

#define kv_push(type, v, x) do {									\
		if ((v).n == (v).m) {										\
			(v).m += kv_step  ;     	    \
			(v).a = (type*)realloc((v).a, sizeof(type) * (v).m);	\
		}															\
		(v).a[(v).n++] = (x);										\
	} while (0)

#define kv_pushp(type, v) (((v).n == (v).m)?							\
						   ((v).m += kv_step ,   		\
							(v).a = (type*)realloc((v).a, sizeof(type) * (v).m), 0)	\
						   : 0), ((v).a + ((v).n++))

#define kv_a(type, v, i) ((v).m <= (size_t)(i)?						\
						  ((v).m = (v).n = (i) + 1, kv_roundup32((v).m), \
						   (v).a = (type*)realloc((v).a, sizeof(type) * (v).m), 0) \
						  : (v).n <= (size_t)(i)? (v).n = (i)			\
						  : 0), (v).a[(i)]

#endif // AC_KVEC_H

///-----------------------------------------------
constexpr auto test_array_size = 0xFFFF ;
using test_array_type = int ;
///-----------------------------------------------
UBENCH( kvec_std_vec , kvec_resize )  {
	kvec_t(test_array_type) array_;
	kv_init(array_);
	kv_resize(test_array_type, array_, test_array_size);
	kv_destroy(array_);
}
///-----------------------------------------------
UBENCH( kvec_std_vec , kvec_push )  {
	kvec_t(test_array_type) array_;
	kv_init(array_);
	for( int count_ = 0 ;  count_ < test_array_size; ++count_ ) 
		kv_push(test_array_type, array_, count_);
	kv_destroy(array_);
}
///-----------------------------------------------
UBENCH( kvec_std_vec , std_vec_resize )  {
	
	std::vector<test_array_type> array_{};
	array_.resize(test_array_size);
}
///-----------------------------------------------
UBENCH( kvec_std_vec , std_vec_push )  {
	
		std::vector<test_array_type> array_{};
		for( int count_ = 0 ;  count_ < test_array_size; ++count_ )  
		array_.push_back(count_);
}
///-----------------------------------------------

