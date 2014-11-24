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
* v3d-qpu.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/v3d-qpu.h"
#include "../include/v3d-list.h"
#include "../include/v3d-controllist.h"
#include "../include/mailbox.h"
#include "../include/debug.h"
#include "../include/csud/platform.h"

volatile unsigned int* v3d = (unsigned int*)0x20C00000;

bool TestControlLists(void);

// Purely for testing we can work
// with the mailbox property interface
bool TestMailboxPropertyInterface(void) {
	volatile unsigned int mailbuffer[256] __attribute__((aligned(16)));

	mailbuffer[0] = 8 * 4;
	mailbuffer[1] = 0;
	mailbuffer[2] = 0x40003;
	mailbuffer[3] = 8;
	mailbuffer[4] = 0;
	mailbuffer[5] = 0;
	mailbuffer[6] = 0;
	mailbuffer[7] = 0;

	MailboxWrite((unsigned int)mailbuffer, 8);
	MailboxRead(8);

	if(mailbuffer[1] != 0x80000000) {
		DebugLog("Error getting resolution.");
		return false;
	} else {
		LOGF("Attained resolution: %d by %d", mailbuffer[5], mailbuffer[6]);
		return true;
	}
}

bool InitV3D(void) {
	volatile unsigned int p[32] __attribute__((aligned(16)));
	int i; i = 0;

	p[i++] = 0;
	p[i++] = 0x00000000;

	// Set Clock Rate
	p[i++] = 0x38002;
	p[i++] = 0x00000008;
	p[i++] = 0x00000008;
	p[i++] = 5; // CLK_V3D_ID
	p[i++] = 250 * 1000 * 1000;
	// ---

	// Enable QPU
	p[i++] = 0x30012;
	p[i++] = 0x00000004;
	p[i++] = 0x00000004;
	p[i++] = 1; // Enable
	// ---

	// End Tag
	p[i++] = 0x00000000;
	// ---

	p[0] = i * sizeof(*p);

	MailboxWrite((u32)p, 8);
	MailboxRead(8);

	if(v3d[V3D_IDENT0] == 0x02443356) {
		DebugLog("QPU successfully initialised.");
		return TestControlLists();
	} else {
		LOGF("QPU Init Error. V3DIdent: 0x%08x", v3d[V3D_IDENT0]);
		return false;
	}
}

u32 AllocateGPUMemory(u32 size, u32 align, u32 flags) {
	volatile unsigned int mailbuffer[32] __attribute__((aligned(16)));
	int i; i = 0;

	mailbuffer[i++] = 0;
	mailbuffer[i++] = 0x00000000;
	
	mailbuffer[i++] = 0x3000C;
	mailbuffer[i++] = 12;
	mailbuffer[i++] = 12;
	mailbuffer[i++] = size;
	mailbuffer[i++] = align;
	mailbuffer[i++] = flags;

	mailbuffer[i++] = 0x00000000;
	mailbuffer[0] = i * sizeof(*mailbuffer);

	MailboxWrite((u32)mailbuffer, 8);
	MailboxRead(8);
	return mailbuffer[5];
}

u32 FreeGPUMemory(u32 handle) {
	volatile unsigned int mailbuffer[32] __attribute__((aligned(16)));
	int i; i = 0;

	mailbuffer[i++] = 0;
	mailbuffer[i++] = 0x00000000;
	
	mailbuffer[i++] = 0x3000F;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = handle;

	mailbuffer[i++] = 0x00000000;
	mailbuffer[0] = i * sizeof(*mailbuffer);

	MailboxWrite((u32)mailbuffer, 8);
	MailboxRead(8);
	return mailbuffer[5];
}

u32 LockGPUMemory(u32 handle) {
	volatile unsigned int mailbuffer[32] __attribute__((aligned(16)));
	int i; i = 0;

	mailbuffer[i++] = 0;
	mailbuffer[i++] = 0x00000000;
	
	mailbuffer[i++] = 0x3000D;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = handle;

	mailbuffer[i++] = 0x00000000;
	mailbuffer[0] = i * sizeof(*mailbuffer);

	MailboxWrite((u32)mailbuffer, 8);
	MailboxRead(8);

	//LOGF("0x%08x ", mailbuffer[5]);

	return mailbuffer[5];
}

u32 UnlockGPUMemory(u32 handle) {
	volatile unsigned int mailbuffer[32] __attribute__((aligned(16)));
	int i; i = 0;

	mailbuffer[i++] = 0;
	mailbuffer[i++] = 0x00000000;
	
	mailbuffer[i++] = 0x3000E;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = 4;
	mailbuffer[i++] = handle;

	mailbuffer[i++] = 0x00000000;
	mailbuffer[0] = i * sizeof(*mailbuffer);

	MailboxWrite((u32)mailbuffer, 8);
	MailboxRead(8);
	return mailbuffer[5];
}

// Test control list functionality.
// Reference: phire/hackdriver
bool TestControlLists(void) {
	u32 handle, addr;
	int i;

	handle = AllocateGPUMemory(0x100, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);

	if(!handle) {
		DebugLog("V3D: Unable to allocate memory.");
		return false;
	}

	addr = LockGPUMemory(handle);
	//LOGF("V3D: GPU memory locked at 0x%08x.", addr);
	DebugLog("GPU memory locked.");

	// Construct a control list, 254 nops
	// with a halt in the middle.
	u8* list = (u8*)addr;

	for(i = 0; i < 255; i++) {
		list[i] = 1; // Nop
	}

	list[128] = 0; // Halt

	// Set up the V3D pipeline to execute the
	// control list
	v3d[V3D_CT0CA] = addr;
	v3d[V3D_CT0EA] = addr + 0x100;

	// Wait for execution to complete
	while(v3d[V3D_CT0CS] & 0x20);

	UnlockGPUMemory(handle);
	FreeGPUMemory(handle);

	return true;
}

/*
* GPUClearScreen:
* Clear the screen with a specific colour using the GPU.
* u32 framebufferAddress: The address to write to.
* u32 clearColour: The colour to clear the screen with.
*/
bool GPUClearScreen(u32 framebufferAddress, u32 clearColour) {
	u32 handle, addr;
	u8 x, y;

	handle = AllocateGPUMemory(0x4000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);

	if(!handle) {
		DebugLog("V3D: Unable to allocate memory.");
		return false;
	}

	addr = LockGPUMemory(handle);
	//LOGF("V3D: GPU memory locked at 0x%08x.", addr);
	//DebugLog("GPU memory locked.");

	u8* list = (u8*)addr;

	// Clear colour
	CLEAR_COLOUR(list, clearColour);

	// Tile rendering mode configuration
	TILE_RENDERING_MODE_CONFIG(list, framebufferAddress, 1920, 1080);

	// Each tile is 64*64 pixels
	u8 maxYTiles = (1080 / 64) + 1;
	u8 maxXTiles = (1920 / 64) + 1;

	for(y = 0; y < maxYTiles; y++) {
		for(x = 0; x < maxXTiles; x++) {
			// Tile coordinates
			TILE_COORDINATE(list, x, y);

			// Store multisample
			if(x == maxXTiles - 1 && y == maxYTiles - 1) {
				MULTISAMPLE_END(list); // Multisample End
			} else {
				MULTISAMPLE_STORE(list); // Store multisample
			}
		}
	}

	// Set up the V3D pipeline to execute the
	// control list
	v3d[V3D_CT1CA] = addr;
	v3d[V3D_CT1EA] = (u32)&list;

	// Wait for execution to complete
	while(v3d[V3D_CT1CS] & 0x20);

	UnlockGPUMemory(handle);
	FreeGPUMemory(handle);

	return true;
}