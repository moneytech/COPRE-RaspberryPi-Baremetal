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
* graphics.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/debug.h"
#include "../include/types.h"
#include "../include/mailbox.h"
#include "../include/v3d-qpu.h"
#include "../include/dma.h"
#include "../include/game.h"

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

extern void memcpy(void* dest, void* source, unsigned int count);
extern void* memset(void* dest, int c, unsigned int count);

// ----- Image data -----
extern unsigned int imageSplash;
extern unsigned int imageFont;
extern unsigned int blockBlue;
extern unsigned int blockGreen;
extern unsigned int blockYellow;
extern unsigned int blockRed;

// ----- Frame buffer information -----
static unsigned int m_screenWidth;
static unsigned int m_screenHeight;
static unsigned int m_bitDepth;
static unsigned int m_framebufferAddress, m_secondBufferAddress;

unsigned int bgColour;
bool incrementingBgColour;
int m_currentlyDisplayedBuffer;

// ----- Used for debugging -----
//unsigned int backBuffer[1080][1920];
extern unsigned int GetTickCount(void);
unsigned int updateTick;
int bounceX = 0;
int bounceY = 0;
bool travellingRight = true;
bool travellingDown = true;

// ----- Image placement macros -----
#define FRAME_BUFFER_OFFSET(x, y) (((y * m_screenWidth) + x) * 4)

unsigned int GetWriteFramebufferAddress(void) {
	if(m_currentlyDisplayedBuffer == 0) {
		return m_secondBufferAddress;
	} else {
		return m_framebufferAddress;
	}
}

int GetWriteBufferIndex(void) {
	return ((m_currentlyDisplayedBuffer + 1) % 2);
}

/*
* InitGraphics:
* Initialises all graphics-related information, including the frame buffer.
* u32 screenWidth: The width of the screen to create.
* u32 screenHeight: The height of the screen to create.
* u32 bitDepth: The number of bits to use for each pixel in the frame buffer.
*
* Returns: (u32) 1 for success, 0 for failure
*/
u32 InitGraphics(u32 screenWidth, u32 screenHeight, u32 bitDepth)
{
	// Set global parameters
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	m_bitDepth = bitDepth;

	bgColour = 0xFF000000;
	incrementingBgColour = true;
	m_currentlyDisplayedBuffer = 0;

	// Set up the frame buffer info	
	framebuffer_init_t* init __attribute__((aligned(16)))
		= (framebuffer_init_t*)0x44040000;
	
	init->width = m_screenWidth;
	init->height = m_screenHeight;
	init->virtWidth = m_screenWidth;
	init->virtHeight = m_screenHeight * 2;
	init->pitch = 0;
	init->bitDepth = m_bitDepth;
	init->xOffset = 0;
	init->yOffset = 0;
	init->ptr = 0;
	init->size = 0;

	updateTick = GetTickCount();
	
	// send the frame buffer info to channel 1
	if (MailboxWrite((u32)init, 1) == 0)
	{
		return 0;
	}

	unsigned int frameBufferResponse;
	frameBufferResponse = MailboxRead(1);

	if (frameBufferResponse == 0)
	{
		m_framebufferAddress = init->ptr;
		m_secondBufferAddress = init->ptr + (m_screenWidth * m_screenHeight);
		return 1;
	}

	return 0;
}

/*
* RenderBackground:
* Renders the background, in this case filling the
* background with bgColour.
*/
void RenderBackground(void)
{
	int yOffset;

	if(m_currentlyDisplayedBuffer == 0) {
		yOffset = 1080;
	} else {
		yOffset = 0;
	}

	GPUClearScreen(m_framebufferAddress + FRAME_BUFFER_OFFSET(0, yOffset), 0xff000000);
}

/*
* RenderImage:
* Render an image to the frame buffer.
* u32 x: The x position of the top-left corner of the image
* u32 y: The y position of the top-left corner of the image
* u32 width: The pixel width of the image
* u32 height: The pixel height of the image
* u32* imageAddress: The starting address of the image data
*/
void RenderImage(u32 x, u32 y, u32 width, u32 height, u32* imageAddress) 
{
	int yOffset;

	if(m_currentlyDisplayedBuffer == 0) {
		yOffset = 1080;
	} else {
		yOffset = 0;
	}

	ImageToFrameBuffer((u32)imageAddress, m_framebufferAddress + FRAME_BUFFER_OFFSET(x, (y + yOffset)), width, height, 8);
}

/*
* RenderPartImage:
* Render part of an image to the frame buffer.
* u32 x: The x position of the top-left corner of the image
* u32 y: The y position of the top-left corner of the image
* u32 width: The width of the image part to render
* u32 height: The height of the image part to render
* u32 offsetX: The starting x position in the image to render
* u32 offsetY: The starting y position in the image to render
* u32 imageWidth: The pixel width of the image
* u32 imageHeight: The pixel height of the image
* u32* imageAddress: The starting address of the image data
*/
void RenderPartImage(u32 x, u32 y, u32 width, u32 height, u32 offsetX, u32 offsetY, u32 imageWidth, u32 imageHeight, u32* imageAddress)
{
	unsigned int* imgAddress;

	int yOffset;

	if(m_currentlyDisplayedBuffer == 0) {
		yOffset = 1080;
	} else {
		yOffset = 0;
	}

	if(imageAddress != 0) 
	{
		imgAddress = imageAddress;
		imgAddress += ((offsetY * imageWidth) + offsetX);

		ImageToFrameBuffer((u32)imgAddress, m_framebufferAddress + FRAME_BUFFER_OFFSET(x, (y + yOffset)), width, height, 1);
	}
}

/*
* RenderFont:
* Render a text string to the frame buffer
* char* text: The text to render
* u32 x: The x position to start rendering the text
* u32 y: The y position to start rendering the text
*/
void RenderFont(char * text, u32 x, u32 y)
{
	unsigned int counter; counter = 0;
	unsigned int xOffset, yOffset;

	if(text != 0) 
	{
		while (text[counter] != 0) 
		{
			if ((text[counter] >= 48 && text[counter] <= 57) )
			{
				yOffset = (text[counter] - 48);
				xOffset = 0;
				//xOffset = ((text[counter] - 32) - (yOffset * 16));
				RenderPartImage(x + (counter * 25), y, 25, 32, xOffset, yOffset * 32, 25, 1152, &imageFont);
			}
			else if (text[counter] >= 97 && text[counter] <= 122)// lower case
			{
				yOffset = (text[counter] - 87);
				xOffset = 0;

				RenderPartImage(x + (counter * 25), y, 25, 32, xOffset, yOffset * 32, 25, 1152, &imageFont);
			}
			else if (text[counter] >= 65 && text[counter] <= 90)
			{
				yOffset = (text[counter] - 55);
				xOffset = 0;

				RenderPartImage(x + (counter * 25), y, 25, 32, xOffset, yOffset * 32, 25, 1152, &imageFont);
			}
			counter++;
		}
	}
}

void RenderFontI(int number, u32 x, u32 y)
{
	char buffer[10];
	char final[10];
	if (number >= 0 && number < 9999999999) //check to make sure the buffer wont overflow with a huge number
	{
		int i, digit; i = 9;
		do 
		{
			digit = number % 10;
			number = number / 10;
			buffer[i]= digit + 0x30; //add on 48 to make it a char value
			i--;
		}while(number > 10);

		buffer[i] = number + 0x30;
	

		int j, counter;counter = 0;
		for (j = 0; j < 10; j++)
		{
			if (buffer[j] > 0)
			{
				final[counter] = buffer[j];
				counter++;
			}	
		}

		RenderFont(final, x, y);
	}
}

/*
* SwapBuffers:
* Swap the frame buffer with the back buffer.
*/
void SwapBuffers(void) {
	//unsigned int frameAddr; frameAddr = m_framebufferAddress;
	//memcpy((unsigned int*)frameAddr, &backBuffer, 1920 * 1080 * sizeof(unsigned int));

	volatile unsigned int mailbuffer[32] __attribute__((aligned(16)));
	int i; i = 0;
	int y;

	// If the primary buffer if being displayed,
	// show set up the offset to show the second one
	if(m_currentlyDisplayedBuffer == 0) {
		y = m_screenHeight;
		m_currentlyDisplayedBuffer = 1;
	} else {
		y = 0;
		m_currentlyDisplayedBuffer = 0;
	}

	mailbuffer[i++] = 0;
	mailbuffer[i++] = 0x00000000;
	
	mailbuffer[i++] = 0x48009; // Set virtual offset
	mailbuffer[i++] = 8;
	mailbuffer[i++] = 8;
	mailbuffer[i++] = 0; // X offset
	mailbuffer[i++] = y; // Y offset

	mailbuffer[i++] = 0x00000000;
	mailbuffer[0] = i * sizeof(*mailbuffer);

	MailboxWrite((u32)mailbuffer, 8);
	MailboxRead(8);
}

/*
* UpdateGraphics:
* Main graphics updating loop.
*/
void UpdateGraphics(void)
{
	// All graphics should be rendered to the
	// back buffer.
	RenderBackground();
	//RenderImage(bounceX, bounceY, 800, 600, &imageSplash);

	int x, y, blockValue;
	unsigned int* texture;

	for(x = 0; x < 10; x++) {
		for(y = 0; y < 24; y++) {
			if(gameBoard[y][x] != 0) {
				blockValue = gameBoard[y][x];

				if(blockValue < 0) {
					blockValue *= -1;
				}

				switch(blockValue) {
					case 1:
						texture = &blockBlue;
						break;

					case 2:
						texture = &blockGreen;
						break;

					case 3:
						texture = &blockYellow;
						break;

					case 4:
						texture = &blockRed;
						break;

					default:
						texture = &blockBlue;
						break;
				}

				RenderImage(x * 32, y * 32, 32, 32, texture);
			}
		}
	}

	// Render bottom for debugging
	for(x = 0; x < 10; x++) {
		RenderImage(x * 32, 24 * 32, 32, 32, &imageSplash);
	}

	//RenderImage(320, 0, 800, 600, &imageSplash);
	RenderFontI(GetScore(), 50, 768);
	RenderFont("PITRIS", 500, 100);
	//RenderImage(100, 100, 32, 320, &imageFont);

	SwapBuffers();
}
