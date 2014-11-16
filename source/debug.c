/*
The MIT License (MIT)

Copyright (c) 2014 Niall Frederick Weedon, Timothy Stanley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

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