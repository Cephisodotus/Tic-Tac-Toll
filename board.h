#pragma once

#ifndef BOARD_H
#define BOARD_H

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <time.h>

const enum turn { aITurn, playerTurn };
const enum boardResult {win, loss};

class Board {
private:
	// TODO: CHANGE "START" and "END" to SOURCE and DESTINATION. SOURCE REPRESENTS THE SOURCE IMAGE!
	// Keeping track of the x/y position of the board
	SDL_Rect positionStart;
	SDL_Rect positionEnd;

	// Initializing the initial size/position variables and the final size for boards
	int initialX, initialY, finalX, finalY = 0;
	int leftX, leftY, rightX, rightY;
	const int finalW = 300;
	const int finalH = 300;

	SDL_Texture* boardTexture;

	// Game markers
	const int xMarker = 0;
	const int oMarker = 1;
	turn boardTurn;
	int turnCount = 0;
	int maxTurnCount = 9;
	boardResult result;

	// Time variables
	float duration = 5.0f;
	float timeStart, timeEnd;

	// All possible winning solutions
	const int solutions[8][3][2] = { { {0,2}, {1,2}, {2,2} },
									 { {0,1}, {1,1}, {2,1} },
									 { {0,0}, {1,0}, {2,0} },
									 { {0,0}, {1,1}, {2,2} },
									 { {0,0}, {0,1}, {0,2} },
									 { {1,0}, {1,1}, {1,2} },
									 { {2,0}, {2,1}, {2,2} },
									 { {2,0}, {1,1}, {0,2} } };

	std::vector<std::vector<std::vector<int>>> solutionsVectors { { {0,2}, {1,2}, {2,2} },
																 { {0,1}, {1,1}, {2,1} },
																 { {0,0}, {1,0}, {2,0} },
																 { {0,0}, {1,1}, {2,2} },
																 { {0,0}, {0,1}, {0,2} },
																 { {1,0}, {1,1}, {1,2} },
																 { {2,0}, {2,1}, {2,2} },
																 { {2,0}, {1,1}, {0,2} } };

	// This map's the solution indices with their frequency
	std::unordered_map<int, int> solnPriorityMap;
	//std::unordered_map<std::vector<int>, int> emptySqrPriorityMap;
	std::vector<std::vector<int>> aIMovePriority;

	// Number of solutions
	const int solutionsTotal = 8;
	const int lineLength = 3;

	// This is the array we will pull starting positions from using a MOD equation
	const int positions[9][2] = { {0,0}, {1,0}, {2,0},
								  {0,1}, {1,1}, {2,1},
								  {0,2}, {1,2}, {2,2} };

	// The grid used when setting up the board. -1 indicates an empty space
	int boardGrid[3][3] = { {-1, -1, -1},
							{-1, -1, -1},
							{-1, -1, -1} };

	// Grid used for public calls and game visuals
	int grid[3][3];

public:
	Board() {}

	// AI logic and calculator
	bool canDecideNextMove();

	// Initialize Values
	void setInitialPos(int x, int y) { initialX = x; initialY = y; }
	void initializeTime(float speedMod);

	// Getters and Setters
	int getInitialX() const { return initialX; }
	int getInitialY() const { return initialY; }
	int getLeftX() const { return leftX; }
	int getLeftY() const { return leftY; }
	int getRightX() const { return rightX; }
	int getRightY() const { return rightY; }
	SDL_Rect getPositionStart() const { return positionStart; }
	SDL_Rect getPositionEnd() const { return positionEnd; }
	SDL_Texture* getTexture() const { return boardTexture; }
	int getFinalWidth() const { return finalW; }
	int getFinalHeight() const { return finalH; }
	float getTimeStart() const { return timeStart; }
	float getTimeEnd() const { return timeEnd; }
	float getDuration() const { return duration; }
	int getBoardGrid(int x, int y) const { return boardGrid[x][y]; }
	turn getBoardTurn() const { return boardTurn; }
	boardResult getBoardResult() const { return result; }
	int getTurnCount() const { return turnCount; }

	void setPositionStart(int x, int y, int w, int h);
	void setPositionEnd(int x, int y, int w, int h);
	void setCornerCoords(int x1, int y1, int x2, int y2);
	void setSprite(std::string fileName, SDL_Renderer* renderer);
	void setBoardMarker(int x, int y, int marker);
	void setAITurn() { boardTurn = aITurn; }
	void setPlayerTurn() { boardTurn = playerTurn; }
	void incrementTurn() { turnCount++;
	std::cout << turnCount << " turn(s) have passed" << std::endl;
	}
	// TODO: Get the current grid
};

#endif // !BOARD_H