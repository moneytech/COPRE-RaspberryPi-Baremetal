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
* dma.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/dma.h"
#include "../include/debug.h"

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

/*
* DMAEnable:
* Enable DMA transfer channels.
*
* u32 channelBits: Combination of values produced by the
* DMA_ENABLE_ENGINE macro.
*/
void DMAEnable(u32 channelBits) {
	*DMA_ENABLE = channelBits;
}

/*
* DMATransfer:
* Transfer data over a DMA channel.
*
* u32 source: Transfer source address
* u32 dest: Transfer destination address
* int len: Byte length of data transfer
* int channel: DMA transfer channel to use
*/
void DMATransfer(u32 source, u32 dest, int len, int channel) {
	if(channel >= 0 && channel < 15) {
		DMACB ctrlBlock __attribute__((aligned(256)));
		u32 reg = DMA_BASE + (channel * 0x100);

		ctrlBlock.transferInformation = DMA_TI_SRC_INC | DMA_TI_DEST_INC;
		ctrlBlock.sourceAddr = source;
		ctrlBlock.destAddr = dest;
		ctrlBlock.len = len;
		ctrlBlock.stride = 0;
		ctrlBlock.nextControlBlockAddr = 0;
		ctrlBlock.reserved1 = 0;
		ctrlBlock.reserved2 = 0;

		PutUInt32(reg + DMA_CONTROL_BLOCK_ADDR, (u32)&ctrlBlock);
		// Activate DMA
		PutUInt32(reg + DMA_CONTROL_AND_STATUS, 1);

		// Wait for transfer completion
		while((GetUInt32(reg + DMA_CONTROL_AND_STATUS) & 0x2) == 0) { };
		//DebugLog("DMA Transfer Complete.");
	}
}

/*
* ImageToFrameBuffer:
* Copy an image to the frame buffer, through a 2D DMA transfer.
*
* u32 source: Transfer source address
* u32 dest: Transfer destination address (the framebuffer)
* int width: The width of the image
* int height: The height of the image
* int channel: The DMA channel to use during the transfer 
*/
void ImageToFrameBuffer(u32 source, u32 dest, short width, short height, int channel) {
	if(channel >= 0 && channel < 15) {
		DMACB ctrlBlock __attribute__((aligned(256)));
		u32 reg = DMA_BASE + (channel * 0x100);
		short dStride = (1920 - width) * 4;

		ctrlBlock.transferInformation = DMA_TI_SRC_INC | DMA_TI_DEST_INC | DMA_TI_2D_MODE;
		ctrlBlock.sourceAddr = source;
		ctrlBlock.destAddr = dest;
		ctrlBlock.len = (width * 4) + (height << 16);
		ctrlBlock.stride = (dStride << 16);
		ctrlBlock.nextControlBlockAddr = 0;
		ctrlBlock.reserved1 = 0;
		ctrlBlock.reserved2 = 0;

		PutUInt32(reg + DMA_CONTROL_BLOCK_ADDR, (u32)&ctrlBlock);
		// Activate DMA
		PutUInt32(reg + DMA_CONTROL_AND_STATUS, 1);

		// Wait for transfer completion
		while((GetUInt32(reg + DMA_CONTROL_AND_STATUS) & 0x1) == 1) { };
		//DebugLog("DMA Transfer Complete.");
	}
}