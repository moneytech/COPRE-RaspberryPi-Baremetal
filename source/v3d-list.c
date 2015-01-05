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
* v3d-list.c
* V3D list helper by phire/hackdriver,
* coverted to this kernel's typings.
*
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/v3d-list.h"

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

void AddByte(u8 **list, u8 d) {
  *((*list)++) = d;
}

union {
	float f;
	u32 i;
} floatToInt;

void AddFloat(u8 **list, float f) {
	floatToInt.f = f;
	u32 d = *((u32 *)&f);
	//u32 d = floatToInt.i;
	*((*list)++) = (d) & 0xff;
	*((*list)++) = (d >> 8)  & 0xff;
	*((*list)++) = (d >> 16) & 0xff;
	*((*list)++) = (d >> 24) & 0xff;
}