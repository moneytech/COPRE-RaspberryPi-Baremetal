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

	//allocated = 0;
}

int Entrypoint(void) {
	unsigned int ledState, ledCount, graphicsInitialised, frameCount;
	renderDebugger = ON;
	ledState = OFF;
	ledCount = frameCount = GetTickCount();

	graphicsInitialised = InitGraphics(800, 600, 32);
	//InitDebug();
	/*DebugLog("This is a test of the debug log");
	DebugLog("It should be a good test of the debug");
	DebugLog("DEBUG!");*/

	// Example Key Binding
	BindKey('a', TestPrint);

	while(1) {
		// Limit to 60 FPS
		if(GetTickCount() > frameCount + (1000000 / 60)) {
			if(graphicsInitialised == 1) {
				UpdateGraphics();
				
				if(renderDebugger == ON) {
					RenderDebugLog();
				}

				frameCount = GetTickCount();		
			}
		}

		ProcessKeyboardEvents();

		// Switch state of OK/ACT LED every 1 second
		if(GetTickCount() > ledCount + 1000000) {
			if(ledState == ON) {
				OkLedOff();
				ledState = OFF;
			} else {
				OkLedOn();
				ledState = ON;
			}
			ledCount = GetTickCount();
		}
	}

	return 0;
}
