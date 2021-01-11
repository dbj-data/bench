
#define _CRT_SECURE_NO_WARNINGS 1
#include <ubut/utest.h>
#include <limits.h>

// #if __STDC_VERSION__ >= 201112L || __cplusplus >= 201103L
#ifdef UTEST_HAVING_TYPE_PRINTERS

#ifdef __cplusplus
#pragma message("This is C++")
UTEST(type_printers, cpp) {
#elif defined(UTEST_OVERLOADABLE)
#pragma message("UTEST_OVERLOADABLE defined : ")
UTEST(type_printers, overloadable) {
#elif __STDC_VERSION__
#pragma message("__STDC_VERSION__ defined : "  _CRT_STRINGIZE(__STDC_VERSION__) )
UTEST(type_printers, stdc) {
#else
#error __STDC_VERSION__ not defined and not C++
#endif

#define TO_S_X(val) #val
#define S(val) TO_S_X(val)
FILE* old = utest_state.output;
FILE* out = tmpfile();
ASSERT_TRUE(!!out);
utest_state.output = out;

int i = INT_MIN;
long l = LONG_MIN;
long long ll = LLONG_MIN;
unsigned u = UINT_MAX;
unsigned long ul = ULONG_MAX;
unsigned long long ull = ULLONG_MAX;
float f = 0.f;
double d = 0.;
long double ld = 0.l;
utest_type_printer(i);
utest_type_printer(l);
utest_type_printer(ll);
utest_type_printer(u);
utest_type_printer(ul);
utest_type_printer(ull);
utest_type_printer(f);
utest_type_printer(d);
utest_type_printer(ld);

char expected[1024] = { 0 };
size_t expected_len =
UBUT_SNPRINTF(expected, sizeof expected - 1, "%d%ld%lld%u%lu%llu%f%f%Lf",
	i, l, ll, u, ul, ull, f, d, ld);
fflush(out);
rewind(out);
char buf[1024] = { '\0' };
size_t n = fread(buf, 1, sizeof buf, out);
fclose(out);
utest_state.output = old;
ASSERT_EQ(n, expected_len);
ASSERT_STREQ(buf, expected);
}

#endif // UTEST_HAVING_TYPE_PRINTERS

// #endif // __STDC_VERSION__ >= 201112L || __cplusplus >= 201103L

