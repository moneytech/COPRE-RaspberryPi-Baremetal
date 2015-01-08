/*
The MIT License (MIT)

Copyright (c) 2014 Niall Frederick Weedon, Timothy Stanley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

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

bl _Setup
bl InitDebug

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
