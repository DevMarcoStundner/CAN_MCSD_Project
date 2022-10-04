#include "stm32l432xx.h"
const int a = 7;		// Will be placed in rodata
int b = 3;			// Will be placed in data (LMA)
int c = 0;			// Will be placed in bss
int d;				// Will be placed in bss (but first in COMMON)

int main()
{	
	c = a + b;
	
	while (1);
	
	return 0;
}
