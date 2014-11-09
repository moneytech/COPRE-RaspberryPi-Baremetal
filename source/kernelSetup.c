/****************************************************
* kernelSetup.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m) test
****************************************************/

extern unsigned char _bss_start;
extern unsigned char _bss_end;

extern unsigned int allocated;

/*
* _Setup:
* Set up additional features before the main kernel
* starts. Includes:
* - Zero out .bss section
*/
void _Setup(void) {
	// Zero out .bss
	unsigned char* dst;
	dst = &_bss_start;
	while(dst < &_bss_end) {
		*dst++ = 0;
	}
}