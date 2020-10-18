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

enum { TESTU_HASHSIZE = 1048576 }; // 20MB
static const int TESTU_HASH = TESTU_HASHSIZE - 1;
static int hasharr[TESTU_HASHSIZE];

// the data numbers can't be the value of TESTU_HASHSIZE,
enum { FREE_SLOT_MARK = TESTU_HASHSIZE };
enum { TESTU_MAXN = 0xFFFF };

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
struct testu_dbj_data {
  int TESTU_HASH /* = TESTU_HASHSIZE - 1 */;
  int hasharr[TESTU_HASHSIZE];
  long data[TESTU_MAXN] /* = { 0L }*/;
};
/******************************************************************/

static void *produce_uniques(struct testu_dbj_data *ubench_fixture) {
  int count = 0;
  for (int i = 0; i < TESTU_MAXN; i++) {
    int key = hashKey(ubench_fixture->data[i]); // unbound hash key
    key &= TESTU_HASH;                          // bound it to hash index
    for (;;) {
      if (ubench_fixture->hasharr[key] == FREE_SLOT_MARK) { // slot is available
        ubench_fixture->hasharr[key] = i; // means the number appears first time
        break;
      } else if (ubench_fixture->data[hasharr[key]] ==
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

#if 0
UBENCH_F_SETUP(testu_dbj_data)
#else
static void ubench_f_setup_testu_dbj_data(struct testu_dbj_data *ubench_fixture)
#endif
{
  ubench_fixture->TESTU_HASH = TESTU_HASHSIZE - 1;
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
UBENCH_F_TEARDOWN(testu_dbj_data)
#else
static void
ubench_f_teardown_testu_dbj_data(struct testu_dbj_data *ubench_fixture)
#endif
{
  memset(ubench_fixture->data, 0L, __crt_countof(ubench_fixture->data));
  memset(ubench_fixture->hasharr, 0, __crt_countof(ubench_fixture->hasharr));
}

#if 0
UBENCH_F(testu_dbj_data, dbj_uniques_arr ) 
{
    UBENCH_F_SETUP(testu_dbj_data) ;
    produce_uniques( ubench_fixture ) ;
}

#else

extern struct ubench_state_s ubench_state;

static void ubench_f_setup_testu_dbj_data(struct testu_dbj_data *);

static void ubench_f_teardown_testu_dbj_data(struct testu_dbj_data *);

static void ubench_run_testu_dbj_data_dbj_uniques_arr(struct testu_dbj_data *);

/*
in here is the loop
*/
static void ubench_f_testu_dbj_data_dbj_uniques_arr
(ubench_int64_t *const ns, const ubench_int64_t size) 
{
  ubench_int64_t i = 0;
  struct testu_dbj_data fixture;
  memset(&fixture, 0, sizeof(fixture));
  /* setup is called from here -- once */
  ubench_f_setup_testu_dbj_data(&fixture);
  for (i = 0; i < size; i++) {
    ns[i] = ubench_ns();
    ubench_run_testu_dbj_data_dbj_uniques_arr(&fixture);
    ns[i] = ubench_ns() - ns[i];
  }
  ubench_f_teardown_testu_dbj_data(&fixture);
}

/*
this is the code for registering the benchmark
*/
static void __cdecl ubench_register_testu_dbj_data_dbj_uniques_arr(void);

__pragma(comment(linker, "/include:"
                         "ubench_register_testu_dbj_data_dbj_uniques_arr"
                         "_"));

__declspec(allocate(".CRT$XCU")) void(
    __cdecl *ubench_register_testu_dbj_data_dbj_uniques_arr_)(void) =
    ubench_register_testu_dbj_data_dbj_uniques_arr;

/*
this is the registration function itself
*/
static void __cdecl ubench_register_testu_dbj_data_dbj_uniques_arr(void) {
  const size_t index = ubench_state.benchmarks_length++;
  const char *name_part = "testu_dbj_data"
                          "."
                          "dbj_uniques_arr";
  const size_t name_size = strlen(name_part) + 1;
  char *name = ((char *)malloc(name_size));
  ubench_state.benchmarks = ((struct ubench_benchmark_state_s *)realloc(
      ((void *)ubench_state.benchmarks),
      sizeof(struct ubench_benchmark_state_s) *
          ubench_state.benchmarks_length));
  ubench_state.benchmarks[index].func =
      &ubench_f_testu_dbj_data_dbj_uniques_arr;
  ubench_state.benchmarks[index].name = name;
  _snprintf_s(name, name_size, name_size, "%s", name_part);
}

/*
this is called from the loop
*/
void ubench_run_testu_dbj_data_dbj_uniques_arr(
    struct testu_dbj_data *ubench_fixture) {
  // UBENCH_F_SETUP(testu_dbj_data) ;
  produce_uniques(ubench_fixture);
}

#endif // ! 0
