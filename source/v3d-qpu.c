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

bool GPURenderTriangle(u32 framebufferAddress, u32 clearColour) {
	u32 handle, addr;
	u8 x, y;
	
	// 8MB buffer
	handle = AllocateGPUMemory(0x800000, 0x1000, MEM_FLAG_COHERENT | MEM_FLAG_ZERO);

	if(!handle) {
		DebugLog("V3D: Unable to allocate memory.");
		return false;
	}

	addr = LockGPUMemory(handle);

	u8* list = (u8*)addr;
	u8* shaderRecord = (u8*)addr + 0x80;
	u8* vertexData = (u8*)addr + 0xa0;
	u8* vertexList = (u8*)addr + 0x70;
	u8* fragmentShader = (u8*)addr + 0xfe00;
	u8* renderControlList = (u8*)addr + 0xe200;

	TILE_BINNING_CONFIGURATION(list, 
		addr + 0x6200, 
		0x8000, 
		addr + 0x100, 
		30, 
		17);

	START_TILE_BINNING(list);
	PRIMITIVE_LIST_FORMAT(list, 0x32); // 16-bit triangle

	CLIP_WINDOW(list, 0, 0, 1920, 1080);

	//CONFIGURATION_STATE(list,
	//	(ENABLE_BACKFACING_PRIMITIVES | 
	//	ENABLE_FRONTFACING_PRIMITIVES |
	//	EARLY_Z_ENABLE |
	//	EARLY_Z_UPDATE));

	//CONFIGURATION_STATE(list, (ENABLE_FRONTFACING_PRIMITIVES +
	//	ENABLE_BACKFACING_PRIMITIVES +
	//	CLOCKWISE_PRIMITIVES), 0);

	// Configuration State
	AddByte(&list, 96);
	AddByte(&list, 0x03);
	AddByte(&list, 0x00);
	AddByte(&list, 0x02);

	VIEWPORT_OFFSET(list, 0, 0);

	// Triangle data
	AddByte(&list, CODE_NV_SHADER_STATE);
	AddWord(&list, addr + 0x80); // Shader record

	// Primitive Index List
	AddByte(&list, CODE_INDEXED_PRIMITIVE_LIST);
	AddByte(&list, 0x04); // 8-bit index, triangles
	AddWord(&list, 3); // Length (number of triangles)
	AddWord(&list, addr + 0x70); // List address
	AddWord(&list, 2); // Maximu index
	// ---

	// End of Binning List
	AddByte(&list, CODE_FLUSH_ALL);
	AddByte(&list, CODE_NOP);
	AddByte(&list, CODE_HALT);
	// ---

	// Shader record ---
	AddByte(&shaderRecord, 0x01); // Flags
	AddByte(&shaderRecord, 6 * 4); // Stride
	AddByte(&shaderRecord, 0xCC); // Number of uniforms (not used)
	AddByte(&shaderRecord, 3); // Number of varyings
	AddWord(&shaderRecord, addr + 0xfe00); // Fragment shader code
	AddWord(&shaderRecord, addr + 0xff00); // Fragment shader uniforms
	AddWord(&shaderRecord, addr + 0xa0); // Vertex data address
	// ---

	// Vertex Data
	VERTEX(vertexData,
		960,
		200,
		1,
		1.0f,
		0.0f,
		0.0f);

	VERTEX(vertexData,
		560,
		800,
		1,
		0.0f,
		1.0f,
		0.0f);

	VERTEX(vertexData,
		1360,
		800,
		1,
		0.0f,
		0.0f,
		1.0f);

	// Vertex list
	AddByte(&vertexList, 0);
	AddByte(&vertexList, 1);
	AddByte(&vertexList, 2);

	// Fragment shader
	AddWord(&fragmentShader, 0x958e0dbf);
	AddWord(&fragmentShader, 0xd1724823); /* mov r0, vary; mov r3.8d, 1.0 */
	AddWord(&fragmentShader, 0x818e7176);
	AddWord(&fragmentShader, 0x40024821); /* fadd r0, r0, r5; mov r1, vary */
	AddWord(&fragmentShader, 0x818e7376);
	AddWord(&fragmentShader, 0x10024862); /* fadd r1, r1, r5; mov r2, vary */
	AddWord(&fragmentShader, 0x819e7540);
	AddWord(&fragmentShader, 0x114248a3); /* fadd r2, r2, r5; mov r3.8a, r0 */
	AddWord(&fragmentShader, 0x809e7009);
	AddWord(&fragmentShader, 0x115049e3); /* nop; mov r3.8b, r1 */
	AddWord(&fragmentShader, 0x809e7012);
	AddWord(&fragmentShader, 0x116049e3); /* nop; mov r3.8c, r2 */
	AddWord(&fragmentShader, 0x159e76c0);
	AddWord(&fragmentShader, 0x30020ba7); /* mov tlbc, r3; nop; thrend */
	AddWord(&fragmentShader, 0x009e7000);
	AddWord(&fragmentShader, 0x100009e7); /* nop; nop; nop */
	AddWord(&fragmentShader, 0x009e7000);
	AddWord(&fragmentShader, 0x500009e7); /* nop; nop; sbdone */

	// Clear colour
	CLEAR_COLOUR(renderControlList, clearColour);

	// Tile rendering mode configuration
	TILE_RENDERING_MODE_CONFIG(renderControlList, framebufferAddress, 1920, 1080);

	//TILE_COORDINATE(renderControlList, 0, 0);
	// Tile buffer general
	AddByte(&renderControlList, 28);
	AddShort(&renderControlList, 0);
	AddWord(&renderControlList, 0);

	// Each tile is 64*64 pixels
	u8 maxYTiles = (1080 / 64) + 1;
	u8 maxXTiles = (1920 / 64) + 1;

	for(x = 0; x < maxXTiles; x++) {
		for(y = 0; y < maxYTiles; y++) {
			// Tile coordinates
			TILE_COORDINATE(renderControlList, x, y);

			// Call tile sublist (this crashes the Pi for some reason)
			//AddByte(&renderControlList, CODE_BRANCH_TO_SUBLIST);
			//AddWord(&renderControlList, addr + 0x6200 + (y * 30 + x) * 32);
			
			// Store multisample
			if(x == maxXTiles - 1 && y == maxYTiles - 1) {
				MULTISAMPLE_END(renderControlList); // Multisample End
			} else {
				MULTISAMPLE_STORE(renderControlList); // Store multisample
			}
		}
	}

	// Run binner control list
	v3d[V3D_CT0CA] = addr;
	v3d[V3D_CT0EA] = addr + 0x80;

	// Wait for execution to complete
	while(v3d[V3D_CT0CS] & 0x20);

	// Set up the V3D pipeline to execute the
	// control list
	v3d[V3D_CT1CA] = addr + 0xe200;
	//v3d[V3D_CT1EA] = (u32)&list;
	v3d[V3D_CT1EA] = (u32)&renderControlList;

	// Wait for execution to complete
	while(v3d[V3D_CT1CS] & 0x20);

	UnlockGPUMemory(handle);
	FreeGPUMemory(handle);

	return true;
}

bool GPURenderTriangle2(u32 framebufferAddress, u32 clearColour) {
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
	u8* shaderRecord = (u8*)addr + 0x1000;
	u8* fragmentShader = (u8*)addr + 0x2000;
	u8* shaderUniforms = (u8*)addr + 0x3000;

	// Clear colour
	CLEAR_COLOUR(list, clearColour);

	// Tile rendering mode configuration
	TILE_RENDERING_MODE_CONFIG(list, framebufferAddress, 1920, 1080);

	CONFIGURATION_STATE(list, (ENABLE_FRONTFACING_PRIMITIVES +
		ENABLE_BACKFACING_PRIMITIVES +
		CLOCKWISE_PRIMITIVES), 0);

	VG_SHADER_STATE(list, (u32)&shaderRecord);

	// Add Shader Record
	AddByte(&shaderRecord, 0);
	AddByte(&shaderRecord, 0);
	AddByte(&shaderRecord, 2);
	AddByte(&shaderRecord, 0);
	// Fragment Shader Code Address
	AddWord(&shaderRecord, (u32)&fragmentShader);
	// Fragment Shader Uniforms Address
	AddWord(&shaderRecord, (u32)&shaderUniforms);
	// ---

	// Write Shader Data
	AddWord(&fragmentShader, 0x958e0dbf);
	AddWord(&fragmentShader, 0xd1724823); /* mov r0, vary; mov r3.8d, 1.0 */
	AddWord(&fragmentShader, 0x818e7176);
	AddWord(&fragmentShader, 0x40024821); /* fadd r0, r0, r5; mov r1, vary */
	AddWord(&fragmentShader, 0x818e7376);
	AddWord(&fragmentShader, 0x10024862); /* fadd r1, r1, r5; mov r2, vary */
	AddWord(&fragmentShader, 0x819e7540);
	AddWord(&fragmentShader, 0x114248a3); /* fadd r2, r2, r5; mov r3.8a, r0 */
	AddWord(&fragmentShader, 0x809e7009);
	AddWord(&fragmentShader, 0x115049e3); /* nop; mov r3.8b, r1 */
	AddWord(&fragmentShader, 0x809e7012);
	AddWord(&fragmentShader, 0x116049e3); /* nop; mov r3.8c, r2 */
	AddWord(&fragmentShader, 0x159e76c0);
	AddWord(&fragmentShader, 0x30020ba7); /* mov tlbc, r3; nop; thrend */
	AddWord(&fragmentShader, 0x009e7000);
	AddWord(&fragmentShader, 0x100009e7); /* nop; nop; nop */
	AddWord(&fragmentShader, 0x009e7000);
	AddWord(&fragmentShader, 0x500009e7); /* nop; nop; sbdone */
	// ---

	// Add Shader Uniforms
	AddWord(&shaderUniforms, 0xFFFFFFFF);
	AddWord(&shaderUniforms, 0xFFFFFFFF);
	// ---

	VG_INLINE_PRIMITIVES_START(list, PRIMITIVE_TYPE_TRIANGLES);

	AddShort(&list, 0); AddShort(&list, 0);
	AddShort(&list, 639); AddShort(&list, 0);
	AddShort(&list, 639); AddShort(&list, 479);

	VG_INLINE_PRIMITIVES_END(list);

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
	v3d[V3D_CT1EA] = addr + 0x4000;

	// Wait for execution to complete
	while(v3d[V3D_CT1CS] & 0x20);

	UnlockGPUMemory(handle);
	FreeGPUMemory(handle);

	return true;
}