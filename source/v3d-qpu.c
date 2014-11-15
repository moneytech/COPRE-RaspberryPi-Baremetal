/****************************************************
* v3d-qpu.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/v3d-qpu.h"
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

void AddWord(u8** list, u32 word) {
	*((*list)++) = word & 0xff;
	*((*list)++) = (word >> 8) & 0xff;
	*((*list)++) = (word >> 16) & 0xff;
	*((*list)++) = (word >> 24) & 0xff;
}

void AddShort(u8** list, u16 data) {
	*((*list)++) = data & 0xff;
	*((*list)++) = (data >> 8) & 0xff;
}

// (Still working on this)
bool GPUClearScreen(u32 framebufferAddress) {
	u32 handle, addr;
	int i;
	u8 x, y;

	// 8MB buffer for now
	handle = AllocateGPUMemory(0x800000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);

	if(!handle) {
		DebugLog("V3D: Unable to allocate memory.");
		return false;
	}

	addr = LockGPUMemory(handle);
	//LOGF("V3D: GPU memory locked at 0x%08x.", addr);
	//DebugLog("GPU memory locked.");

	u8* list = (u8*)addr;
	i = 0;

	// Clear colour
	list[i++] = 114;
	// White (has to be repeated twice
	// for 32-bit colour)
	AddWord(&list, 0xffffffff);
	i += 4;
	AddWord(&list, 0xffffffff);
	i += 4;

	AddWord(&list, 0);
	i += 4;

	list[i++] = 0;
	// ---

	// Tile rendering mode configuration
	list[i++] = 113;
	AddWord(&list, framebufferAddress);
	i += 4;
	// Screen dimensions
	AddShort(&list, 800);
	AddShort(&list, 600);
	i += 4;
	list[i++] = 0x04; // Framebuffer mode (linear rgba8888)
	list[i++] = 0x00;
	// ---

	// 8 columns and rows
	for(y = 0; y < 8; y++) {
		for(x = 0; x < 10; x++) {
			// Tile coordinates
			list[i++] = 115;
			list[i++] = x;
			list[i++] = y;
			// Store multisample
			list[i++] = 24;
		}
	}

	list[i - 1] = 25; // Multisample end
	list[i] = 0; // Nop

	// Set up the V3D pipeline to execute the
	// control list
	v3d[V3D_CT1CA] = addr;
	v3d[V3D_CT1EA] = addr + i;

	// Wait for execution to complete
	while(v3d[V3D_CT1CS] & 0x20);

	UnlockGPUMemory(handle);
	FreeGPUMemory(handle);

	return true;
}