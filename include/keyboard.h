/****************************************************
* keyboard.h
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

typedef void (*keyBinding)(void);

#ifndef __cplusplus
	typedef enum {
		false = 0,
		true = 1,
	} bool;
#endif

extern unsigned int KeyCode(char);
extern void BindKey(unsigned char code, keyBinding event);
extern void OnKeyboardDisconnected(keyBinding func);
extern void UnbindKey(unsigned char code);
extern void ProcessKeyboardEvents(void);