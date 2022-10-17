#include "os.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_utils.h"
#include "startup.h"

volatile static uint64_t time = 0;
static void (*callback)(uint32_t) = NULL;

void SysTick_Handler() {
  time++;
  if (callback != NULL) callback(time);
}

void os_setcallback(void (*newcallback)(uint32_t)) {
  callback = newcallback;
}

void os_init() {
  time = 0;
  SystemCoreClockUpdate();
  LL_Init1msTick(SystemCoreClock);
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

uint64_t os_fulltime() {
  return time;
}

uint32_t os_time() {
  return (uint32_t)time;
}
