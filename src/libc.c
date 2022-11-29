#include <stddef.h>
#include "startup.h"

extern int __bss_end__;

void _exit(int excode __attribute__((unused))) {
  faultIntHandler();
}

void *_sbrk(int incr) {
  static unsigned char *heap = NULL;
  unsigned char *prev_heap;

  if (heap == NULL) {
    heap = (unsigned char *)&__bss_end__;
  }
  prev_heap = heap;

  heap += incr;

  return prev_heap;
}
