// (c) dbj.org -- DBJ_LICENSE -- https://dbj.org/dbj_license
#include <stdint.h>
#include <time.h>

#include "../ubench.h/ubench.h"

#ifndef _MSC_VER
#error This is Windows build only
#endif

//
// the way of 'fixtures' but without macros
// so that one can see what is happening
// on windows build
//

// https://codingforspeed.com/c-coding-example-using-hash-algorithm-to-remove-duplicate-number-by-on/

#ifdef BEFORE

enum { TESTU_HASHSIZE = 1048576 }; // 20MB
static const int TESTU_HASH = TESTU_HASHSIZE - 1;
// the data numbers can't be the value of TESTU_HASHSIZE,
enum { FREE_SLOT_MARK = TESTU_HASHSIZE };
enum { TESTU_MAXN = 0xFFFF };

#else // AFTER still does not help

#define TESTU_HASHSIZE 1048576
#define TESTU_HASH TESTU_HASHSIZE - 1
// the data numbers can't be the value of TESTU_HASHSIZE,
#define FREE_SLOT_MARK TESTU_HASHSIZE 
#define TESTU_MAXN 0xFFFF

#endif // AFTER


#ifndef _WIN64
static int hashKey(long x) {
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = ((x >> 16) ^ x) * 0x45d9f3b;
  x = (x >> 16) ^ x;
  return x;
}
#else  // _WIN64
static uint64_t hashKey(uint64_t x) {
  x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
  x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
  x = x ^ (x >> 31);
  return x;
}
#endif // _WIN64
/******************************************************************/
// this is the 'fixture'
struct my_data {
  int HASH ;
  int hasharr[TESTU_HASHSIZE];
  long data[TESTU_MAXN] ;
};
/******************************************************************/

static void *produce_uniques(struct my_data *ubench_fixture) {
  int count = 0;
  for (int i = 0; i < TESTU_MAXN; i++) {
    int key = hashKey(ubench_fixture->data[i]); // unbound hash key
    key &= TESTU_HASH;                          // bound it to hash index
    for (;;) {
      if (ubench_fixture->hasharr[key] == FREE_SLOT_MARK) { // slot is available
        ubench_fixture->hasharr[key] = i; // means the number appears first time
        break;
      } else if (ubench_fixture->data[ubench_fixture->hasharr[key]] ==
                 ubench_fixture->data[i]) { // same number
        count++;
        break;
      } else {
        key = (key + 1) & TESTU_HASH; // different number, collision happens
      }
    }
  }

  return ubench_fixture;
}

// this is to be called once from the loop function
#if 0
UBENCH_F_SETUP(my_data)
#else
static void ubench_f_setup_my_data(struct my_data *ubench_fixture)
#endif
{
  ubench_fixture->HASH = TESTU_HASHSIZE - 1;
  // make random data
  srand(time(NULL));
  for (int i = 0; i < TESTU_MAXN; i++)
    ubench_fixture->data[i] = rand() % TESTU_MAXN;
  // init the hash table by using the special value
  for (int i = 0; i < TESTU_HASHSIZE; i++)
    ubench_fixture->hasharr[i] = FREE_SLOT_MARK;

  printf("\nLeaving %s", __FUNCSIG__);
}

#if 0
UBENCH_F_TEARDOWN(my_data)
#else
static void
ubench_f_teardown_my_data(struct my_data *ubench_fixture)
#endif
{
  memset(ubench_fixture->data, 0L, __crt_countof(ubench_fixture->data));
  memset(ubench_fixture->hasharr, 0, __crt_countof(ubench_fixture->hasharr));
}

#if 0
UBENCH_F(my_data, dbj_uniques_arr ) 
{
    UBENCH_F_SETUP(my_data) ;
    produce_uniques( ubench_fixture ) ;
}

#else

extern struct ubench_state_s ubench_state;

static void ubench_f_setup_my_data(struct my_data *);

static void ubench_f_teardown_my_data(struct my_data *);

static void ubench_run_my_data_dbj_uniques_arr(struct my_data *);

/*
in here is the loop
this is registered as a benchmark function
the problem is calling this function produces stack overflow
Why?
*/
/* dbj: removed --> static */ void ubench_f_my_data_dbj_uniques_arr
(ubench_int64_t *const ns, const ubench_int64_t size) 
{ // <-- dbj: cl.exe __chkstk() kicks in here and stops the show
  ubench_int64_t i = 0;
  struct my_data fixture;
  memset(&fixture, 0, sizeof(fixture));
  /* setup is called from here -- once */
  ubench_f_setup_my_data(&fixture);
  for (i = 0; i < size; i++) {
    ns[i] = ubench_ns();
    ubench_run_my_data_dbj_uniques_arr(&fixture);
    ns[i] = ubench_ns() - ns[i];
  }
  ubench_f_teardown_my_data(&fixture);
}

/* this is forward declaration */
#ifdef BEFORE
static void __cdecl ubench_register_my_data_dbj_uniques_arr(void);

__pragma(comment(linker, "/include:"
                         "ubench_register_my_data_dbj_uniques_arr"
                         "_"));
__declspec(allocate(".CRT$XCU")) void(
    __cdecl *ubench_register_my_data_dbj_uniques_arr_)(void) =
    ubench_register_my_data_dbj_uniques_arr;
#endif // BEFORE

/*
this is the registration function itself
this *is* indeed called before main starts
*/
static void __cdecl ubench_register_my_data_dbj_uniques_arr(void) {
  const size_t index = ubench_state.benchmarks_length++;
  const char *name_part = "my_data"
                          "."
                          "dbj_uniques_arr";
  const size_t name_size = strlen(name_part) + 1;
  char *name = ((char *)malloc(name_size));
  ubench_state.benchmarks = ((struct ubench_benchmark_state_s *)realloc(
      ((void *)ubench_state.benchmarks),
      sizeof(struct ubench_benchmark_state_s) *
          ubench_state.benchmarks_length));
  ubench_state.benchmarks[index].func =
      /* dbj:removed --> & , does not help */ubench_f_my_data_dbj_uniques_arr;
  ubench_state.benchmarks[index].name = name;
  _snprintf_s(name, name_size, name_size, "%s", name_part);
}

#ifndef BEFORE
// no forward declaration --> static void __cdecl ubench_register_my_data_dbj_uniques_arr(void);

__pragma(comment(linker, "/include:"
                         "ubench_register_my_data_dbj_uniques_arr"
                         "_"));
__declspec(allocate(".CRT$XCU")) void(
    __cdecl *ubench_register_my_data_dbj_uniques_arr_)(void) =
    ubench_register_my_data_dbj_uniques_arr;
#endif // ! BEFORE

/*
this is called from the loop
*/
void ubench_run_my_data_dbj_uniques_arr(
    struct my_data *ubench_fixture) {
  // UBENCH_F_SETUP(my_data) ;
  produce_uniques(ubench_fixture);
}

#endif // ! 0
