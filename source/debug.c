/****************************************************
* debug.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "graphics.h"

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

char * line1;
char * line2;
char * line3;

void InitDebug()
{
	line1 = "";
	line2 = "";
	line3 = "";
}

void DebugLog(char * text)
{
	line3 = line2;
	line2 = line1;
	line1 = text;	
}

void RenderDebugLog()
{
	RenderFont(line1, 0, 580);
	RenderFont(line2, 0, 564);
	RenderFont(line3, 0, 548);
}