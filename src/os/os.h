#ifndef OS_H
#define OS_H

#include <stdint.h>
#include <stddef.h>
#include "startup.h"

#define OS_MAX_TIMERS 10

typedef struct {
  uint32_t settime;
  void (*callback)(uint32_t);
} os_timer_TypeDef;

void os_init();
void os_setcallback(void (*)(uint32_t));
uint8_t os_timeout(uint64_t timeout_ns, void(*callback)());
int8_t os_setalarm(uint64_t altime, void (*callback)(uint32_t));

uint64_t os_fulltime();
uint32_t os_time();

#endif
