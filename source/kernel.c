/****************************************************
* kernel.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m) test
****************************************************/

#include "keyboard.h"
#include "graphics.h"

#define ON 1
#define OFF 0

extern void OkLedOn(void);
extern void OkLedOff(void);

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern unsigned int GetTickCount(void);

int Entrypoint(void) {
	unsigned int ledState, ledCount, graphicsInitialised, frameCount;
	ledState = OFF;
	ledCount = frameCount = GetTickCount();

	graphicsInitialised = InitGraphics(800, 600, 32);

	// Example Key Binding
	BindKey('a', OkLedOn);

	if (graphicsInitialised == 1) {
		UpdateGraphics();
	}

	while(1) {
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
		
		ProcessKeyboardEvents();
	}

	return 0;
}
