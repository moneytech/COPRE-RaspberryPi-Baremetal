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

// Currently test for 2D image transfer
// to framebuffer
void DMATransfer2D(u32 source, u32 dest, short widthBytes, short height, int channel) {
	if(channel >= 0 && channel < 15) {
		DMACB ctrlBlock __attribute__((aligned(256)));
		u32 reg = DMA_BASE + (channel * 0x100);
		short dStride = (1920 - (widthBytes / 4)) * 4;

		ctrlBlock.transferInformation = DMA_TI_SRC_INC | DMA_TI_DEST_INC | DMA_TI_2D_MODE;
		ctrlBlock.sourceAddr = source;
		ctrlBlock.destAddr = dest;
		ctrlBlock.len = widthBytes + (height << 16);
		ctrlBlock.stride = (dStride << 16);
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