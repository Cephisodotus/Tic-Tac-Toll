#pragma once

#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_image.h>
#include "board.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <math.h>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const enum gameState {mainMenu, ticTacToe};

class GameManager {
public:
	GameManager();
	~GameManager();

	// Main constant update methods
	void tick();
	void renderFrame();
	void input();

	// Interpolation used for size changing
	float lerp(float start, float end, float increment);
	float checkDuration(Board* board);

	// Create a board
	void gameStart(); // Do things that need to happen when the game starts
	bool boardTimerIsReady();
	void spawnBoard();

	// Try filling space
	bool canFillSpace(Board* board, int mouseX, int mouseY);

	// Draw methods
	void draw(Board* board, int adjustedWAndH); // Overload for changing size of board
	void draw(const char* message, int posX, int posY, int r, int g, int b, int size); // Draw overload for text

private:
	gameState currentState;
	
	// Game markers
	const int xMarker = 0;
	const int oMarker = 1;
	int playerLives = 5;

	std::vector<Board*> boardZBuffer;
	std::list<Board*> boardZBufferList;
	std::list<Board*>::iterator zIterator;
	std::list<Board*>::reverse_iterator revZIterator;

	SDL_Renderer* renderer;
	SDL_Window* window;

	// Setting up all the textures used by GameManager
	SDL_Texture* xTexture;
	SDL_Texture* oTexture;
	SDL_Texture* titleTex;
	SDL_Texture* livesTextTex;
	SDL_Texture* num5Tex;
	SDL_Texture* num4Tex;
	SDL_Texture* num3Tex;
	SDL_Texture* num2Tex;
	SDL_Texture* num1Tex;

	// Used for game updates and shutdown
	bool running;
	int count;

	// Handling time and frame counts
	int frameCount, timerFPS, lastFrame;
	float runTime;
	float lastBoardSpawn, spawnInterval;

	// Window size
	const int windowHeight = 600;
	const int windowWidth = 800;

	// Input stuff
	int mouseX, mouseY;
	Board* intersectedBoard(int mouseX, int mouseY);

	// SpeedMod is a difficulty modifier for speeding up board spawning
	float speedMod = 5.0f;
	float speedRamping = 30.0f; // Affects how quickly speed increases (bigger number = slower ramping)

	// Initialize some variables we can reuse
	SDL_Color color;

	// Title
	std::string titleJPG = "sourceImages/title.jpg";

	// Board and Markers
	std::string oPNG = "sourceImages/o.png";
	std::string xPNG = "sourceImages/x.png";
	std::string boardPNG = "sourceImages/board.png";

	std::string oJPG = "sourceImages/o.jpg";
	std::string xJPG = "sourceImages/x.jpg";
	std::string boardJPG = "sourceImages/board.jpg";

	// Lives/Numbers
	std::string livesTextJPG = "sourceImages/lives.jpg";
	std::string fiveJPG = "sourceImages/num5.jpg";
	std::string fourJPG = "sourceImages/num4.jpg";
	std::string threeJPG = "sourceImages/num3.jpg";
	std::string twoJPG = "sourceImages/num2.jpg";
	std::string oneJPG = "sourceImages/num1.jpg";
};

#endif // !GAME_H