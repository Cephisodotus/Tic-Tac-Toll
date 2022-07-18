#include "board.h"
//#include <chrono.h>

/*


	THINGS TO REMEMBER:
	- If using coordinates to represent the CENTER of the board, subtract w/2 and h/2 from position
		- This point ALSO applies to the sprites for the X's and O's
	- Use the board's x/y coordinates to help inform sprite locations for X's and O's
			- Use 1/3rd the w/h and multiply it by the array index to get easy reference to cell coordinates
			
	- Use a z-buffer to help inform which boards to render last, and which are targeted by user when clicking

*/

void Board::setPositionStart(int x, int y, int w, int h) {
	positionStart.x = x;
	positionStart.y = y;
	positionStart.w = w;
	positionStart.h = h;
}

void Board::setPositionEnd(int x, int y, int w, int h) {
	positionEnd.x = x;
	positionEnd.y = y;
	positionEnd.w = w;
	positionEnd.h = h;
}

// Corner coords used for comparing mouseX and mouseY
void Board::setCornerCoords(int x1, int y1, int x2, int y2) {
	leftX = x1;
	leftY = y1;
	rightX = x2;
	rightY = y2;
}

void Board::initializeTime(float speedMod) {
	timeStart = (float)SDL_GetTicks() / 1000;
	timeEnd = timeStart + (duration * (1 / speedMod));
}

// Sets the sprite for the board
void Board::setSprite(std::string fileName, SDL_Renderer* renderer) {
	SDL_Surface* surface;
	surface = IMG_Load("sourceImages/board.jpg");
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	boardTexture = SDL_CreateTextureFromSurface(renderer, surface);
}

// Sets a marker value on the board
void Board::setBoardMarker(int x, int y, int marker) {
	boardGrid[x][y] = marker;
}

bool Board::canDecideNextMove() {
	int xInLineCount = 0;
	int oInLineCount = 0;
	std::vector<int> prioritySquare;

	// Handling potential high-priority lines

	// Iterate through the solution lines and cross-reference current board for potential win conditions
	for (int i = 0; i < solutionsTotal; i++) {
		for (int j = 0; j < lineLength; j++) {
			// Increment different line counts based on what markers exist in line
			//		xMarker = 0 , oMarker = 1
			switch (boardGrid[solutions[i][j][0]][solutions[i][j][1]]) {
			case -1: // Empty space
				prioritySquare = { solutions[i][j][0], solutions[i][j][1] };
				break;
			case 0: // X Marker
				xInLineCount++;
				break;
			case 1: // O Marker
				oInLineCount++;
				break;
			default:
				break;
			}
		}

		if (xInLineCount == 3) {
			// Player wins, award points then return false
			result = win;
			return false;
		}
		else if (turnCount == maxTurnCount) {
			// Board is full but the player isn't a winner
			result = loss;
			return false;
		}
		else if (oInLineCount == 2 && xInLineCount == 0) {
			// Move here, AI wins
			boardGrid[prioritySquare[0]][prioritySquare[1]] = oMarker;
			incrementTurn(); // Increment the turn count
			result = loss;
			return false;
		}
		else if (xInLineCount == 2 && oInLineCount == 0) {
			// If the amount of x's in this line is equal to or greater than the max, push this spot onto priority map or increment it if it already exists
			// The priority square is the only place in solution line that could be moved to
			aIMovePriority.push_back(prioritySquare);
			boardGrid[prioritySquare[0]][prioritySquare[1]] = oMarker;
			incrementTurn(); // Increment the turn count
			// Pass turn back to player
			setPlayerTurn();
			return true;
		}
		else {
			// No pressing need, push an empty square onto low priority stack
			aIMovePriority.push_back(prioritySquare);
		}

		// Reset the line counters for a new solution
		xInLineCount = 0;
		oInLineCount = 0;
	}

	srand(time(NULL));

	// Create a random number based on the number of positions in the priority vector
	int randomSquare = rand() % aIMovePriority.size();
	boardGrid[aIMovePriority[randomSquare][0]][aIMovePriority[randomSquare][1]] = 1; // Pick a random low-priority square to place a circle
	aIMovePriority.clear(); // Clear this turns AI queue
	incrementTurn(); // Increment the turn count
	if (turnCount >= 9) {
		result = loss;
		return false;	
	}
	setPlayerTurn(); // Pass turn to player

	return true;
}