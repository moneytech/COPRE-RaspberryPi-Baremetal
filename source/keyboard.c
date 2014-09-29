/****************************************************
* keyboard.c
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

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

#ifndef __cplusplus
	typedef enum {
		false = 0,
		true = 1,
	} bool;
#endif

typedef void (*keyBinding)(void);

/* External links to CSUD functions */
extern void UsbCheckForChange(void);
extern unsigned int KeyboardCount(void);
extern unsigned int KeyboardGetAddress(unsigned int);
extern unsigned int KeyboardPoll(unsigned int);
extern unsigned int KeyboardGetKeyDownCount(unsigned int);
extern bool KeyboadGetKeyIsDown(unsigned int, unsigned short);
extern bool KeyboardGetKeyDown(unsigned int, unsigned int);

extern void OkLedOn();
extern void OkLedOff();

keyBinding keyBindings[26] = { 	NULL, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL, NULL, NULL, NULL, NULL,
								NULL, NULL };

bool keyWasDown[26] = { false, false, false, false, false, false,
						false, false, false, false, false, false,
						false, false, false, false, false, false,
						false, false, false, false, false, false,
						false, false };

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

void ProcessKeyboardEvents(void) {
	unsigned int address;
	bool keyDown;
	int i;
	
	// Check for new connections / disconnections
	UsbCheckForChange();
	
	// Execute input operations if there is at least
	// one keyboard connected
	if(KeyboardCount() > 0) {
		address = KeyboardGetAddress(0);

		for(i = 0; i < 26; i++) {
			if(keyBindings[i] != NULL) {
				keyDown = KeyboardGetKeyIsDown(address, i + 4);
				// Key has to be down for at least 2 polls:
				// this stops dodgy input periods
				if(keyWasDown[i] == true && keyDown == true) {
					keyBindings[i]();
				}

				keyWasDown[i] = keyDown;
			}
		}

		KeyboardPoll(address);
	}
}