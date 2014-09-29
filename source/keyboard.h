/****************************************************
* keyboard.h
* C Interface for Alex Chadwick's USB Driver
* https://github.com/Chadderz121/csud
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

typedef void (*keyBinding)(void);

extern unsigned int KeyCode(char);
extern void BindKey(unsigned char code, keyBinding event);
extern void UnbindKey(unsigned char code);
extern void ProcessKeyboardEvents(void);