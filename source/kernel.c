/****************************************************
* kernel.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m) test
****************************************************/

#include "../include/csud/platform.h"
#include "../include/keyboard.h"
#include "../include/graphics.h"
#include "../include/debug.h"
#include "../include/types.h"

#define ON 1
#define OFF 0

extern void OkLedOn(void);
extern void OkLedOff(void);

extern unsigned int GetTickCount(void);
extern void UsbCheckForChange(void);

unsigned int renderDebugger;

void TestPrint(void) {
	DebugLog("Key pressed!");
}

/*
* Entrypoint:
* Main entry point for the kernel. This function is
* called after previous parts of the kernel have
* been set up.
*/
int Entrypoint(void) {
	unsigned int ledState, ledCount, graphicsInitialised, frameCount, tick;
	int framesRendered;

	renderDebugger = ON;
	ledState = OFF;
	ledCount = frameCount = GetTickCount();

	graphicsInitialised = InitGraphics(800, 600, 32);

	framesRendered = 0;

	// Example Key Binding
	BindKey('a', TestPrint);

	while(1) {
		tick = GetTickCount();

		//if(graphicsInitialised == 1) {
			if(tick > frameCount + (1000000 / 60)) {
				UpdateGraphics();
				framesRendered++;
				frameCount = tick;
			}
		//}

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

			LOGF("FPS: %i", framesRendered);
			framesRendered = 0;

			ProcessKeyboardEvents();
		}
	}

	return 0;
}
