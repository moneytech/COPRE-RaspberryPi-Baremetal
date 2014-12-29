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
* kernel.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m) test
****************************************************/

#include "../include/csud/platform.h"
#include "../include/graphics.h"
#include "../include/keyboard.h"
#include "../include/debug.h"
#include "../include/v3d-qpu.h"
#include "../include/mailbox.h"
#include "../include/uart.h"

#define ON 1
#define OFF 0

/*
* Data memory barrier
* No memory access after the DMB can run until all memory accesses before it
* have completed
*/
#define MEMORY_BARRIER() asm volatile \
	("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0) )

extern void OkLedOn(void);
extern void OkLedOff(void);
extern unsigned int GetTickCount(void);
extern void EnableDMA(void);

/*
* Entrypoint:
* Main entry point for the kernel. This function is
* called after previous parts of the kernel have
* been set up.
*/
int Entrypoint(void) {
	unsigned int ledState, ledCount, graphicsInitialised, frameCount, tick, keyboardCount;
	int framesRendered;

	ledState = OFF;
	ledCount = frameCount = keyboardCount = GetTickCount();

	DebugLog("Enabling UART");
	InitUART();

	//UARTPutC(65);

	DebugLog("Enabling DMA");
	EnableDMA();

	DebugLog("Testing MPI");

	if(TestMailboxPropertyInterface() == true) {
		DebugLog("Initialising V3D");

		if(InitV3D() == true) {

			DebugLog("Initialising Graphics");
			graphicsInitialised = InitGraphics(1920, 1080, 32);

			framesRendered = 0;

			// If the graphics don't initialise,
			// keep the ok LED on and branch to an infinite loop
			if(graphicsInitialised == 0) {
				OkLedOn();
				while(1) { };
			}

			int totalSeconds = 0;
			bool printedReport = false;

			while(1) {
				MEMORY_BARRIER();
				tick = GetTickCount();
				MEMORY_BARRIER();

				// Framerate limiting doesn't work
				if(totalSeconds < 10 && tick > frameCount + (1000000 / 6000)) {
					UpdateGraphics();
					framesRendered++;
				} else if(totalSeconds >= 10 && !printedReport) {
					DebugLog("Benchmark Complete @ 1920*1080");
					LOGF("%d frames in 10 seconds", framesRendered);
					RenderDebugLog();
					SwapBuffers();
					printedReport = true;
				}

				// Keyboard processing is really slow
				/*if(tick > keyboardCount + (1000000 / 20)) {
					ProcessKeyboardEvents();
				}*/

				// Switch state of OK/ACT LED every 1 second
				if(tick > ledCount + 1000000) {
					if(ledState == ON) {
						OkLedOff();
						ledState = OFF;
					} else {
						OkLedOn();
						ledState = ON;
					}
					ledCount = tick;

					//LOGF("FPS: %i", framesRendered);
					//framesRendered = 0;
					totalSeconds++;
				}
			}
		}
	}

	// Trap here as we have nothing to go to
	while(1) { }

	return 0;
}
