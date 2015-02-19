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

typedef enum {
	CLOCKWISE = 0,
	ANTICLOCKWISE
} rot_direction_t;

extern unsigned int GetTickCount(void);
bool MovePiece(direction_t moveDirection);
void RotatePiece(rot_direction_t direction);
void CheckLines(void);

unsigned int boardTick;
int currentPieceX, currentPieceY, pieceRotation, newPieceIndex, pieceColour;
int currentScore;
bool placeNewPiece, gameOver;

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
// Four rotations for four pieces, dimensions 4 in x
// and 2 in y.
int pieces[4][7][4][4] = {
	// <rotation 0>
	{
		{ 
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 0, 1, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		}
	},
	// </rotation 0>
	// <rotation 1>
	{
		{ 
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 }
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		}
	},
	// </rotation 1>
	// <rotation 2>
	{
		{ 
			{ 0, 0, 0, 0 },
			{ 1, 1, 1, 1 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }
		},
		{
			{ 1, 1, 1, 0 },
			{ 0, 0, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 1, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 1, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 1, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 1, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		}
	},
	// </rotation 2>
	// <rotation 3>
	{
		{ 
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 1, 0, 0, 0 }
		},
		{
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 1, 0, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 1, 0, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		},
		{
			{ 0, 1, 0, 0 },
			{ 1, 1, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 0, 0 }	
		}
	},
	// </rotation 3>
};

int pieceWidths[7] = { 4, 3, 3, 2, 3, 3, 3 };
int pieceHeights[7] = { 1, 2, 2, 2, 2, 2, 2 };

void MovePieceDown(void) 
{
	if(placeNewPiece == false) {
		MovePiece(DOWN);
	}
}

void MovePieceLeft(void) 
{
	if(placeNewPiece == false) {
		MovePiece(LEFT);
	}
}

void MovePieceRight(void) 
{
	if(placeNewPiece == false) {
		MovePiece(RIGHT);
	}
}

void DropPiece(void) 
{
	if(placeNewPiece == false) {
		while(MovePiece(DOWN) == true) {

		};

		placeNewPiece = true;
	}
}

void RotatePieceClockwise(void) 
{
	RotatePiece(CLOCKWISE);
}

void RotatePieceAnticlockwise(void) 
{
	RotatePiece(ANTICLOCKWISE);
}

int GetScore(void)
{
	return currentScore;
}

void GameInit(void) 
{
	boardTick = GetTickCount();
	currentPieceY = 0;
	currentPieceX = 3;
	pieceRotation = 0;
	newPieceIndex = 0;
	pieceColour = 1;
	placeNewPiece = false;
	gameOver = false;
	currentScore = 0;

	BindKey('A', MovePieceLeft);
	BindKey('D', MovePieceRight);
	BindKey('S', MovePieceDown);
	BindKey('Z', DropPiece); // TODO: Space bar
	BindKey('Q', RotatePieceAnticlockwise);
	BindKey('E', RotatePieceClockwise);
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
		pieceRotation = 0;
		px = 0;
		py = 0;

		// Check Game Over
		for(x = 0; x < 10; x++) {
			for(y = 0; y < 2; y++) {
				if(gameBoard[y][x] != 0) {
					gameOver = true;
				}
			}
		}

		if(gameOver == true) {
			// Do something for game over
		}

		// Place New Piece
		for(x = currentPieceX; x < (currentPieceX + 4); x++) {
			py = 0;

			for(y = currentPieceY; y < (currentPieceY + 2); y++) {
				gameBoard[y][x] = pieces[pieceRotation][newPieceIndex][py][px];
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
		if(MovePiece(DOWN) == false) {
			placeNewPiece = true;
		}

		boardTick = GetTickCount();
	}
}

void ClearUnsecuredBlocks() 
{
	int x, y;

	// Clear all non-secured pieces from the board
	for(x = 0; x < 10; x++) {
		for(y = 0; y < 24; y++) {
			if(gameBoard[y][x] > 0) {
				gameBoard[y][x] = 0;
			}
		}
	}
}

bool MovePiece(direction_t moveDirection)
{
	int x, y, px, py;
	int moveX, moveY, width, height;

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

	if(pieceRotation % 2 == 0) {
		width = pieceWidths[newPieceIndex];
		height = pieceHeights[newPieceIndex];
	} else {
		width = pieceHeights[newPieceIndex];
		height = pieceWidths[newPieceIndex];
	}

	if((currentPieceX + moveX) < 0 || (currentPieceX + moveX + width) > 10 || (currentPieceY + moveY + height) > 24) {
		return false;
	}

	// Check if a piece is below the current one, 
	// before it is dropped
	for(x = currentPieceX; x < (currentPieceX + 4); x++) {
		for(y = currentPieceY; y < (currentPieceY + 4); y++) {
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

		for(y = currentPieceY; y < (currentPieceY + 4); y++) {
			if((pieces[pieceRotation][newPieceIndex][y][0] > 0 && gameBoard[y][currentPieceX + moveX] < 0) ||
				(pieces[pieceRotation][newPieceIndex][y][width - 1] > 0 && gameBoard[y][currentPieceX + width - 1 + moveX] < 0)) {
				
				return false;
			}
		}
	}

	// Drop piece only if it can be
	// (determined by the loop above)
	if(placeNewPiece == false) {
		ClearUnsecuredBlocks();

		// Place new piece in position
		px = 0;
		py = 0;

		currentPieceX += moveX;
		currentPieceY += moveY;

		for(x = currentPieceX; x < (currentPieceX + 4); x++) {
			py = 0;

			for(y = currentPieceY; y < (currentPieceY + 4); y++) {
				if(gameBoard[y][x] == 0 && pieces[pieceRotation][newPieceIndex][py][px] != 0) {
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
			currentScore++;
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

void RotatePiece(rot_direction_t direction) 
{
	int indexChange = 0;
	int x, y, px, py;
	bool rotationValid;

	switch(direction) {
		case CLOCKWISE:
			indexChange = 1;
			break;

		case ANTICLOCKWISE:
			indexChange = -1;
			break;

		default:
			break;
	}

	if(pieceRotation + indexChange < 0) {
		pieceRotation = 3;
	} else if(pieceRotation + indexChange > 3) {
		pieceRotation = 0;
	} else {
		pieceRotation += indexChange;
	}

	px = 0;
	py = 0;

	// Determine whether the rotation will
	// collide with any secured blocks. If so,
	// the rotation is not valid.
	rotationValid = true;

	for(x = currentPieceX; x < (currentPieceX + 4); x++) {
		py = 0;

		for(y = currentPieceY; y < (currentPieceY + 4); y++) {
			if(rotationValid == true && pieces[pieceRotation][newPieceIndex][py][px] != 0 && gameBoard[y][x] > 0) {
				rotationValid = false;
			}

			py++;
		}

		px++;
	}

	// Place new block if the rotation is valid
	if(rotationValid == true) {
		ClearUnsecuredBlocks();
		px = 0;
		py = 0;

		for(x = currentPieceX; x < (currentPieceX + 4); x++) {
			py = 0;

			for(y = currentPieceY; y < (currentPieceY + 4); y++) {
				if(pieces[pieceRotation][newPieceIndex][py][px] != 0) {
					gameBoard[y][x] = pieceColour;
				}

				py++;
			}

			px++;
		}
	}
}
