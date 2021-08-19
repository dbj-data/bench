/*
 Copyright 2020 dbj@dbj.org -- https://dbj.org/license_dbj
 */

#pragma once

#ifdef __clang__
 // or use
 // c++20 [[likely]] : https://en.cppreference.com/w/cpp/language/attributes/likely
 //
#undef dbj_likely
#define dbj_likely(cond) __builtin_expect(!!(cond), true)

#undef dbj_unlikely
#define dbj_unlikely(cond) __builtin_expect(!!(cond), false)

#else // ! __clang__

#undef dbj_likely
#define dbj_likely(cond) (cond)

#undef dbj_unlikely
#define dbj_unlikely(cond) (cond)

#endif // ! __clang__

// ps: basically waste of time