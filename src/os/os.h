#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stddef.h>
#include "startup.h"

void os_init();
void os_setcallback(void (*)(uint32_t));

uint64_t os_fulltime();
uint32_t os_time();

#endif
