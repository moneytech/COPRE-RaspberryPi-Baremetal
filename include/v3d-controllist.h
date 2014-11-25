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
* v3d-controllist.h
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/v3d-list.h"

// Reference: Page 66 Videocore IV Reference Arch
#define CODE_HALT 0x0
#define CODE_NOP 0x1
// 2 - 3 reserved
#define CODE_FLUSH 0x4
#define CODE_FLUSH_ALL 0x5
#define CODE_START_TILE_BINNING 0x6
#define CODE_INCREMENT_SEMAPHORE 0x7
#define CODE_WAIT_ON_SEMAPHORE 0x8
// 9 - 15 reserved
#define CODE_BRANCH 0x10
#define CODE_BRANCH_TO_SUBLIST 0x11
#define CODE_RETURN_FROM_SUBLIST 0x12
// 19 - 23 reserved
#define CODE_STORE_MULTISAMPLE 0x18
#define CODE_STORE_MULTISAMPLE_END_OF_FRAME 0x19
#define CODE_STORE_FULL_RES_TILEBUFFER 0x1A
#define CODE_RELOAD_FULL_RES_TILEBUFFER 0x1B
// 30 - 31 reserved
#define CODE_INDEXED_PRIMITIVE_LIST 0x20
#define CODE_VERTEX_ARRAY_PRIMITIVES 0x21
// 34 - 40 reserved
#define CODE_VG_COORDINATE_ARRAY_PRIMITIVES 0x29
#define CODE_VG_INLINE_PRIMITIVES 0x2A
// 43 - 47 reserved
#define CODE_COMPRESSED_PRIMITIVE_LIST 0x30
#define CODE_CLIPPED_PRIMITIVE_WITH_COMPRESSED_PRIMITIVE_LIST 0x31
// 50 - 55 reserved
#define CODE_PRIMITIVE_LIST_FORMAT 0x38
// 57 - 63 reserved
#define CODE_GL_SHADER_STATE 0x40
#define CODE_NV_SHADER_STATE 0x41
#define CODE_VG_SHADER_STATE 0x42
#define CODE_VG_INLINE_SHADER_RECORD 0x43
// 68 - 95 reserved
#define CODE_CONFIGURATION_BITS 0x60
#define CODE_FLAT_SHADE_FLAGS 0x61
#define CODE_POINTS_SIZE 0x62
#define CODE_LINE_WIDTH 0x63
#define CODE_RHT_X_BOUNDARY 0x64
#define CODE_DEPTH_OFFSET 0x65
#define CODE_CLIP_WINDOW 0x66
#define CODE_VIEWPORT_OFFSET 0x67
#define CODE_Z_MIN_MAX 0x68
#define CODE_CLIPPER_XY_SCALING 0x69
#define CODE_CLIPPER_Z_SCALE 0x6A
// 107 - 111 reserved
#define CODE_TILE_BINNING_MODE_CONFIGURATION 0x70
#define CODE_TILE_RENDERING_MODE_CONFIGURATION 0x71
#define CODE_CLEAR_COLOURS 0x72
#define CODE_TILE_COORDINATES 0x73
// 116 - 255 reserved

// CODE_CONFIGRATION_BITS defines (page 69)
// 24 bits, top 6 bits unused
#define EARLY_Z_UPDATE (1 << 17)
#define EARLY_Z_ENABLE (1 << 16)
#define Z_UPDATES_ENABLE (1 << 15)
#define DEPTH_TEST_NEVER (0 << 12)
#define DEPTH_TEST_LESS_THAN (8 << 12)
#define DEPTH_TEST_EQUAL (9 << 12)
#define DEPTH_TEST_LESS_EQUAL (10 << 12)
#define DEPTH_TEST_GREATER (11 << 12)
#define DEPTH_TEST_NOT_EQUAL (12 << 12)
#define DEPTH_TEST_GREATER_EQUAL (13 << 12)
#define DEPTH_TEST_ALWAYS (14 << 12)
#define COVERAGE_CLEAR_ON_READ (0 << 11)
#define COVERAGE_LEAVE_ON_READ (1 << 11)
#define COVERAGE_UPDATE_MODE_NONZERO (0 << 9)
#define COVERAGE_UPDATE_MODE_ODD (1 << 9)
#define COVERAGE_UPDATE_MODE_OR (2 << 9)
#define COVERAGE_UPDATE_MODE_ZERO (3 << 9)
#define COVERAGE_PIPE_SELECT (1 << 8)
#define RASTERISER_OVERSAMPLE_MODE_NONE (0 << 6)
#define RASTERISER_OVERSAMPLE_MODE_4X (1 << 6)
#define RASTERISER_OVERSAMPLE_MODE_16X (2 << 6)
#define RASTERISER_OVERSAMPLE_MODE_RESERVED (3 << 6)
#define COVERAGE_READ_TYPE_4x8_BIT_LEVEL (0 << 5)
#define COVERAGE_READ_TYPE_16BIT_MASK (1 << 5)
// Antialiased points and lines not supported,
// as such defines are not included.
#define DEPTH_OFFSET_ENABLE (1 << 3)
#define ANTICLOCKWISE_PRIMITIVES (0 << 2)
#define CLOCKWISE_PRIMITIVES (1 << 2)
#define ENABLE_BACKFACING_PRIMITIVES (1 << 1)
#define ENABLE_FRONTFACING_PRIMITIVES (1 << 0)

/*
* CLEAR_COLOUR macro:
* Add the clear colour structure to the
* control list specified.
* Notes:
*
* Colour has to be repeated twice
* for 32-bit colour
*/
#define CLEAR_COLOUR(l, colour) \
	AddByte(&l, CODE_CLEAR_COLOURS); \
	AddWord(&l, colour); \
	AddWord(&l, colour); \
	AddWord(&l, 0); \
	AddByte(&l, 0);

#define FRAMEBUFFER_MODE_LINEAR_RGBA8888 (u8)0x04

/*
* TILE_RENDERING_MODE_CONFIG macro:
* Add the tile rendering mode configuration
* to the specified control list.
*/
#define TILE_RENDERING_MODE_CONFIG(l, fb, w, h) \
	AddByte(&l, CODE_TILE_RENDERING_MODE_CONFIGURATION); \
	AddWord(&l, fb); \
	AddShort(&l, w); \
	AddShort(&l, h); \
	AddByte(&l, FRAMEBUFFER_MODE_LINEAR_RGBA8888); \
	AddByte(&l, 0x00);

#define TILE_BINNING_CONFIGURATION(l, tile_alloc_addr, tile_len, tilestate_alloc_addr, tiles_x, tiles_y) \
	AddByte(&l, CODE_TILE_BINNING_MODE_CONFIGURATION); \
	AddWord(&l, tile_alloc_addr); \
	AddWord(&l, tile_len); \
	AddWord(&l, tilestate_alloc_addr); \
	AddByte(&l, tiles_x); \
	AddByte(&l, tiles_y); \
	AddByte(&l, 0x04);

/*
* TILE_COORDINATE macro:
* Add the tile coordinate structure to
* the specified control list. 
*/
#define TILE_COORDINATE(l, x, y) \
	AddByte(&l, CODE_TILE_COORDINATES); \
	AddByte(&l, x); \
	AddByte(&l, y);

/*
* MULTISAMPLE_STORE macro:
* Store the multisample of the previously
* specified tile coordinates.
*/
#define MULTISAMPLE_STORE(l) \
	AddByte(&l, CODE_STORE_MULTISAMPLE);

/*
* MULTISAMPLE_END macro:
* Add the multisample end structure
* to the specified control list.
*/
#define MULTISAMPLE_END(l) \
	AddByte(&l, CODE_STORE_MULTISAMPLE_END_OF_FRAME);

#define START_TILE_BINNING(l) \
	AddByte(&l, CODE_START_TILE_BINNING);

#define PRIMITIVE_LIST_FORMAT(l, format) \
	AddByte(&l, CODE_PRIMITIVE_LIST_FORMAT); \
	AddByte(&l, format);

#define CLIP_WINDOW(l, x, y, w, h) \
	AddByte(&l, CODE_CLIP_WINDOW); \
	AddShort(&l, x); \
	AddShort(&l, y); \
	AddShort(&l, w); \
	AddShort(&l, h);

#define CONFIGURATION_STATE(l, config) \
	AddByte(&l, CODE_CONFIGURATION_BITS); \
	AddShort(&l, ((config & 0x00FF0000) >> 16)); \
	AddShort(&l, ((config & 0x0000FF00) >> 8)); \
	AddShort(&l, ((config & 0x000000FF) >> 0));

#define VIEWPORT_OFFSET(l, x_offset, y_offset) \
	AddByte(&l, CODE_VIEWPORT_OFFSET); \
	AddShort(&l, x_offset); \
	AddShort(&l, y_offset);

#define VERTEX(l, fx, fy, fz, r, g, b) \
	AddShort(&l, (fx << 4)); \
	AddShort(&l, (fy << 4)); \
	AddFloat(&l, (fz << 4)); \
	AddFloat(&l, 1.0f); \
	AddFloat(&l, r); \
	AddFloat(&l, g); \
	AddFloat(&l, b);
