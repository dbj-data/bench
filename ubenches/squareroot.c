#include <assert.h>

#if DBJ_USE_UTEST
#include "../ubut/utest.h"
#endif

#if DBJ_USE_UBENCH
#include "../ubut/ubench.h"
#endif

// warning C4244: 'return': conversion from 'double' to 'int', possible loss of data
#pragma warning( push )
#pragma warning( disable : 4244 )

// https://stackoverflow.com/a/27958565/10870835
// faster vs hand made implementations in here 
// but **sometimes** ridiculously slow when compared to math.h sqrt
static double squareroot(double x) {

// be aware: assert slows things down considerably
assert(x >= 0);

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
#if DBJ_USE_UBENCH
static double data_ = 0;

void construct(void);
UBUT_INITIALIZER(construct) { data_ = 0xFFF ; }

UBENCH(sqrt_algo, sqrt_int) { sqrt_int(data_); }

UBENCH(sqrt_algo, crt_sqrt) { (void)sqrt(data_); }

UBENCH(sqrt_algo, sqrt_newton) { sqrt_newton(data_); }

#endif // DBJ_USE_UBENCH

/* ---------------------------------------------------------- */

#pragma warning( pop )
