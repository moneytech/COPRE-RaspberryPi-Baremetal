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
* RandomNum.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

int MT[623];
int index;


void InitilizeRandomGenerator(int seed)
{
	int i;

	index = 0;
	MT[0] = seed;
	
	for (i = 1; i < 623; i++)
	{ 
		// ^ XOR, >> shift right
		MT[i] = (0x6c078965 * (MT[i-1] ^ (MT[i-1] >> 30)) + i);
		//lowest 32 bits of the number
		MT[i] &= 0xffffffff; 
	}
}

void generateNumbers()
{
	int i, y;
	for (i = 0; i < 624; i++)
	{
		y = (MT[i] & 0x80000000) + (MT[(i + 1) % 624] & 0x7fffffff);//bit 31 of MT[i] + bits 0-30 of MT
		MT[i] = MT[(i + 397) % 624] ^ (y >> 1);
		if ((y % 2) != 0)
		{
			MT[i] = (MT[i] ^ 0x9908b0df);
		}
	}

}

int GetRandomNumber(void)
{
	int y;
	if (index == 0)
	{
		generateNumbers();
	}

	y = MT[index];
	y = y ^ (y >> 11);
	y = y ^ ((y << 7) & (0x9d2c5680));
	y = y ^ ((y << 15) & (0xefc60000));
	y = y ^ (y >> 18);

	index = (index + 1) % 624;
	return y; 
}


