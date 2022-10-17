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
void defaultIntHandler(void);
void faultIntHandler(void);

void NMI_Handler(void) __attribute ((weak, alias("faultIntHandler")));
void HardFault_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void MemManage_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void BusFault_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void UsageFault_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void SVC_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void DebugMon_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void PendSV_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void SysTick_Handler(void) __attribute ((weak, alias("defaultIntHandler")));
void WWDG_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void PVD_PVM_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TAMP_STAMP_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void RTC_WKUP_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void FLASH_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void RCC_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI0_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI3_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI4_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel3_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel4_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel5_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel6_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA1_Channel7_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void ADC1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void CAN1_TX_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void CAN1_RX0_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void CAN1_RX1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void CAN1_SCE_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI9_5_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM1_BRK_TIM15_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM1_UP_TIM16_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM1_TRG_COM_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM1_CC_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void I2C1_EV_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void I2C1_ER_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void SPI1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void USART1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void USART2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void EXTI15_10_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void RTC_Alarm_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void SPI3_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM6_DAC_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TIM7_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel3_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel4_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel5_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void COMP_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void LPTIM1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void LPTIM2_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void USB_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel6_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void DMA2_Channel7_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void LPUART1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void QUADSPI_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void I2C3_EV_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void I2C3_ER_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void SAI1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void SWPMI1_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void TSC_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void RNG_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void FPU_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));
void CRS_IRQHandler(void) __attribute ((weak, alias("defaultIntHandler")));

// Below we create an array of pointers which would form our vector table
// We use __attribute__ ((section(".vectors"))) to tell the compiler that we want the
// array to be placed in a memory section that we call ".vectors"
void (*vectors[])(void) __attribute__ ((section(".vectors"))) = 
{
    (void (*)(void))	0x20010000,  	  // Address of top of stack. 20kB = 1024 x 20 = 20480 bytes = 0x5000 
    (void (*)(void))  startup,    	  // Address of the reset handler which is also our startup function
    (void (*)(void))  NMI_Handler,   // NMI interrupt
    (void (*)(void))  HardFault_Handler,   // Hard Fault interrupt
	  (void (*)(void)) 	MemManage_Handler,
	  (void (*)(void)) 	BusFault_Handler,
	  (void (*)(void)) 	UsageFault_Handler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	SVC_Handler,
	  (void (*)(void)) 	DebugMon_Handler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	PendSV_Handler,
	  (void (*)(void)) 	SysTick_Handler,
	  (void (*)(void)) 	WWDG_IRQHandler,
	  (void (*)(void)) 	PVD_PVM_IRQHandler,
	  (void (*)(void)) 	TAMP_STAMP_IRQHandler,
	  (void (*)(void)) 	RTC_WKUP_IRQHandler,
	  (void (*)(void)) 	FLASH_IRQHandler,
	  (void (*)(void)) 	RCC_IRQHandler,
	  (void (*)(void)) 	EXTI0_IRQHandler,
	  (void (*)(void)) 	EXTI1_IRQHandler,
	  (void (*)(void)) 	EXTI2_IRQHandler,
	  (void (*)(void)) 	EXTI3_IRQHandler,
	  (void (*)(void)) 	EXTI4_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel1_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel2_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel3_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel4_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel5_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel6_IRQHandler,
	  (void (*)(void)) 	DMA1_Channel7_IRQHandler,
	  (void (*)(void)) 	ADC1_IRQHandler,
	  (void (*)(void)) 	CAN1_TX_IRQHandler,
	  (void (*)(void)) 	CAN1_RX0_IRQHandler,
	  (void (*)(void)) 	CAN1_RX1_IRQHandler,
	  (void (*)(void)) 	CAN1_SCE_IRQHandler,
	  (void (*)(void)) 	EXTI9_5_IRQHandler,
	  (void (*)(void)) 	TIM1_BRK_TIM15_IRQHandler,
	  (void (*)(void)) 	TIM1_UP_TIM16_IRQHandler,
	  (void (*)(void)) 	TIM1_TRG_COM_IRQHandler,
	  (void (*)(void)) 	TIM1_CC_IRQHandler,
	  (void (*)(void)) 	TIM2_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	I2C1_EV_IRQHandler,
	  (void (*)(void)) 	I2C1_ER_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	SPI1_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	USART1_IRQHandler,
	  (void (*)(void)) 	USART2_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	EXTI15_10_IRQHandler,
	  (void (*)(void)) 	RTC_Alarm_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	SPI3_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	TIM6_DAC_IRQHandler,
	  (void (*)(void)) 	TIM7_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel1_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel2_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel3_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel4_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel5_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	COMP_IRQHandler,
	  (void (*)(void)) 	LPTIM1_IRQHandler,
	  (void (*)(void)) 	LPTIM2_IRQHandler,
	  (void (*)(void)) 	USB_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel6_IRQHandler,
	  (void (*)(void)) 	DMA2_Channel7_IRQHandler,
	  (void (*)(void)) 	LPUART1_IRQHandler,
	  (void (*)(void)) 	QUADSPI_IRQHandler,
	  (void (*)(void)) 	I2C3_EV_IRQHandler,
	  (void (*)(void)) 	I2C3_ER_IRQHandler,
	  (void (*)(void)) 	SAI1_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	SWPMI1_IRQHandler,
	  (void (*)(void)) 	TSC_IRQHandler,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	0,
	  (void (*)(void)) 	RNG_IRQHandler,
	  (void (*)(void)) 	FPU_IRQHandler,
	  (void (*)(void)) 	CRS_IRQHandler
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

  os_init();

	// Calling the main function
	main();
	
	while(1);	// Normally main() should never return, but just incase we loop infinitely
}

void defaultIntHandler() {
  while(1);
}

void faultIntHandler() {
  while(1);
}
