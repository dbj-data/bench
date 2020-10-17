#pragma once

#include "comonu.h"

// Only enable the test if we aren't using Visual Studio, or we're using Visual
// Studio 2019.
#if !defined(_MSC_VER) || (_MSC_VER >= 1920)
#include <chrono>
#include <thread>

UBENCH(cpp11, tenth_of_a_second) {
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}
#endif

UBENCH(cpp11, do_nothing) {
  static char a[8 * 1024 * 1024];
  static char b[8 * 1024 * 1024];
  UBENCH_DO_NOTHING(a);
  memcpy(b, a, sizeof(a));
  UBENCH_DO_NOTHING(b);
}

struct cpp11_my_fixture {
  char *data;
};

UBENCH_F_SETUP(cpp11_my_fixture) {
  const int size = 128 * 1024 * 1024;
  ubench_fixture->data = static_cast<char *>(malloc(size));
  memset(ubench_fixture->data, ' ', size - 2);
  ubench_fixture->data[size - 1] = '\0';
  ubench_fixture->data[size / 2] = 'f';
}

UBENCH_F_TEARDOWN(cpp11_my_fixture) { free(ubench_fixture->data); }

UBENCH_F(cpp11_my_fixture, strchr) {
  UBENCH_DO_NOTHING(strchr(ubench_fixture->data, 'f'));
}

UBENCH_F(cpp11_my_fixture, strrchr) {
  UBENCH_DO_NOTHING(strrchr(ubench_fixture->data, 'f'));
}
