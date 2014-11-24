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
	AddByte(&l, 114); \
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
	AddByte(&l, 113); \
	AddWord(&l, fb); \
	AddShort(&l, w); \
	AddShort(&l, h); \
	AddByte(&l, FRAMEBUFFER_MODE_LINEAR_RGBA8888); \
	AddByte(&l, 0x00);

/*
* TILE_COORDINATE macro:
* Add the tile coordinate structure to
* the specified control list. 
*/
#define TILE_COORDINATE(l, x, y) \
	AddByte(&l, 115); \
	AddByte(&l, x); \
	AddByte(&l, y);

/*
* MULTISAMPLE_STORE macro:
* Store the multisample of the previously
* specified tile coordinates.
*/
#define MULTISAMPLE_STORE(l) \
	AddByte(&l, 24);

/*
* MULTISAMPLE_END macro:
* Add the multisample end structure
* to the specified control list.
*/
#define MULTISAMPLE_END(l) \
	AddByte(&l, 25);
