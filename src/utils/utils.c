#include "utils.h"

void UT_byteset(uint8_t *dest, char ch, uint32_t count) {
  for (;count;count--) {
    dest[count] = ch;
  }
}
