/* 
 * Basic start file example
 * Jan Swanepoel, 2019
 * 
*/

// Create references to symbols defined in the linker script 
extern unsigned int _data_start;	
extern unsigned int _data_end;
extern unsigned int _data_load;
extern unsigned int _bss_start;
extern unsigned int _bss_end;

void startup();			// Function prototype (forward declaration) for startup function
int main();			// Function prototype for main function

// Below we create an array of pointers which would form our vector table
// We use __attribute__ ((section(".vectors"))) to tell the compiler that we want the
// array to be placed in a memory section that we call ".vectors"
unsigned int * vectors[2] __attribute__ ((section(".vectors"))) = 
{
    (unsigned int *)	0x20010000,  	  // Address of top of stack. 20kB = 1024 x 20 = 20480 bytes = 0x5000 
    (unsigned int *)  startup     	  // Address of the reset handler which is also our startup function
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

	// Calling the main function
	main();
	
	while(1);	// Normally main() should never return, but just incase we loop infinitely
}
