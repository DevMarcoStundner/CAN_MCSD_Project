#include "os.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_utils.h"
#include "libll/stm32l4xx_ll_rcc.h"
#include "libll/stm32l4xx_ll_system.h"
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
  // Enable and check that HSI is running
  LL_RCC_HSI_Enable();
  while(!LL_RCC_HSI_IsReady());
  // 16MHz up to 320MHz and down to 80MHz for sysclk
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_4, 80, LL_RCC_PLLR_DIV_4);
  LL_RCC_PLL_Enable();
  // optional: add timeout error to pllrdy
  while(!LL_RCC_PLL_IsReady());
  // Enable R output to sysclk mux
  LL_RCC_PLL_EnableDomain_SYS();
  // config axb bus presc for 1
  // update and check flash wait states
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);
  while(LL_FLASH_LATENCY_4 != LL_FLASH_GetLatency());
  // enable and check that pll is system clock source
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
  while(LL_RCC_SYS_CLKSOURCE_STATUS_PLL != LL_RCC_GetSysClkSource());
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
