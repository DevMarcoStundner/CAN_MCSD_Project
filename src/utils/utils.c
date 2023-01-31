#include "utils.h"
#include <math.h>

/**
 * @brief does the same as memset
 * @note necessary as we dont use a libc
 * @note the compiler attributes are there to stop gcc from linking this as memset even though memset does not exist
 */
void __attribute__((optimize("-fno-tree-loop-distribute-patterns"))) UT_byteset(uint8_t *dest, uint8_t ch, uint32_t count) {
  for (;count;count--) {
    dest[count] = ch;
  }
}

bool ut_fequal(float a, float b) {
  a = fabsf(a);
  b = fabsf(b);
  const float tolerance = 1e-4;
  return (fabsf(a-b) < tolerance);
}
