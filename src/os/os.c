#include "os.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_utils.h"
#include "startup.h"

static uint64_t time = 0;

void os_inittime() {
  time = 0;
  LL_Init1msTick(SystemCoreClock);
}

uint64_t os_fulltime() {
  return 0;
}

uint32_t os_time() {
  return 0;
}
