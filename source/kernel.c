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

#define ON 1
#define OFF 0

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

	DebugLog("Enabling DMA");
	EnableDMA();

	DebugLog("Testing MPI");

	if(TestMailboxPropertyInterface() == true) {
		DebugLog("Initialising V3D");

		if(InitV3D() == true) {

			DebugLog("Initialising Graphics");
			graphicsInitialised = InitGraphics(800, 600, 32);

			framesRendered = 0;

			// If the graphics don't initialise,
			// keep the ok LED on and branch to an infinite loop
			if(graphicsInitialised == 0) {
				OkLedOn();
				while(1) { };
			}

			while(1) {
				tick = GetTickCount();

				if(tick > frameCount + (1000000 / 60)) {
					UpdateGraphics();
					framesRendered++;
				}

				if(tick > keyboardCount + (1000000 / 20)) {
					ProcessKeyboardEvents();
				}

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
					framesRendered = 0;
				}
			}
		}
	}

	// Trap here as we have nothing to go to
	while(1) { }

	return 0;
}
