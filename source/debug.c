/****************************************************
* debug.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "graphics.h"

#define MAX_LINES 35

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

char* lines[MAX_LINES];

void InitDebug()
{
	int i;
	
	for(i = 0; i < MAX_LINES; i++) {
		lines[i] = "";
	}
}

void DebugLog(char * text)
{
	int i;
	
	for(i = MAX_LINES - 1; i > 0; i--) {
		lines[i] = lines[i - 1];
	}

	lines[0] = text;
}

void RenderDebugLog()
{
	int i;

	for(i = 0; i < MAX_LINES; i++) {
		RenderFont(lines[i], 0, 580 - (i * 16));
	}
}