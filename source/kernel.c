/****************************************************
* kernel.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m) test
****************************************************/

#include "../include/csud/platform.h"
#include "keyboard.h"
#include "graphics.h"
#include "debug.h"

#define ON 1
#define OFF 0

extern void OkLedOn(void);
extern void OkLedOff(void);

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern unsigned int GetTickCount(void);
extern void UsbCheckForChange(void);

extern unsigned char _bss_start;
extern unsigned char _bss_end;

extern unsigned int allocated;

unsigned int renderDebugger;

void ToggleDebugger(void) {
	if(renderDebugger == ON) {
		renderDebugger = OFF;
	} else {
		renderDebugger = ON;
	}
}

void TestPrint(void) {
	DebugLog("Key pressed!");
}

void _Setup(void) {
	// Zero out .bss
	unsigned char* dst;
	dst = &_bss_start;
	while(dst < &_bss_end) {
		*dst++ = 0;
	}
}

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
