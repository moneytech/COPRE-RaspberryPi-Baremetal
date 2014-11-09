/****************************************************
* debug.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/graphics.h"

#define MAX_LINES 35

char* lines[MAX_LINES];

/*
* InitDebug:
* Initialise data ready to handle debug information.
*/
void InitDebug()
{
	int i;
	
	for(i = 0; i < MAX_LINES; i++) {
		lines[i] = "";
	}
}

/*
* DebugLog:
* Add a message to the debug logger.
* char* text: The message to add to the logger.
*/
void DebugLog(char * text)
{
	int i;
	
	for(i = MAX_LINES - 1; i > 0; i--) {
		lines[i] = lines[i - 1];
	}

	lines[0] = text;
}

/*
* RenderDebugLog:
* Render the debug log to the screen.
*/
void RenderDebugLog()
{
	int i;

	for(i = 0; i < MAX_LINES; i++) {
		RenderFont(lines[i], 0, 580 - (i * 16));
	}
}