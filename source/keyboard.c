/****************************************************
* keyboard.c
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/platform.h"
#include "debug.h"

#define NULL 0
/* CSUD Return Codes */
#define KEYBOARD_SUCCESS 0
#define KEYBOARD_INVALID_ARGUMENT -2
#define KEYBOARD_BAD_DEVICE_RESPONSE -4
#define KEYBOARD_INCOMPATIBLE -5
#define KEYBOARD_BROKEN_LIBRARY -6
#define KEYBOARD_OUT_OF_MEMORY -7
#define KEYBOARD_TIMEOUT -8
#define KEYBOARD_DISCONNECTED -9

#define MAX_KEYS 26

#ifndef __cplusplus
	typedef enum {
		false = 0,
		true = 1,
	} bool;
#endif

typedef enum {
	OK = 0,
	ErrorGeneral = -1,
	ErrorArgument = -2,
	ErrorRetry = -3,
	ErrorDevice = -4,
	ErrorIncompatible = -5,
	ErrorCompiler = -6,
	ErrorMemory = -7,
	ErrorTimeout = -8,
	ErrorDisconnected = -9,
} Result;

typedef void (*keyBinding)(void);

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

unsigned int KeyCode(char code) {
	if(code >= 'a' && code <= 'z') {
		return code - 93;
	} else if(code >= 'A' && code <= 'Z') {
		return code - 61;
	} else {
		return code;
	}
}

void BindKey(unsigned char code, keyBinding event) {
	unsigned int index;
	index = KeyCode(code) - 4;
	keyBindings[index] = event;
}

void UnbindKey(unsigned char code) {
	unsigned int index;
	index = KeyCode(code) - 4;
	keyBindings[index] = NULL;
}

void OnKeyboardDisconnected(keyBinding func) {
	keyboardDisconnectedFunc = func;
}

void DetectKeyboards(void) {
	// Check for new connections / disconnections
	UsbCheckForChange();
	if(KeyboardCount() > 0) {
		keyboardAddress = KeyboardGetAddress(0);
	}
}

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

void KeyboardInit(void) {
	int i;
	for(i = 0; i < MAX_KEYS; i++) {
		keyBindings[i] = NULL;
		keyWasDown[i] = false;
	}

	keyboardDisconnectedFunc = NULL;
	keyboardAddress = KeyboardGetAddress(0);
}