/*
(C) 2020 by dbj@dbj.org -- LICENSE_DBJ -- https://dbj.org/license_dbj
*/

#ifndef UBUT_UBENCH_H_INCLUDED
#define UBUT_UBENCH_H_INCLUDED

#include "ubut_top.h"
#include "ubut_print.h"

typedef void (*ubench_benchmark_t)(ubut_int64_t *const, const ubut_int64_t);

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
UBUT_EXTERN struct ubench_state_s ubench_state;

#define UBENCH_REZ_OUT(...)                                                     \
  if (ubench_state.output) {                                                   \
    fprintf(ubench_state.output, __VA_ARGS__);                                 \
  }   
/*
-------------------------------------------------------------------------------
ubench begins here
-------------------------------------------------------------------------------
*/

#define UBENCH(SET, NAME)                                                      \
  UBUT_EXTERN struct ubench_state_s ubench_state;                            \
  static void ubench_run_##SET##_##NAME(void);                                 \
  static void ubench_##SET##_##NAME(ubut_int64_t *const ns,                  \
                                    const ubut_int64_t size) {               \
    ubut_int64_t i = 0;                                                      \
    for (i = 0; i < size; i++) {                                               \
      ns[i] = ubut_ns();                                                     \
      ubench_run_##SET##_##NAME();                                             \
      ns[i] = ubut_ns() - ns[i];                                             \
    }                                                                          \
  }                                                                            \
  UBUT_INITIALIZER(ubench_register_##SET##_##NAME) {                         \
    const size_t index = ubench_state.benchmarks_length++;                     \
    const char *name_part = #SET "." #NAME;                                    \
    const size_t name_size = strlen(name_part) + 1;                            \
    char *name = UBUT_PTR_CAST(char *, malloc(name_size));                   \
    ubench_state.benchmarks = UBUT_PTR_CAST(                                 \
        struct ubench_benchmark_state_s *,                                     \
        realloc(UBUT_PTR_CAST(void *, ubench_state.benchmarks),              \
                sizeof(struct ubench_benchmark_state_s) *                      \
                    ubench_state.benchmarks_length));                          \
    ubench_state.benchmarks[index].func = &ubench_##SET##_##NAME;              \
    ubench_state.benchmarks[index].name = name;                                \
    UBUT_SNPRINTF(name, name_size, "%s", name_part);                         \
  }                                                                            \
  void ubench_run_##SET##_##NAME(void)

UBUT_FORCEINLINE
int ubench_should_filter(const char *filter, const char *benchmark);

UBUT_FORCEINLINE int ubench_should_filter(const char *filter,
                                     const char *benchmark) {
  if (filter) {
    const char *filter_cur = filter;
    const char *benchmark_cur = benchmark;
    const char *filter_wildcard = UBUT_NULL;

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


/* Informational switches */
#define HELP_STR "--help"
#define LIST_STR "--list"
/* Benchmark config switches */
#define FILTER_STR "--filter="
#define OUTPUT_STR "--output="
#define CONFIDENCE_STR "--confidence=" 
#define SLEN(S) (sizeof(S) - 1)

#define PFX_DIVIDER "[----------]"
#define     PFX_RUN "[ RUN      ]"
#define  PFX_FAILED UBUT_VT_YELLOW "[  FAILED  ]" UBUT_VT_RESET
#define  PFX_PASSED "[  PASSED  ]"
#define      PFX_OK "[      OK  ]"

UBUT_FORCEINLINE int ubench_main(int /*argc*/, const char *const /*argv*/[]);
UBUT_FORCEINLINE int ubench_main(int argc, const char *const argv[]) {
  ubut_uint64_t failed = 0;
  size_t index = 0;
  size_t *failed_benchmarks = UBUT_NULL;
  size_t failed_benchmarks_length = 0;
  const char *filter = UBUT_NULL;
  ubut_uint64_t ran_benchmarks = 0;

  static const char *const FOPEN_MODE = "w+";
// static const char *const FOPEN_MODE = "a+";

  /* loop through all arguments looking for our options */
  for (index = 1; index < UBUT_CAST(size_t, argc); index++) {

    if (0 == ubut_strncmp(argv[index], HELP_STR, SLEN(HELP_STR))) {

UBUT_INFO( "ubench - the benchmarking solution" ) ;
UBUT_INFO( "Command line Options:" ) ;
UBUT_INFO( "  --help                   Show this message and exit." ) ;
UBUT_INFO("  --filter=<filter>         Filter the benchmarks to run (EG.)");
UBUT_INFO( "MyBench*.a would run MyBenchmark.a but not MyBenchmark.b)." ) ;
UBUT_INFO("  %s         List benchmarks, one per line. ", LIST_STR );
UBUT_INFO( "Output names can be passed to --filter." ) ;
UBUT_INFO( "  --output=<output>         Output a CSV file of the results." ) ;
UBUT_INFO("  --confidence=<confidence> Change the confidence cut-off for a ");
UBUT_INFO( "failed test. Defaults to 2.5%%" ) ;

      goto cleanup;
    } else if (0 == ubut_strncmp(argv[index], FILTER_STR, SLEN(FILTER_STR))) {
      /* user wants to filter what benchmarks to run! */
      filter = argv[index] + SLEN(FILTER_STR);
    } else if (0 == ubut_strncmp(argv[index], OUTPUT_STR, SLEN(OUTPUT_STR))) {
        /* user has given rezult file name */
        ubench_state.output =
          ubut_fopen(argv[index] + SLEN(OUTPUT_STR), FOPEN_MODE);
    } else if (0 == ubut_strncmp(argv[index], LIST_STR, SLEN(LIST_STR))) {
        /* user wants a list of benchmarks */
        UBUT_INFO(" ");
        UBUT_INFO("List of benchmarks");
        UBUT_INFO(" ");
        for (index = 0; index < ubench_state.benchmarks_length; index++) {
            UBUT_INFO("%-4d: %s", index, ubench_state.benchmarks[index].name);
        }
        UBUT_INFO(" ");
        UBUT_INFO("To run exact benchmark or group, filtered by name, use the --filter option");
        UBUT_INFO(" ");
        /* after listing the benchmark list, don't actually proceed to run the benchmarks */
      goto cleanup;
    } else if (0 == ubut_strncmp(argv[index], CONFIDENCE_STR,
                                   SLEN(CONFIDENCE_STR))) {
      /* user wants to specify a different confidence */
      ubench_state.confidence = atof(argv[index] + SLEN(CONFIDENCE_STR));

      /* must be between 0 and 100 */
      if ((ubench_state.confidence < 0) || (ubench_state.confidence > 100)) {
          UBUT_ERROR(
                "Confidence must be in the range [0..100] (you specified %f)",
                ubench_state.confidence);
        goto cleanup;
      }
    }
  } // for()

  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    if (ubench_should_filter(filter, ubench_state.benchmarks[index].name)) {
      continue;
    }

    ran_benchmarks++;
  }

  UBUT_INFO(PFX_DIVIDER "Running %" UBUT_PRIu64 " benchmarks.",
         UBUT_CAST(ubut_uint64_t, ran_benchmarks));
  // header for the rezult file
  UBENCH_REZ_OUT("name, mean (ns), stddev (%%), confidence (%%)\n");

#define UBENCH_MIN_ITERATIONS 10
#define UBENCH_MAX_ITERATIONS 500
  static const ubut_int64_t max_iterations = UBENCH_MAX_ITERATIONS;
  static const ubut_int64_t min_iterations = UBENCH_MIN_ITERATIONS;

  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    int result = 1;
    size_t mndex = 0;
    ubut_int64_t best_avg_ns = 0;
    double best_deviation = 0;
    double best_confidence = 101.0;

    ubut_int64_t iterations = 10;
    ubut_int64_t ns[UBENCH_MAX_ITERATIONS] = {0};

#undef UBENCH_MAX_ITERATIONS
#undef UBENCH_MIN_ITERATIONS

    if (ubench_should_filter(filter, ubench_state.benchmarks[index].name)) {
      continue;
    }

    UBUT_INFO( PFX_RUN "%s", ubench_state.benchmarks[index].name);

    // Time once to work out the base number of iterations to use.
    ubench_state.benchmarks[index].func(ns, 1);

    iterations = (100 * 1000 * 1000) / ns[0];
    iterations = iterations < min_iterations ? min_iterations : iterations;
    iterations = iterations > max_iterations ? max_iterations : iterations;

    for (mndex = 0; (mndex < 100) && (result != 0); mndex++) {
      ubut_int64_t kndex = 0;
      ubut_int64_t avg_ns = 0;
      double deviation = 0;
      double confidence = 0;

      iterations = iterations * (UBUT_CAST(ubut_int64_t, mndex) + 1);
      iterations = iterations > max_iterations ? max_iterations : iterations;

      ubench_state.benchmarks[index].func(ns, iterations);

      for (kndex = 0; kndex < iterations; kndex++) {
        avg_ns += ns[kndex];
      }

      avg_ns /= iterations;

      for (kndex = 0; kndex < iterations; kndex++) {
        const double v = UBUT_CAST(double, ns[kndex] - avg_ns);
        deviation += v * v;
      }

      deviation = sqrt(deviation / iterations);

      // Confidence is the 99% confidence index - whose magic value is 2.576.
      confidence = 2.576 * deviation / sqrt(UBUT_CAST(double, iterations));
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
        UBUT_WARN("confidence interval %f%% exceeds maximum permitted %f%%",
             best_confidence, ubench_state.confidence);
    }

      UBENCH_REZ_OUT("%s, %" UBUT_PRId64 ", %f, %f,\n",
              ubench_state.benchmarks[index].name, best_avg_ns, best_deviation,
              best_confidence);

    {
      // const char *const colour = (0 != result) ? colours[RED] : colours[GREEN];
      const char *const status =
          (0 != result) ? PFX_FAILED : PFX_OK ;
      const char *unit = "us";

      if (0 != result) {
        const size_t failed_benchmark_index = failed_benchmarks_length++;

#pragma warning( push )
#pragma warning( disable : 6308 )

        failed_benchmarks = UBUT_PTR_CAST(
            size_t *, realloc(UBUT_PTR_CAST(void *, failed_benchmarks),
                              sizeof(size_t) * failed_benchmarks_length));

#pragma warning( pop )

        failed_benchmarks[failed_benchmark_index] = index;
        failed++;
      }

      // UBUT_INFO("%s%s (mean ", status, ubench_state.benchmarks[index].name);

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

      UBUT_INFO( "%s%s (mean " "%" UBUT_PRId64 ".%03" UBUT_PRId64
             "%s, confidence interval +- %f%%)",
          status, ubench_state.benchmarks[index].name,
             best_avg_ns / 1000, best_avg_ns % 1000, unit, best_confidence);
    }
  }

  UBUT_INFO(PFX_DIVIDER "%" UBUT_PRIu64 " benchmarks ran.", ran_benchmarks);
  UBUT_INFO(PFX_PASSED "%" UBUT_PRIu64 " benchmarks.", ran_benchmarks - failed);

  if (0 != failed) {
      UBUT_WARN(PFX_FAILED"%" UBUT_PRIu64 " benchmarks, listed below:", failed);
    for (index = 0; index < failed_benchmarks_length; index++) {
        UBUT_WARN(PFX_FAILED"%s", ubench_state.benchmarks[failed_benchmarks[index]].name);
    }
  }

cleanup:
  for (index = 0; index < ubench_state.benchmarks_length; index++) {
    free(UBUT_PTR_CAST(void *, ubench_state.benchmarks[index].name));
  }

  free(UBUT_PTR_CAST(void *, failed_benchmarks));
  free(UBUT_PTR_CAST(void *, ubench_state.benchmarks));

  if (ubench_state.output) {
    fclose(ubench_state.output);
  }

  return UBUT_CAST(int, failed);
}

#undef HELP_STR
#undef LIST_STR
#undef FILTER_STR
#undef OUTPUT_STR
#undef CONFIDENCE_STR
#undef SLEN

#undef PFX_DIVIDER
#undef     PFX_RUN
#undef  PFX_FAILED
#undef  PFX_PASSED
#undef      PFX_OK


UBUT_C_FUNC UBUT_NOINLINE void ubench_do_nothing(void *const);

#define UBENCH_DO_NOTHING(x) ubench_do_nothing(x)

#ifdef  __clang__
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    _Pragma("clang diagnostic push")                                           \
        _Pragma("clang diagnostic ignored \"-Wlanguage-extension-token\"");    \
    asm volatile("" : : "r,m"(ptr) : "memory");                                \
    _Pragma("clang diagnostic pop");                                           \
  }

#else //UBENCH_IS_WIN

// https://github.com/MicrosoftDocs/cpp-docs/issues/2601#issue-744464977
// see also the top of the ubut_top.h
// 
#define UBENCH_DECLARE_DO_NOTHING()                                            \
  void ubench_do_nothing(void *ptr) {                                          \
    (void)ptr;                                                                 \
    _Compiler_barrier();                                                       \
  }

#endif

/*
   We need, in exactly one source file, define the global struct that will hold
   the data we need to run ubench. This macro allows the user to declare the
   data without having to use the UBENCH_MAIN macro, thus allowing them to write
   their own main() function.

   We also use this to define the 'do nothing' method that lets us keep data
   that the compiler would normally deem is dead for the purposes of timing.
*/
#define UBENCH_STATE                                                         \
  UBENCH_DECLARE_DO_NOTHING() ;                                                 \
  struct ubench_state_s ubench_state = {0, 0, 0, 2.5}

/*
   define a main() function to call into ubench.h and start executing
   benchmarks! A user can optionally not use this macro, and instead define
   their own main() function and manually call ubench_main. The user must, in
   exactly one source file, use the UBENCH_STATE macro to declare a global
   struct variable that ubench requires.

 #define UBENCH_MAIN()                                                          \
  UBENCH_STATE();                                                              \
  int main(int argc, const char *const argv[]) {                               \
    return ubench_main(argc, argv);                                            \
  }
  */

#endif /* UBUT_UBENCH_H_INCLUDED */