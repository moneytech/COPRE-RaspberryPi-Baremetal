/****************************************************
* imageData.s
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

.section .data
.align 4

.globl imageSplash
imageSplash:
	.incbin "bin/splash.png.bin"
