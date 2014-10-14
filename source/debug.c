/****************************************************
* debug.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "graphics.h"

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern unsigned int imageFont;

void DebugLog()
{
	char text [6];
	text[0] = 'h'; text[1] = 'e'; text[2] = 'l'; text[3] = 'l'; text[4] = 'o'; text[5] = 0;

	unsigned int counter; counter = 0;
	while (text[counter] != 0) 
	{
		unsigned int yOffset; yOffset = ((text[counter] - 32) / 16);
		unsigned int xOffset; xOffset = ((text[counter] - 32) - (yOffset * 16));
		RenderPartImage(400 + (counter * 8),500,8,16,xOffset * 8,yOffset * 16,128,96,(unsigned int)&imageFont);
		counter++;
	} 
	
}