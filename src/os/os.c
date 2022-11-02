#include "os.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_utils.h"
#include "startup.h"

static volatile uint64_t time = 0;
static void (*callback)(uint32_t) = NULL;
static os_timer_TypeDef active_timers[OS_MAX_TIMERS] = {0};

/**
 * @brief Interrrupt handler for Systick Interrupt
 */
void SysTick_Handler() {
  time++;
  if (callback != NULL) callback(time);
}

/**
 * @brief set callback for os tick
 * @param newcallback is a function pointer which takes one uint32 value
 */
void os_setcallback(void (*newcallback)(uint32_t)) {
  callback = newcallback;
}

/**
 * @brief Initialization of the os component
 * calculates current system clock, configures the systick for 1ms and enables it
 * @note This is only called via startup.c
 */
void os_init() {
  time = 0;
  SystemCoreClockUpdate();
  LL_Init1msTick(SystemCoreClock);
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
}

/**
 * @brief gets the full 64 bit time value
 * @note the current build configuration does not allow more than addition with 64 bit values, so be careful
 */
uint64_t os_fulltime() {
  return time;
}

/**
 * @brief gets the first 32 bits from the full time value
 * @note this is less overflow safe than 64 bit time
 */
uint32_t os_time() {
  return (uint32_t)time;
}

/**
 * @brief add tiemr with callback
 * @param altime is the tick count the callback will be called
 * @param callback is a function pointer which takes one uint32
 * @note a maximum of OS_MAX_TIMERS timers may be used
 * @retval a 8 bit value containing the timer id, returns -1 if no space was available
 */
int8_t os_setalarm(uint64_t altime, void (*callback)(uint32_t)) {
  for (int i=0; i<OS_MAX_TIMERS; i++) {
    os_timer_TypeDef* timslot = active_timers+i;
    if (timslot->callback == NULL) {
      timslot->callback = callback;
      timslot->settime = altime;
      return i;
    }
  }
  return -1;
}
