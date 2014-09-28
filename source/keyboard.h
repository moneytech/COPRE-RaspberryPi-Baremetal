/****************************************************
* keyboard.h
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/
#define KEYBOARD_SUCCESS 0
#define KEYBOARD_INVALID_ARGUMENT -2
#define KEYBOARD_BAD_DEVICE_RESPONSE -4
#define KEYBOARD_INCOMPATIBLE -5
#define KEYBOARD_BROKEN_LIBRARY -6
#define KEYBOARD_OUT_OF_MEMORY -7
#define KEYBOARD_TIMEOUT -8
#define KEYBOARD_DISCONNECTED -9

//extern int UsbInitialise(void);
extern void UsbCheckForChange(void);
extern unsigned int KeyboardCount(void);
extern unsigned int KeyboardGetAddress(unsigned int);
extern unsigned int KeyboardPoll(unsigned int);
extern unsigned int KeyboardGetModifiers(unsigned int);
extern unsigned int KeyboardGetKeyDownCount(unsigned int);
extern unsigned int KeyboardGetKeyDown(unsigned int, unsigned int);
extern unsigned int KeyboardGetKeyIsDown(unsigned int, unsigned int);
extern unsigned int KeyboardGetLedSupport(unsigned int);
extern unsigned int KeyboardSetLeds(unsigned int, unsigned int);
