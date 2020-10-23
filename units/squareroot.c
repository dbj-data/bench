#include <assert.h>

// #include "../utest.h/utest.h"
#include "../ubench.h/ubench.h"
#include "../fwk/nanoclib.h"

// https://stackoverflow.com/a/27958565/10870835
// faster but ridiculously slow when compared to math.h sqrt
static double squareroot(double x) {
  if (x < 1)
    return 1.0 / squareroot(x); // MSalter's general solution

  double xhi = x;
  double xlo = 0;
  double guess = x / 2;

  while (guess * guess != x) {
    if (guess * guess > x)
      xhi = guess;
    else
      xlo = guess;

    double new_guess = (xhi + xlo) / 2;
    if (new_guess == guess)
      break; // not getting closer
    guess = new_guess;
  }
  return guess;
}

// faster than double version but obviously less precize
static int sqrt_int(int x) {
  if (x < 1)
    return 1.0 / squareroot(x); // MSalter's general solution

  int xhi = x;
  int xlo = 0;
  int guess = x / 2;

  while (guess * guess != x) {
    if (guess * guess > x)
      xhi = guess;
    else
      xlo = guess;

    int new_guess = (xhi + xlo) / 2;
    if (new_guess == guess)
      break; // not getting closer
    guess = new_guess;
  }
  return guess;
}

// this is the slowest of the three
static double sqrt_newton(double x) {
  enum { NUM_OF_ITERATIONS = 24 };

  assert(x >= 0);
  if (x == 0)
    return 0;

  double guess = x;
  for (int i = 0; i < NUM_OF_ITERATIONS; i++)
    guess -= (guess * guess - x) / (2 * guess);
  return guess;
}
/* ---------------------------------------------------------- */

static double data_ = 0;

void construct(void);
UBENCH_INITIALIZER(construct) { data_ = 0xFFF ; }

UBENCH(dbj, sqrt_int) { sqrt_int(data_); }

UBENCH(clib, sqrt) { (void)sqrt(data_); }

UBENCH(dbj, sqrt_newton) { sqrt_newton(data_); }

#ifdef __clang__
// it seems this will be visited only
// if we link with the static runtime lib
void destruct(void) __attribute__((destructor));
static void destruct(void) { data_ = 0; }
#endif // __clang__

#if DBJ_USE_UTEST
 UTEST(dbj, utest) { UTEST_EXPECT(sqrt_int(9), 3, ==); }
#endif
