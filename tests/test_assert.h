#pragma once

#include <stdio.h>
#include <stdio.h>

#define TEST_ASSERT(x) { \
  if (!((x))) { \
    fflush(stdout); \
    fprintf(stderr, "FAILED: [%s:%d] %s\n", __FILE__, __LINE__, #x); \
    fflush(stderr); \
    exit(1); \
  } \
}
