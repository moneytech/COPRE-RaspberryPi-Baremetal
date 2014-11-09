/****************************************************
* keyboard.c
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/platform.h"
#include "../include/csud/types.h"
#include "../include/types.h"
#include "../include/debug.h"

#define MAX_KEYS 26

/* External links to CSUD functions */
extern void UsbCheckForChange(void);
extern unsigned int KeyboardCount(void);
extern unsigned int KeyboardGetAddress(unsigned int);
extern Result KeyboardPoll(unsigned int);
extern unsigned int KeyboardGetKeyDownCount(unsigned int);
extern bool KeyboadGetKeyIsDown(unsigned int, unsigned short);
extern unsigned short KeyboardGetKeyDown(unsigned int, unsigned int);

keyBinding keyboardDisconnectedFunc;
keyBinding keyBindings[MAX_KEYS];
bool keyWasDown[MAX_KEYS];

unsigned int keyboardAddress;						

bool KeyboardGetKeyIsDown(unsigned int addr, unsigned short keycode) {
	return KeyboadGetKeyIsDown(addr, keycode);
}

/*
* KeyCode:
* Return key code for a specific character, in the region
* CSUD recognises.
* char code: The key code to convert
*
* Returns: (u32) Key code in the range CSUD recognises.
*/
u32 KeyCode(char code) {
	if(code >= 'a' && code <= 'z') {
		return code - 93;
	} else if(code >= 'A' && code <= 'Z') {
		return code - 61;
	} else {
		return code;
	}
}

/*
* BindKey:
* Bind an event to a particular keyboard key.
* char code: The key code to bind the event to
* keyBinding event: The event to call when the key is pressed.
*
* Returns: (u32) Key code in the range CSUD recognises.
*/
void BindKey(char code, keyBinding event) {
	u32 index;
	index = KeyCode(code) - 4;
	keyBindings[index] = event;
}

/*
* UnbindKey:
* Unbind an event to a particular keyboard key.
* char code: The key event to unbind
*/
void UnbindKey(char code) {
	u32 index;
	index = KeyCode(code) - 4;
	keyBindings[index] = NULL;
}

/*
* OnKeyboardDisconnected:
* Set up a function to be called when a 
* keyboard is disconnected.
* keyBinding func: The event to call when a keyboard is disconnected.
*/
void OnKeyboardDisconnected(keyBinding func) {
	keyboardDisconnectedFunc = func;
}

/*
* DetectKeyboards:
* Detect changes in the USB driver and assign a new
* keyboard if necessary.
*/
void DetectKeyboards(void) {
	// Check for new connections / disconnections
	UsbCheckForChange();
	if(KeyboardCount() > 0) {
		keyboardAddress = KeyboardGetAddress(0);
	}
}

/*
* ProcessKeyboardEvents:
* Main keyboard event processing function. Checks key
* states and triggers events if a key is pressed.
*/
void ProcessKeyboardEvents(void) {
	bool keyDown;
	int i;

	// Retrieve the keyboard address if we
	// haven't stored one yet.
	if(keyboardAddress == 0) {
		DetectKeyboards();
	}

	// After detecting the keyboard
	// successfully, detect key events
	if(keyboardAddress != 0) {
		for(i = 0; i < MAX_KEYS; i++) {
			if(keyBindings[i] != NULL) {
				keyDown = KeyboardGetKeyIsDown(keyboardAddress, i + 4);

				if(keyDown == true) {
					DebugLog("Firing key-bound event.");
					keyBindings[i]();
				}
				keyWasDown[i] = keyDown;
			}
		}

		KeyboardPoll(keyboardAddress);
	} else {
		if(keyboardDisconnectedFunc != NULL) {
			keyboardDisconnectedFunc();
		}
		DetectKeyboards();
	}
}

/*
* KeyboardInit:
* Initialise data structures ready for keyboard
* processing.
*/
void KeyboardInit(void) {
	int i;
	for(i = 0; i < MAX_KEYS; i++) {
		keyBindings[i] = NULL;
		keyWasDown[i] = false;
	}

	keyboardDisconnectedFunc = NULL;
	keyboardAddress = KeyboardGetAddress(0);
}