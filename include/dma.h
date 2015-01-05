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
* dma.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/platform.h"

#define DMA_BASE 0x20007000
#define DMA_CONTROL_AND_STATUS 0x0
#define DMA_CONTROL_BLOCK_ADDR 0x4
#define DMA_TRANSFER_INFORMATION 0x8
#define DMA_SOURCE_ADDRESS 0xC
#define DMA_DEST_ADDRESS 0x10
#define DMA_TRANSFER_LENGTH 0x14
#define DMA_STRIDE 0x18
#define DMA_NEXT_CONTROL_BLOCK 0x1C
#define DMA_DEBUG 0x20

#define DMA_TI_SRC_INC (1 << 8)
#define DMA_TI_DEST_INC (1 << 4)
#define DMA_TI_2D_MODE (1 << 1)

typedef struct {
	u32 transferInformation;
	u32 sourceAddr;
	u32 destAddr;
	u32 len;
	u32 stride;
	u32 nextControlBlockAddr;
	u32 reserved1;
	u32 reserved2;
} DMACB;

extern void DMATransfer(u32 source, u32 dest, int len, int channel);
extern void DMATransfer2D(u32 source, u32 dest, short widthBytes, short height, int channel);
