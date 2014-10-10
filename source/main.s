/****************************************************
* main.s
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

/*
* Use the _start label (the ASM entry point)
*/
.section .init
.globl _start
_start:

/* 
* This command loads the physical address of the GPIO region into r0.
*/
ldr r0,=0x20200000

/*
* Set the GPIO function select for pin 16
* 24 bytes for GPIO controller
*	Each 4 bytes relate to 10 GPIO pins
*	54 GPIO pins, therefore 6 sets of 4 bytes (24 bytes)
*		Every 3 bytes relates to a particular GPIO pin
*	16th pin is within pins 10-19, we need the 6th set of 3 bits
*		This is where the number 18 comes from (below, 6 * 3)
*	Use a 4 byte offset (below) as pin 16 is in the second set of pins
*/
mov r1,#1
lsl r1,#18

str r1,[r0,#4]

/*
* Initialise the USB Driver
*/

mov sp, #0x8000
bl UsbInitialise
bl KeyboardInit

/*
* Move to the C Entry Point
*/
bl Entrypoint

/*
* Return System clock count
*/
.globl GetTickCount
GetTickCount:
	ldr r0, =0x20003004
	ldr r0, [r0]
	bx lr
