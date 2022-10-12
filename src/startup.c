/* 
 * Basic start file example
 * Jan Swanepoel, 2019
 * 
*/

#include <stdint.h>
#include "startup.h"
#include "libcmsis/system_stm32l4xx.h"
#include "libll/stm32l4xx_ll_utils.h"
#include "os/os.h"

// Create references to symbols defined in the linker script 
extern unsigned int _data_start;	
extern unsigned int _data_end;
extern unsigned int _data_load;
extern unsigned int _bss_start;
extern unsigned int _bss_end;

void startup();			// Function prototype (forward declaration) for startup function
int main();			// Function prototype for main function
void faultHandler();

// Below we create an array of pointers which would form our vector table
// We use __attribute__ ((section(".vectors"))) to tell the compiler that we want the
// array to be placed in a memory section that we call ".vectors"
unsigned int * vectors[] __attribute__ ((section(".vectors"))) = 
{
    (unsigned int *)	0x20010000,  	  // Address of top of stack. 20kB = 1024 x 20 = 20480 bytes = 0x5000 
    (unsigned int *)  startup,    	  // Address of the reset handler which is also our startup function
    (unsigned int *)  NMI_Handler,   // NMI interrupt
    (unsigned int *)  HardFault_Handler,   // Hard Fault interrupt
	  (unsigned int *) 	MemManage_Handler,
	  (unsigned int *) 	BusFault_Handler,
	  (unsigned int *) 	UsageFault_Handler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	SVC_Handler,
	  (unsigned int *) 	DebugMon_Handler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	PendSV_Handler,
	  (unsigned int *) 	SysTick_Handler,
	  (unsigned int *) 	WWDG_IRQHandler,
	  (unsigned int *) 	PVD_PVM_IRQHandler,
	  (unsigned int *) 	TAMP_STAMP_IRQHandler,
	  (unsigned int *) 	RTC_WKUP_IRQHandler,
	  (unsigned int *) 	FLASH_IRQHandler,
	  (unsigned int *) 	RCC_IRQHandler,
	  (unsigned int *) 	EXTI0_IRQHandler,
	  (unsigned int *) 	EXTI1_IRQHandler,
	  (unsigned int *) 	EXTI2_IRQHandler,
	  (unsigned int *) 	EXTI3_IRQHandler,
	  (unsigned int *) 	EXTI4_IRQHandler,
	  (unsigned int *) 	DMA1_Channel1_IRQHandler,
	  (unsigned int *) 	DMA1_Channel2_IRQHandler,
	  (unsigned int *) 	DMA1_Channel3_IRQHandler,
	  (unsigned int *) 	DMA1_Channel4_IRQHandler,
	  (unsigned int *) 	DMA1_Channel5_IRQHandler,
	  (unsigned int *) 	DMA1_Channel6_IRQHandler,
	  (unsigned int *) 	DMA1_Channel7_IRQHandler,
	  (unsigned int *) 	ADC1_IRQHandler,
	  (unsigned int *) 	CAN1_TX_IRQHandler,
	  (unsigned int *) 	CAN1_RX0_IRQHandler,
	  (unsigned int *) 	CAN1_RX1_IRQHandler,
	  (unsigned int *) 	CAN1_SCE_IRQHandler,
	  (unsigned int *) 	EXTI9_5_IRQHandler,
	  (unsigned int *) 	TIM1_BRK_TIM15_IRQHandler,
	  (unsigned int *) 	TIM1_UP_TIM16_IRQHandler,
	  (unsigned int *) 	TIM1_TRG_COM_IRQHandler,
	  (unsigned int *) 	TIM1_CC_IRQHandler,
	  (unsigned int *) 	TIM2_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	I2C1_EV_IRQHandler,
	  (unsigned int *) 	I2C1_ER_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	SPI1_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	USART1_IRQHandler,
	  (unsigned int *) 	USART2_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	EXTI15_10_IRQHandler,
	  (unsigned int *) 	RTC_Alarm_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	SPI3_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	TIM6_DAC_IRQHandler,
	  (unsigned int *) 	TIM7_IRQHandler,
	  (unsigned int *) 	DMA2_Channel1_IRQHandler,
	  (unsigned int *) 	DMA2_Channel2_IRQHandler,
	  (unsigned int *) 	DMA2_Channel3_IRQHandler,
	  (unsigned int *) 	DMA2_Channel4_IRQHandler,
	  (unsigned int *) 	DMA2_Channel5_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	COMP_IRQHandler,
	  (unsigned int *) 	LPTIM1_IRQHandler,
	  (unsigned int *) 	LPTIM2_IRQHandler,
	  (unsigned int *) 	USB_IRQHandler,
	  (unsigned int *) 	DMA2_Channel6_IRQHandler,
	  (unsigned int *) 	DMA2_Channel7_IRQHandler,
	  (unsigned int *) 	LPUART1_IRQHandler,
	  (unsigned int *) 	QUADSPI_IRQHandler,
	  (unsigned int *) 	I2C3_EV_IRQHandler,
	  (unsigned int *) 	I2C3_ER_IRQHandler,
	  (unsigned int *) 	SAI1_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	SWPMI1_IRQHandler,
	  (unsigned int *) 	TSC_IRQHandler,
	  (unsigned int *) 	0,
	  (unsigned int *) 	0,
	  (unsigned int *) 	RNG_IRQHandler,
	  (unsigned int *) 	FPU_IRQHandler,
	  (unsigned int *) 	CRS_IRQHandler
};

// The startup function, address was provided in the vector table	
void startup()
{
	volatile unsigned int *src, *dest;

	// Copy data section values from load time memory address (LMA) to their address in SRAM 
	for (src = &_data_load, dest = &_data_start; dest < &_data_end; src++, dest++) 
		*dest = *src;
	
	// Initialize all uninitialized variables (bss section) to 0
	for (dest = &_bss_start; dest < &_bss_end; dest++)
		*dest = 0;

  SystemCoreClockUpdate();
  os_inittime();

	// Calling the main function
	main();
	
	while(1);	// Normally main() should never return, but just incase we loop infinitely
}

static void defaultIntHandler() {
  while(1);
}

static void faultIntHandler() {
  while(1);
}

