/*
(C) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj
*/

#ifndef UBUT_UBENCH_H_INCLUDED
#define UBUT_UBENCH_H_INCLUDED

#include "ubut_top.h"


static UBENCH_INLINE ubench_int64_t ubench_ns(void) {
//#ifdef UBENCH_IS_WIN
  LARGE_INTEGER counter;
  LARGE_INTEGER frequency;
  QueryPerformanceCounter(&counter);
  QueryPerformanceFrequency(&frequency);
  return UBENCH_CAST(ubench_int64_t,
                     (counter.QuadPart * 1000000000) / frequency.QuadPart);
}

typedef void (*ubench_benchmark_t)(ubench_int64_t *const, const ubench_int64_t);

struct ubench_benchmark_state_s {
  ubench_benchmark_t func;
  char *name;
};

struct ubench_state_s {
  struct ubench_benchmark_state_s *benchmarks;
  size_t benchmarks_length;
  FILE *output;
  double confidence;
};

/* extern to the global state ubench needs to execute */
UBENCH_EXTERN struct ubench_state_s ubench_state;

/*
-------------------------------------------------------------------------------
ubench begins here
-------------------------------------------------------------------------------
*/

#define UBENCH(SET, NAME)                                                      \
  UBENCH_EXTERN struct ubench_state_s ubench_state;                            \
  static void ubench_run_##SET##_##NAME(void);                                 \
  static void ubench_##SET##_##NAME(ubench_int64_t *const ns,                  \
                                    const ubench_int64_t size) {               \
    ubench_int64_t i = 0;                                                      \
    for (i = 0; i < size; i++) {                                               \
      ns[i] = ubench_ns();                                                     \
      ubench_run_##SET##_##NAME();                                             \
      ns[i] = ubench_ns() - ns[i];                                             \
    }                                                                          \
  }                                                                            \
  UBENCH_INITIALIZER(ubench_register_##SET##_##NAME) {                         \
    const size_t index = ubench_state.benchmarks_length++;                     \
    const char *name_part = #SET "." #NAME;                                    \
    const size_t name_size = strlen(name_part) + 1;                            \
    char *name = UBENCH_PTR_CAST(char *, malloc(name_size));                   \
    ubench_state.benchmarks = UBENCH_PTR_CAST(                                 \
        struct ubench_benchmark_state_s *,                                     \
        realloc(UBENCH_PTR_CAST(void *, ubench_state.benchmarks),              \
                sizeof(struct ubench_benchmark_state_s) *                      \
                    ubench_state.benchmarks_length));                          \
    ubench_state.benchmarks[index].func = &ubench_##SET##_##NAME;              \
    ubench_state.benchmarks[index].name = name;                                \
    UBENCH_SNPRINTF(name, name_size, "%s", name_part);                         \
  }                                                                            \
  void ubench_run_##SET##_##NAME(void)

#pragma region fixtures
#if DBJ_USES_UBENCH_FIXTURE

#define UBENCH_F_SETUP(FIXTURE)                                                \
  static void ubench_f_setup_##FIXTURE(struct FIXTURE *ubench_fixture)

#define UBENCH_F_TEARDOWN(FIXTURE)                                             \
  static void ubench_f_teardown_##FIXTURE(struct FIXTURE *ubench_fixture)

#define UBENCH_F(FIXTURE, NAME)                                                \
  UBENCH_EXTERN struct ubench_state_s ubench_state;                            \
  static void ubench_f_setup_##FIXTURE(struct FIXTURE *);                      \
  static void ubench_f_teardown_##FIXTURE(struct FIXTURE *);                   \
  static void ubench_run_##FIXTURE##_##NAME(struct FIXTURE *);                 \
  static void ubench_f_##FIXTURE##_##NAME(ubench_int64_t *const ns,            \
                                          const ubench_int64_t size) {         \
    ubench_int64_t i = 0;                                                      \
    struct FIXTURE fixture;                                                    \
    memset(&fixture, 0, sizeof(fixture));                                      \
    ubench_f_setup_##FIXTURE(&fixture);                                        \
    for (i = 0; i < size; i++) {                                               \
      ns[i] = ubench_ns();                                                     \
      ubench_run_##FIXTURE##_##NAME(&fixture);                                 \
      ns[i] = ubench_ns() - ns[i];                                             \
    }                                                                          \
    ubench_f_teardown_##FIXTURE(&fixture);                                     \
  }                                                                            \
  UBENCH_INITIALIZER(ubench_register_##FIXTURE##_##NAME) {                     \
    const size_t index = ubench_state.benchmarks_length++;                     \
    const char *name_part = #FIXTURE "." #NAME;                                \
    const size_t name_size = strlen(name_part) + 1;                            \
    char *name = UBENCH_PTR_CAST(char *, malloc(name_size));                   \
    ubench_state.benchmarks = UBENCH_PTR_CAST(                                 \
        struct ubench_benchmark_state_s *,                                     \
        realloc(UBENCH_PTR_CAST(void *, ubench_state.benchmarks),              \
                sizeof(struct ubench_benchmark_state_s) *                      \
                    ubench_state.benchmarks_length));                          \
    ubench_state.benchmarks[index].func = &ubench_f_##FIXTURE##_##NAME;        \
    ubench_state.benchmarks[index].name = name;                                \
    UBENCH_SNPRINTF(name, name_size, "%s", name_part);                         \
  }                                                                            \
  void ubench_run_##FIXTURE##_##NAME(struct FIXTURE *ubench_fixture)

#endif // DBJ_USES_UBENCH_FIXTURE
#pragma endregion fixtures


UBENCH_WEAK
int ubench_should_filter(const char *filter, const char *benchmark);

UBENCH_WEAK int ubench_should_filter(const char *filter,
                                     const char *benchmark) {
  if (filter) {
    const char *filter_cur = filter;
    const char *benchmark_cur = benchmark;
    const char *filter_wildcard = UBENCH_NULL;

    while (('\0' != *filter_cur) && ('\0' != *benchmark_cur)) {
      if ('*' == *filter_cur) {
        /* store the position of the wildcard */
        filter_wildcard = filter_cur;

        /* skip the wildcard character */
        filter_cur++;

        while (('\0' != *filter_cur) && ('\0' != *benchmark_cur)) {
          if ('*' == *filter_cur) {
            /*
               we found another wildcard (filter is something like *foo*) so we
               exit the current loop, and return to the parent loop to handle
               the wildcard case
            */
            break;
          } else if (*filter_cur != *benchmark_cur) {
            /* otherwise our filter didn't match, so reset it */
            filter_cur = filter_wildcard;
          }

          /* move benchmark along */
          benchmark_cur++;

          /* move filter along */
          filter_cur++;
        }

        if (('\0' == *filter_cur) && ('\0' == *benchmark_cur)) {
          return 0;
        }

        /* if the benchmarks have been exhausted, we don't have a match! */
        if ('\0' == *benchmark_cur) {
          return 1;
        }
      } else {
        if (*benchmark_cur != *filter_cur) {
          /* benchmark doesn't match filter */
          return 1;
        } else {
          /* move our filter and benchmark forward */
          benchmark_cur++;
          filter_cur++;
        }
      }
    }

    if (('\0' != *filter_cur) ||
        (('\0' != *benchmark_cur) &&
         ((filter == filter_cur) || ('*' != filter_cur[-1])))) {
      /* we have a mismatch! */
      return 1;
    }
  }

  return 0;
}

// UBENCH_WEAK
int ubench_main(int argc, const char *const argv[]);

inline int ubench_main(int argc, const char *const argv[]) {
  ubench_uint64_t failed = 0;
  size_t index = 0;
  size_t *failed_benchmarks = UBENCH_NULL;
  size_t failed_benchmarks_length = 0;
  const char *filter = UBENCH_NULL;
  ubench_uint64_t ran_benchmarks = 0;

  enum colours { RESET, GREEN, RED };

  // const int use_colours = UBENCH_COLOUR_OUTPUT();
  static char *colours[] = {(char *)"\033[0m", (char *)"\033[32m",
                            (char *)"\033[31m"};

  if (FALSE == UBENCH_COLOUR_OUTPUT()) {
    static const char *no_colurs[] = {"", "", ""};
    memcpy(colours, no_colurs, 3 * sizeof(char *));
  }

  static const char *const FOPEN_MODE = "w+";
// static const char *const FOPEN_MODE = "a+";

/* Informational switches */
#define HELP_STR "--help"
#define LIST_STR "--list-benchmarks"
/* Benchmark config switches */
#define FILTER_STR "--filter="
#define OUTPUT_STR "--output="
#define CONFIDENCE_STR "--confidence="
#define SLEN(S) (sizeof(S) - 1)

  /* loop through all arguments looking for our options */
  for (index = 1; index < UBENCH_CAST(size_t, argc); index++) {

    if (0 == ubench_strncmp(argv[index], HELP_STR, SLEN(HELP_STR))) {
      printf("ubench.h - the single file benchmarking solution for C/C++!\n"
             "Command line Options:\n");
      printf("  --help                    Show this message and exit.\n"
             "  --filter=<filter>         Filter the benchmarks to run (EG. "
             "MyBench*.a would run MyBenchmark.a but not MyBenchmark.b).\n"
             "  --list-benchmarks         List benchmarks, one per line. "
             "Output names can be passed to --filter.\n"
             "  --output=<output>         Output a CSV file of the results.\n"
             "  --confidence=<confidence> Change the confidence cut-off for a "
             "failed test. Defaults to 2.5%%\n");
      goto cleanup;
    } else if (0 == ubench_strncmp(argv[index], FILTER_STR, SLEN(FILTER_STR))) {
      /* user wants to filter what benchmarks to run! */
      filter = argv[index] + SLEN(FILTER_STR);
    } else if (0 == ubench_strncmp(argv[index], OUTPUT_STR, SLEN(OUTPUT_STR))) {
        /* user has given rezult file name */
        ubench_state.output =
          ubench_fopen(argv[index] + SLEN(OUTPUT_STR), FOPEN_MODE);
    } else if (0 == ubench_strncmp(argv[index], LIST_STR, SLEN(LIST_STR))) {
        /* user wants a list of benchmarks */
        for (index = 0; index < ubench_state.benchmarks_length; index++) {
        UBENCH_PRINTF("%s\n", ubench_state.benchmarks[index].name);
      }
      /* when printing the benchmark list, don't actually proceed to run the benchmarks */
      goto cleanup;
    } else if (0 == ubench_strncmp(argv[index], CONFIDENCE_STR,
                                   SLEN(CONFIDENCE_STR))) {
      /* user wants to specify a different confidence */
      ubench_state.confidence = atof(argv[index] + SLEN(CONFIDENCE_STR));

      /* must be between 0 and 100 */
      if ((ubench_state.confidence < 0) || (ubench_state.confidence > 100)) {
        fprintf(stderr,
                "Confidence must be in the range [0..100] (you specified %f)\n",
                ubench_state.confidence);
        goto cleanup;
      }
    }
  } // for()

#undef HELP_STR
#undef LIST_STR
#undef FILTER_STR
#undef OUTPUT_STR
#undef CONFIDENCE_STR
#undef SLEN

  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    if (ubench_should_filter(filter, ubench_state.benchmarks[index].name)) {
      continue;
    }

    ran_benchmarks++;
  }

  printf("%s[==========]%s Running %" UBENCH_PRIu64 " benchmarks.\n",
         colours[GREEN], colours[RESET],
         UBENCH_CAST(ubench_uint64_t, ran_benchmarks));

  if (ubench_state.output) {
    fprintf(ubench_state.output,
            "name, mean (ns), stddev (%%), confidence (%%)\n");
  }

#define UBENCH_MIN_ITERATIONS 10
#define UBENCH_MAX_ITERATIONS 500
  static const ubench_int64_t max_iterations = UBENCH_MAX_ITERATIONS;
  static const ubench_int64_t min_iterations = UBENCH_MIN_ITERATIONS;

  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    int result = 1;
    size_t mndex = 0;
    ubench_int64_t best_avg_ns = 0;
    double best_deviation = 0;
    double best_confidence = 101.0;

    ubench_int64_t iterations = 10;
    ubench_int64_t ns[UBENCH_MAX_ITERATIONS] = {0};

#undef UBENCH_MAX_ITERATIONS
#undef UBENCH_MIN_ITERATIONS

    if (ubench_should_filter(filter, ubench_state.benchmarks[index].name)) {
      continue;
    }

    printf("%s[ RUN      ]%s %s\n", colours[GREEN], colours[RESET],
           ubench_state.benchmarks[index].name);

    // Time once to work out the base number of iterations to use.
    ubench_state.benchmarks[index].func(ns, 1);

    iterations = (100 * 1000 * 1000) / ns[0];
    iterations = iterations < min_iterations ? min_iterations : iterations;
    iterations = iterations > max_iterations ? max_iterations : iterations;

    for (mndex = 0; (mndex < 100) && (result != 0); mndex++) {
      ubench_int64_t kndex = 0;
      ubench_int64_t avg_ns = 0;
      double deviation = 0;
      double confidence = 0;

      iterations = iterations * (UBENCH_CAST(ubench_int64_t, mndex) + 1);
      iterations = iterations > max_iterations ? max_iterations : iterations;

      ubench_state.benchmarks[index].func(ns, iterations);

      for (kndex = 0; kndex < iterations; kndex++) {
        avg_ns += ns[kndex];
      }

      avg_ns /= iterations;

      for (kndex = 0; kndex < iterations; kndex++) {
        const double v = UBENCH_CAST(double, ns[kndex] - avg_ns);
        deviation += v * v;
      }

      deviation = sqrt(deviation / iterations);

      // Confidence is the 99% confidence index - whose magic value is 2.576.
      confidence = 2.576 * deviation / sqrt(UBENCH_CAST(double, iterations));
      confidence = (confidence / avg_ns) * 100;

      deviation = (deviation / avg_ns) * 100;

      // If we've found a more confident solution, use that.
      result = confidence > ubench_state.confidence;

      // If the deviation beats our previous best, record it.
      if (confidence < best_confidence) {
        best_avg_ns = avg_ns;
        best_deviation = deviation;
        best_confidence = confidence;
      }
    }

    if (result) {
      printf("confidence interval %f%% exceeds maximum permitted %f%%\n",
             best_confidence, ubench_state.confidence);
    }

    if (ubench_state.output) {
      fprintf(ubench_state.output, "%s, %" UBENCH_PRId64 ", %f, %f,\n",
              ubench_state.benchmarks[index].name, best_avg_ns, best_deviation,
              best_confidence);
    }

    {
      const char *const colour = (0 != result) ? colours[RED] : colours[GREEN];
      const char *const status =
          (0 != result) ? "[  FAILED  ]" : "[       OK ]";
      const char *unit = "us";

      if (0 != result) {
        const size_t failed_benchmark_index = failed_benchmarks_length++;
        failed_benchmarks = UBENCH_PTR_CAST(
            size_t *, realloc(UBENCH_PTR_CAST(void *, failed_benchmarks),
                              sizeof(size_t) * failed_benchmarks_length));
        failed_benchmarks[failed_benchmark_index] = index;
        failed++;
      }

      printf("%s%s%s %s (mean ", colour, status, colours[RESET],
             ubench_state.benchmarks[index].name);

      for (mndex = 0; mndex < 2; mndex++) {
        if (best_avg_ns <= 1000000) {
          break;
        }

        // If the average is greater than a million, we reduce it and change the
        // unit we report.
        best_avg_ns /= 1000;

        switch (mndex) {
        case 0:
          unit = "ms";
          break;
        case 1:
          unit = "s";
          break;
        }
      }

      printf("%" UBENCH_PRId64 ".%03" UBENCH_PRId64
             "%s, confidence interval +- %f%%)\n",
             best_avg_ns / 1000, best_avg_ns % 1000, unit, best_confidence);
    }
  }

  printf("%s[==========]%s %" UBENCH_PRIu64 " benchmarks ran.\n",
         colours[GREEN], colours[RESET], ran_benchmarks);
  printf("%s[  PASSED  ]%s %" UBENCH_PRIu64 " benchmarks.\n", colours[GREEN],
         colours[RESET], ran_benchmarks - failed);

  if (0 != failed) {
    printf("%s[  FAILED  ]%s %" UBENCH_PRIu64 " benchmarks, listed below:\n",
           colours[RED], colours[RESET], failed);
    for (index = 0; index < failed_benchmarks_length; index++) {
      printf("%s[  FAILED  ]%s %s\n", colours[RED], colours[RESET],
             ubench_state.benchmarks[failed_benchmarks[index]].name);
    }
  }

cleanup:
  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    free(UBENCH_PTR_CAST(void *, ubench_state.benchmarks[index].name));
  }

  free(UBENCH_PTR_CAST(void *, failed_benchmarks));
  free(UBENCH_PTR_CAST(void *, ubench_state.benchmarks));

  if (ubench_state.output) {
    fclose(ubench_state.output);
  }

  return UBENCH_CAST(int, failed);
}

UBENCH_C_FUNC UBENCH_NOINLINE void ubench_do_nothing(void *const);

#define UBENCH_DO_NOTHING(x) ubench_do_nothing(x)

#if defined(__clang__)
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    _Pragma("clang diagnostic push")                                           \
        _Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"");    \
    asm volatile("" : : "r,m"(ptr) : "memory");                                \
    _Pragma("clang diagnostic pop");                                           \
  }
#elif defined(UBENCH_IS_WIN)
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    (void)ptr;                                                                 \
    _ReadWriteBarrier();                                                       \
  }
#else
//#define UBENCH_DECLARE_DO_NOTHING()                                            \
//  void ubench_do_nothing(void *ptr) {                                          \
//    asm volatile("" : : "r,m"(ptr) : "memory");                                \
//  }
#endif

/*
   We need, in exactly one source file, define the global struct that will hold
   the data we need to run ubench. This macro allows the user to declare the
   data without having to use the UBENCH_MAIN macro, thus allowing them to write
   their own main() function.

   We also use this to define the 'do nothing' method that lets us keep data
   that the compiler would normally deem is dead for the purposes of timing.
*/
#define UBENCH_STATE()                                                         \
  UBENCH_DECLARE_DO_NOTHING()                                                  \
  struct ubench_state_s ubench_state = {0, 0, 0, 2.5}

/*
   define a main() function to call into ubench.h and start executing
   benchmarks! A user can optionally not use this macro, and instead define
   their own main() function and manually call ubench_main. The user must, in
   exactly one source file, use the UBENCH_STATE macro to declare a global
   struct variable that ubench requires.
*/
#define UBENCH_MAIN()                                                          \
  UBENCH_STATE();                                                              \
  int main(int argc, const char *const argv[]) {                               \
    return ubench_main(argc, argv);                                            \
  }

#endif /* UBUT_UBENCH_H_INCLUDED */