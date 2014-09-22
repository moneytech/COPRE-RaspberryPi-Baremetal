/****************************************************
* mem.s
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/
.section .mem

.globl PutUInt32
PutUInt32:
	str r1, [r0]
	bx lr

.globl GetUInt32
GetUInt32:
	ldr r0, [r0]
	bx lr
