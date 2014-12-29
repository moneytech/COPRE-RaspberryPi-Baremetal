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
* uart.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/platform.h"

#define GPFSEL1 0x20200004
#define GPSET0 0x2020001C
#define GPCLR0 0x20200028
#define GPPUD 0x20200094
#define GPPUDCLK0 0x20200098

#define AUX_ENABLES 0x20215004
#define AUX_MU_IO_REG 0x20215040
#define AUX_MU_IER_REG 0x20215044
#define AUX_MU_IIR_REG 0x20215048
#define AUX_MU_LCR_REG 0x2021504C
#define AUX_MU_MCR_REG 0x20215050
#define AUX_MU_LSR_REG 0x20215054
#define AUX_MU_MSR_REG 0x20215058
#define AUX_MU_SCRATCH 0x2021505C
#define AUX_MU_CNTL_REG 0x20215060
#define AUX_MU_STAT_REG 0x20215064
#define AUX_MU_BAUD_REG 0x20215068

#define IRQ_BASIC 0x2000B200
#define IRQ_PEND1 0x2000B204
#define IRQ_PEND2 0x2000B208
#define IRQ_FIQ_CONTROL 0x2000B210
#define IRQ_ENABLE1 0x2000B210
#define IRQ_ENABLE2 0x2000B214
#define IRQ_ENABLE_BASIC 0x2000B218
#define IRQ_DISABLE1 0x2000B21C
#define IRQ_DISABLE2 0x2000B220
#define IRQ_DISABLE_BASIC 0x2000B224

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

char uartEnabled = 0;

/*
* InitUART:
* Enable UART pins, based on dwelch's uart04 implementation
* github.com/dwelch67/raspberrypi/tree/master/uart04
*/
void InitUART(void) {
	unsigned int ra;

	PutUInt32(AUX_ENABLES, 1);
	PutUInt32(AUX_MU_IER_REG, 0);
	PutUInt32(AUX_MU_CNTL_REG, 0);
	PutUInt32(AUX_MU_LCR_REG, 3);
	PutUInt32(AUX_MU_MCR_REG, 0);
	PutUInt32(AUX_MU_IER_REG, 0x5); // enable rx interrupts
	PutUInt32(AUX_MU_IIR_REG, 0xC6);
	PutUInt32(AUX_MU_BAUD_REG, 270);

	ra = GetUInt32(GPFSEL1);
	ra &= ~(7 << 12); // gpio14
	ra |= 2 << 12; // alt5
	ra &= ~(7 << 15); // gpio15
	ra |= 2 << 15; // alt5

	PutUInt32(GPFSEL1, ra);
	PutUInt32(GPPUD, 0);

	PutUInt32(GPPUDCLK0, (1 << 14) | (1 << 15));

	PutUInt32(GPPUDCLK0, 0);
	PutUInt32(AUX_MU_CNTL_REG, 3);

	uartEnabled = 1;
}

/*
* UARTPutC:
* Write a character to the output UART pin.
*/
void UARTPutC(unsigned int c) {
	if(uartEnabled == 1) {
		while(1) {
			if(GetUInt32(AUX_MU_LSR_REG) & 0x20) {
				break;
			}
		}

		PutUInt32(AUX_MU_IO_REG, c);
	}
}

/*
* UARTPutS:
* Write a string to the UART output pin.
*/
void UARTPutS(char* text) {
	if(uartEnabled == 1 && text != 0) {
		int counter = 0;

		while(text[counter] != 0) {
			UARTPutC(text[counter]);
			counter++;
		}
	}
}