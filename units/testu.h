// (c) dbj.org -- DBJ_LICENSE -- https://dbj.org/dbj_license 
#include "comonu.h"
#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
/******************************************************************/
// https://codingforspeed.com/c-coding-example-using-hash-algorithm-to-remove-duplicate-number-by-on/

enum { HASHSIZE = 1048576 };  // 20MB
static const int HASH = HASHSIZE - 1;
static int hasharr[HASHSIZE];

// the data numbers can't be the value of HASHSIZE, 
enum { FREE_SLOT_MARK = HASHSIZE } ;
enum { MAXN = 0xFFFF } ;

#ifndef _WIN64
static int hashKey(long x) {
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}
#else // _WIN64
uint64_t hashKey(uint64_t x) {
    x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
    x = x ^ (x >> 31);
    return x;
}
#endif // _WIN64
/******************************************************************/
/** remove duplicates from an array                              **/
/******************************************************************/
UBENCH( dbj, fast_arr_of_uniques ) {

static long data[MAXN] = { 0L } ;

// make random data
srand (time(NULL));
for (int i = 0; i < MAXN; i ++) data[i] = rand() % MAXN;

// we init the hash table by using the special value
for (int i = 0; i < HASHSIZE; i ++) hasharr[i] = FREE_SLOT_MARK;

// And, the following O(n) algorithm will try each number and check if 
// it appears in the table first (almost O(1) lookup).
int count = 0;
for (int i = 0; i < MAXN; i ++) {
        int key = hashKey(data[i]); // unbound hash key
        key &= HASH;  // bound it to hash index
        for (;;) {
                if (hasharr[key] == FREE_SLOT_MARK) { // slot is available
                    hasharr[key] = i;  // means the number appears first time
                    break;
                }
                else if (data[hasharr[key]] == data[i]) { // same number
                    count ++;
                    break;
                } else {
                    key = (key + 1) & HASH; // different number, collision happens
                }
        }
    }

    // uniques are in hasharr
#if 0
// The next piece of code loops the hash table and print out the unique numbers.
printf("\n %s -- %d numbers removed.\n", __FUNCSIG__, count );

 printf("\n{ ");
    for (int i = 0; i < HASHSIZE; i ++) {
        if (hasharr[i] != HASHSIZE) {
            printf("%d ", data[hasharr[i]] );
        }
    }
 printf("}\n");
#endif // 0
}
/******************************************************************/
// enum { HASHSIZE = 1048576 };  // 20MB
struct dbj_fixture {
int HASH /* = HASHSIZE - 1 */;
int hasharr[HASHSIZE];
long data[MAXN] /* = { 0L }*/ ;
};

UBENCH_F_SETUP(dbj_fixture) 
{
ubench_fixture->HASH = HASHSIZE - 1;
// make random data
srand (time(NULL));
for (int i = 0; i < MAXN; i ++) ubench_fixture->data[i] = rand() % MAXN;
// init the hash table by using the special value
for (int i = 0; i < HASHSIZE; i ++) ubench_fixture->hasharr[i] = FREE_SLOT_MARK;

  printf("\nLeaving %s", __FUNCSIG__) ;
}

UBENCH_F_TEARDOWN(dbj_fixture) { /* nothing to do */ }

static void * produce_uniques ( struct dbj_fixture * ubench_fixture)  {
int count = 0;
for (int i = 0; i < MAXN; i ++) {
        int key = hashKey(ubench_fixture->data[i]); // unbound hash key
        key &= HASH;  // bound it to hash index
        for (;;) {
                if (ubench_fixture->hasharr[key] == FREE_SLOT_MARK) { // slot is available
                    ubench_fixture->hasharr[key] = i;  // means the number appears first time
                    break;
                }
                else if (ubench_fixture->data[hasharr[key]] == ubench_fixture->data[i]) { // same number
                    count ++;
                    break;
                } else {
                    key = (key + 1) & HASH; // different number, collision happens
                }
        }
    }

    return ubench_fixture;
}

UBENCH_F(dbj_fixture, dbj_uniques_arr ) 
{
     UBENCH_DO_NOTHING(0);
int count = 0;
for (int i = 0; i < MAXN; i ++) {
        int key = hashKey(ubench_fixture->data[i]); // unbound hash key
        key &= HASH;  // bound it to hash index
        for (;;) {
                if (ubench_fixture->hasharr[key] == FREE_SLOT_MARK) { // slot is available
                    ubench_fixture->hasharr[key] = i;  // means the number appears first time
                    break;
                }
                else if (ubench_fixture->data[hasharr[key]] == ubench_fixture->data[i]) { // same number
                    count ++;
                    break;
                } else {
                    key = (key + 1) & HASH; // different number, collision happens
                }
        }
    }
}

#ifdef __cplusplus
} // extern "C" {
#endif // __cplusplus