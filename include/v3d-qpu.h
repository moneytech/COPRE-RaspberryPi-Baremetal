/****************************************************
* v3d-qpu.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/types.h"

extern bool TestMailboxPropertyInterface(void);
extern bool InitV3D(void);
extern bool GPUClearScreen(u32 framebufferAddress);

// V3D Defines
#define V3D_IDENT0 0
// Control List Executor Thread n: control and status
#define V3D_CT0CS (0x100 >> 2)
#define V3D_CT1CS (0x104 >> 2)
// Control List Executor Thread n: current address
#define V3D_CT0CA (0x110 >> 2)
#define V3D_CT1CA (0x114 >> 2)
// Control List Executor Thread n: end address
#define V3D_CT0EA (0x108 >> 2)
#define V3D_CT1EA (0x10C >> 2)

// Flags for memory allocation
enum {
	MEM_FLAG_DISCARDABLE = 1 << 0, /* can be resized to 0 at any time. Use for cached data */
	MEM_FLAG_NORMAL = 0 << 2, /* normal allocating alias. Don't use from ARM */
	MEM_FLAG_DIRECT = 1 << 2, /* 0xC alias uncached */
	MEM_FLAG_COHERENT = 2 << 2, /* 0x8 alias. Non-allocating in L2 but coherent */
	MEM_FLAG_L1_NONALLOCATING = (MEM_FLAG_DIRECT | MEM_FLAG_COHERENT), /* Allocating in L2 */
	MEM_FLAG_ZERO = 1 << 4, /* initialise buffer to all zeros */
	MEM_FLAG_NO_INIT = 1 << 5, /* don't initialise (default is initialise to all ones */
	MEM_FLAG_HINT_PERMALOCK = 1 << 6, /* Likely to be locked for long periods of time. */
};