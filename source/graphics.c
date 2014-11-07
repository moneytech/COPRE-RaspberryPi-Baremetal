/****************************************************
* graphics.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/debug.h"
#include "../include/types.h"
#include "../include/mailbox.h"

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

// ----- Image data -----
extern unsigned int imageSplash;
extern unsigned int imageFont;

// ----- Frame buffer information -----
static unsigned int m_screenWidth;
static unsigned int m_screenHeight;
static unsigned int m_bitDepth;
static unsigned int m_framebufferAddress;

unsigned int bgColour;
unsigned int backBuffer[800][600];

unsigned int GetFramebufferAddress(void) {
	return m_framebufferAddress;
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

	// Set up the frame buffer info	
	framebuffer_init_t* init = (framebuffer_init_t*)0x44040000;
	
	init->width = m_screenWidth;
	init->height = m_screenHeight;
	init->virtWidth = m_screenWidth;
	init->virtHeight = m_screenHeight;
	init->pitch = 0;
	init->bitDepth = m_bitDepth;
	init->xOffset = 0;
	init->yOffset = 0;
	init->ptr = 0;
	init->size = 0;

	// send the frame buffer info to channel 1
	if (MailboxWrite(&init, 1) == 0)
	{
		return 0;
	}

	unsigned int frameBufferResponse;
	frameBufferResponse = MailboxRead(1);

	if (frameBufferResponse == 0)
	{
		m_framebufferAddress = init->ptr;
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
	int i, j;

	for (i = 0; i < m_screenHeight; i++)
	{
		for (j = 0; j < m_screenWidth; j++)
		{
			backBuffer[j][i] = bgColour;
		}
	}
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
	int i, j, colour;

	for(i = y; i < (y + height); i++) {
		for(j = x; j < (x + width); j++) {
			colour = *imageAddress++;

			if(colour != 0xFFFFFFFF) {
				if((x + j) >= 0 && (x + j) < 800 && (y + i) >= 0 && (y + i) < 600) {
					backBuffer[j][i] = colour;
				}
			}
		}
	}
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
	int i, j, colour;
	unsigned int* imgAddress;

	if(imageAddress != 0) {
		imgAddress = imageAddress;
		imgAddress += ((offsetY * imageWidth) + offsetX);

		for(i = 0; i < height; i++) 
		{
			for(j = 0; j < width; j++) 
			{
				if((x + j) >= 0 && (x + j) < 800 && (y + i) >= 0 && (y + i) < 600) {
					colour = *imgAddress++;
					backBuffer[x + j][y + i] = colour;
				}
			}

			imgAddress += (imageWidth - width);
		}
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

	if(text != 0) {
		while (text[counter] != 0) 
		{
			yOffset = ((text[counter] - 32) / 16);
			xOffset = ((text[counter] - 32) - (yOffset * 16));
			RenderPartImage(x + (counter * 8), y, 8, 16, xOffset * 8, yOffset * 16, 128, 96, &imageFont);
			counter++;
		}
	}
}

/*
* SwapBuffers:
* Swap the frame buffer with the back buffer.
*/
void SwapBuffers(void) {
	int i, j;
	unsigned int frameAddr; frameAddr = m_framebufferAddress;

	for(i = 0; i < 600; i++) {
		for(j = 0; j < 800; j++) {
			// Place the value of the backbuffer into
			// the framebuffer
			PutUInt32(frameAddr, backBuffer[j][i]);
			frameAddr += 4;
		}
	}
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
	RenderImage(0, 0, 800, 600, &imageSplash);
	RenderDebugLog();
	// Swap the back buffer info with that of the
	// frame buffer.
	SwapBuffers();

	bgColour += 0x00010101;
}
