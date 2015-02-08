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
* game.c
* By:
*  Niall Frederick Weedon (nweedon)
*  Tim Stanley (Spike71m)
****************************************************/

#include "../include/csud/types.h"
#include "../include/keyboard.h"

typedef enum {
	LEFT = 0,
	RIGHT,
	DOWN
} direction_t;

extern unsigned int GetTickCount(void);
bool MovePiece(direction_t moveDirection);
void CheckLines(void);

unsigned int boardTick;
int currentPieceX, currentPieceY, newPieceIndex, pieceColour;
bool placeNewPiece;

// Displayed game board is 10*20, extra
// space at the top is allowed for the pieces to
// drop into the game board.
int gameBoard[24][10] = {
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
	{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
}; 
// Four pieces, dimensions 4 in x
// and 2 in y.
int pieces[7][2][4] = {
	{ 
		{ 0, 0, 0, 0 },
		{ 1, 1, 1, 1 }
	},
	{
		{ 1, 0, 0, 0 },
		{ 1, 1, 1, 1 }	
	},
	{
		{ 0, 0, 0, 1 },
		{ 1, 1, 1, 1 }	
	},
	{
		{ 1, 1, 0, 0 },
		{ 1, 1, 0, 0 }	
	},
	{
		{ 1, 1, 0, 0 },
		{ 0, 1, 1, 0 }	
	},
	{
		{ 0, 1, 1, 0 },
		{ 1, 1, 0, 0 }	
	},
	{
		{ 0, 1, 0, 0 },
		{ 1, 1, 1, 0 }	
	}
};

int pieceWidths[7] = { 4, 4, 4, 2, 3, 3, 3 };

void MovePieceDown(void) {
	if(placeNewPiece == false) {
		MovePiece(DOWN);
	}
}

void MovePieceLeft(void) {
	if(placeNewPiece == false) {
		MovePiece(LEFT);
	}
}

void MovePieceRight(void) {
	if(placeNewPiece == false) {
		MovePiece(RIGHT);
	}
}

void DropPiece(void) {
	if(placeNewPiece == false) {
		while(MovePiece(DOWN) == true) {

		};

		placeNewPiece = true;
	}
}

void GameInit(void) 
{
	boardTick = GetTickCount();
	currentPieceY = 0;
	currentPieceX = 3;
	newPieceIndex = 0;
	pieceColour = 1;
	placeNewPiece = false;

	BindKey('A', MovePieceLeft);
	BindKey('D', MovePieceRight);
	BindKey('S', MovePieceDown);
	BindKey('Z', DropPiece); // TODO: Space bar
}

void GameUpdate(void) 
{
	int x, y, px, py;

	if(placeNewPiece == true) {
		// Secure all pieces currently on the board
		for(x = 0; x < 10; x++) { 
			for(y = 0; y < 24; y++) {
				if(gameBoard[y][x] > 0) {
					gameBoard[y][x] *= -1;
				}
			}
		}

		CheckLines();

		// Add a new piece
		currentPieceX = 3;
		currentPieceY = 0;
		px = 0;
		py = 0;

		// TODO: Game over if there are already pieces in the top 2 rows
		for(x = currentPieceX; x < (currentPieceX + 4); x++) {
			py = 0;

			for(y = currentPieceY; y < (currentPieceY + 2); y++) {
				gameBoard[y][x] = pieces[newPieceIndex][py][px];
				py++;
			}

			px++;
		}

		newPieceIndex++;
		pieceColour++;

		if(newPieceIndex == 7) {
			newPieceIndex = 0;
		}

		if(pieceColour > 4) {
			pieceColour = 1;
		}

		placeNewPiece = false;
	}

	if(GetTickCount() > (boardTick + 500000)) {
		// Place piece
		px = 0; 
		py = 0;

		// Drop piece, stop when it hits the bottom
		if(currentPieceY < 22) {
			MovePiece(DOWN);
		} else {
			placeNewPiece = true;
		}

		boardTick = GetTickCount();
	}
}

bool MovePiece(direction_t moveDirection)
{
	int x, y, px, py;
	int moveX, moveY;

	switch(moveDirection) {
		case DOWN:
			moveX = 0;
			moveY = 1;
			break;

		case LEFT:
			moveX = -1;
			moveY = 0;
			break;

		case RIGHT:
			moveX = 1;
			moveY = 0;
			break;

		default:
			moveX = 0;
			moveY = 0;
			break;
	}

	if((currentPieceX + moveX) < 0 || (currentPieceX + moveX + pieceWidths[newPieceIndex]) > 10 || (currentPieceY + moveY) > 22) {
		return false;
	}

	// Check if a piece is below the current one, 
	// before it is dropped
	for(x = currentPieceX; x < (currentPieceX + 4); x++) {
		for(y = currentPieceY; y < (currentPieceY + 2); y++) {
			if(gameBoard[y][x + moveX] > 0 && gameBoard[y + moveY][x + moveX] < 0) {
				placeNewPiece = true;
				return false;
			}
		}
	}

	// Check if a piece is to the left or right of the
	// current one
	if(moveX != 0) {
		int width = pieceWidths[newPieceIndex];

		for(y = currentPieceY; y < (currentPieceY + 2); y++) {
			if((pieces[newPieceIndex][y][0] > 0 && gameBoard[y][currentPieceX + moveX] < 0) ||
				(pieces[newPieceIndex][y][width - 1] > 0 && gameBoard[y][currentPieceX + width - 1 + moveX] < 0)) {
				
				return false;
			}
		}
	}

	// Drop piece only if it can be
	// (determined by the loop above)
	if(placeNewPiece == false) {
		// Clear all non-secured pieces from the board
		for(x = 0; x < 10; x++) {
			for(y = 0; y < 24; y++) {
				if(gameBoard[y][x] > 0) {
					gameBoard[y][x] = 0;
				}
			}
		}

		// Place new piece in position
		px = 0;
		py = 0;

		currentPieceX += moveX;
		currentPieceY += moveY;

		for(x = currentPieceX; x < (currentPieceX + 4); x++) {
			py = 0;

			for(y = currentPieceY; y < (currentPieceY + 2); y++) {
				if(gameBoard[y][x] == 0 && pieces[newPieceIndex][py][px] != 0) {
					gameBoard[y][x] = pieceColour;
				}

				py++;
			}

			px++;
		}
	}

	return true;
}

void CheckLines() 
{
	int x, y, py, totalBlocks;

	for(y = 23; y > 0; y--) {
		totalBlocks = 0;

		for(x = 0; x < 10; x++) {
			if(gameBoard[y][x] < 0) {
				totalBlocks++;
			}
		}

		if(totalBlocks == 10) {
			// TODO: Score

			// Move the upper pieces down
			for(py = (y - 1); py >= 0; py--) {
				for(x = 0; x < 10; x++) {
					gameBoard[py + 1][x] = gameBoard[py][x];
					gameBoard[py][x] = 0;
				}
			}

			// Increment y, so the same row will be checked
			// again next time around.
			y++;
		}
	}
}