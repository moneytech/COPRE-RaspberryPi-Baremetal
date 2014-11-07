/****************************************************
* types.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#pragma once

extern void PutUInt32(unsigned int, unsigned int);
extern unsigned int GetUInt32(unsigned int);

typedef unsigned int u32;

typedef struct framebuffer_init_t {
	u32 width;
	u32 height;
	u32 virtWidth;
	u32 virtHeight;
	u32 pitch;
	u32 bitDepth;
	u32 xOffset;
	u32 yOffset;
	u32 ptr;
	u32 size;
} framebuffer_init_t;