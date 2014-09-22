/****************************************************
* led.s
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/
.section .led
/*
* Turn the OK LED on
*/
.globl OkLedOn
OkLedOn:
	push { r4 - r5 }
	ldr r5,=0x20200000
	/* 
	* Set the 16th bit of r1. The 16th bit is
	* for the 16th pin (the ACT/OK LED)
	*/
	mov r4,#1
	lsl r4,#16

	/* 
	* Low: 40-48
	* High: 28-36
	* Set GPIO 16 to low, causing the LED to turn on.
	*/
	str r4,[r5,#40]
	pop { r4 - r5 }
	bx lr

/*
* Turn the OK LED off
*/
.globl OkLedOff
OkLedOff:
	push { r4 }
	ldr r5,=0x20200000
	mov r4,#1
	lsl r4,#16
	str r4,[r5, #28]
	pop { r4 }
	bx lr
