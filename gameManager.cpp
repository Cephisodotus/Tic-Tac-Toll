#include "gameManager.h"

GameManager::GameManager() {
	// Make sure everything initializes correctly
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		std::cout << "Could not initialize SDL. Error: " << SDL_GetError() << std::endl;
		system("pause");

		// It's a wash, close the program
		exit(EXIT_FAILURE);
	}
	
	// Initialize JPG system
	int imageFlags = IMG_INIT_JPG;
	if (!(IMG_Init(imageFlags) & imageFlags)) {
		std::cout << "Could not initialize JPG Images. Error: " << SDL_GetError() << std::endl;
		system("pause");

		exit(EXIT_FAILURE);
	}

	// Setup a time-based random variable
	srand(time(NULL));

	// Setup game window
	SDL_CreateWindowAndRenderer(windowWidth, windowHeight, 0, &window, &renderer);
	SDL_SetWindowTitle(window, "Tic-Tac-TOLL-THE-DEAD");

	// Set initial conditions
	currentState = mainMenu; // game state
	running = true; // game is running
	spawnInterval = 4.0f; // Spawn interval for boards

	// Setup image for main menu
	SDL_Surface* surface;
	surface = IMG_Load(titleJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	titleTex = SDL_CreateTextureFromSurface(renderer, surface);

	tick(); // Start ticking the frames
}

GameManager::~GameManager() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit;
}

void GameManager::tick() {
	// Use the game state to check if game is started
	while (running) {
		switch (currentState) {
		case mainMenu:
			break;
		case ticTacToe:
			if (boardTimerIsReady()) {
				spawnBoard();
			}
			break;
		default:
			break;
		}

		renderFrame();
		input();

		lastFrame = SDL_GetTicks();
		runTime = (float)lastFrame / 1000.0f;
	}
}

// Check whether it's ok to spawn another board
bool GameManager::boardTimerIsReady() {
	return runTime - lastBoardSpawn > spawnInterval;
}

// Lerp function for interpolating the size of boards
float GameManager::lerp(float start, float end, float increment) {
	return (start * (1.0f - increment)) + (end * increment);
}

void GameManager::renderFrame() {
	// Set initial draw color (Black) and draw a rectangle
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = windowWidth;
	rect.h = windowHeight;

	SDL_RenderFillRect(renderer, &rect);

	// Rects used for spawning various static images on screen
	SDL_Rect startPos;
	SDL_Rect endPos;

	// Load the main menu image
	if (currentState == mainMenu) {
		startPos.x = 0; startPos.y = 0; startPos.h = 600; startPos.w = 800;
		endPos.x = 0; endPos.y = 0;; endPos.h = 600; endPos.w = 800;
		SDL_RenderCopyEx(renderer, titleTex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
	}

	// Load position and size for lives counter
	startPos.x = 0; startPos.y = 0; startPos.h = 100; startPos.w = 200;
	endPos.x = 0; endPos.y = 500;; endPos.h = 100; endPos.w = 200;
	SDL_RenderCopyEx(renderer, livesTextTex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);

	startPos.x = 0; startPos.y = 0; startPos.h = 100; startPos.w = 100;
	endPos.x = 200; endPos.y = 500;; endPos.h = 100; endPos.w = 100;

	// Really bulky, but decide which "lives" jpg to use
	switch (playerLives) {
	case 0:
		running = false;
		break;
	case 1:
		SDL_RenderCopyEx(renderer, num1Tex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
		break;
	case 2:
		SDL_RenderCopyEx(renderer, num2Tex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
		break;
	case 3:
		SDL_RenderCopyEx(renderer, num3Tex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
		break;
	case 4:
		SDL_RenderCopyEx(renderer, num4Tex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
		break;
	case 5:
		SDL_RenderCopyEx(renderer, num5Tex, &startPos, &endPos, 0, NULL, SDL_FLIP_NONE);
		break;
	default:
		break;
	}

	// Set iterator to beginning of z-buffer and draw
	zIterator = boardZBufferList.begin();

	for (int i = 0; i < boardZBufferList.size(); i++) {
		float incrementVal = checkDuration(*zIterator);
		if (incrementVal <= 1) {
			draw(*zIterator, lerp(0.0f, (*zIterator)->getFinalWidth(), incrementVal));
			zIterator++;
		}
		else {
			boardZBufferList.erase(zIterator);
			playerLives--;
		}
	}

	// Update frame count and FPS
	frameCount++;
	timerFPS = SDL_GetTicks() - lastFrame;

	// Show the frame
	SDL_RenderPresent(renderer);
}

void GameManager::spawnBoard() {
	Board* board = new Board;
	board->initializeTime(speedMod);

	// Create random values for the board's spawn points
	// TODO: Apply some kind of additional modifier based on existing boards to minimize overlap?
	int posX = rand() % (windowWidth - board->getFinalWidth()) + (board->getFinalWidth() / 2);
	int posY = rand() % (windowHeight - board->getFinalHeight()) + (board->getFinalHeight() / 2);

	// Set board initial conditions
	board->setInitialPos(posX, posY);
	board->setPositionStart(0, 0, board->getFinalWidth(), board->getFinalHeight()); // Our source is the whole image
	board->setPositionEnd(posX, posY, 0, 0);
	board->setSprite(boardJPG, renderer);

	// Pass turn to player
	board->setPlayerTurn();

	boardZBufferList.push_front(board);

	// Update last spawn time and increase speed for next board spawn
	lastBoardSpawn = (float)SDL_GetTicks() / 1000;
	speedMod = (runTime + speedRamping) / speedRamping;
}

// Check to see if the board has reached its max duration. Return value > 1 if true, else calculate the increment value to be used for the lerp
float GameManager::checkDuration(Board* board) {
	if (runTime > (board->getTimeStart() + board->getDuration())) {
		return 2.0f;
	}
	else {
		return (runTime - board->getTimeStart()) / board->getDuration();
	}
}

void GameManager::draw(Board* board, int adjustWAndH) {
	// TODO: Grabbing the position values, changing them, then grabbing them again seems like too much.
	//		- May just be trade off for changing the size
	SDL_Rect positionStart = board->getPositionStart();
	SDL_Rect positionEnd = board->getPositionEnd();

	// Use the width/height of the square boards to offset the "spawn" location and keep track of the corners of the board
	int positionOffset = adjustWAndH / 2;
	board->setPositionEnd(board->getInitialX() - positionOffset, board->getInitialY() - positionOffset, adjustWAndH, adjustWAndH);
	board->setCornerCoords(board->getInitialX() - positionOffset, board->getInitialY() - positionOffset, board->getInitialX() + positionOffset, board->getInitialY() + positionOffset);

	positionEnd = board->getPositionEnd();

	// Draw the background rectangle
	SDL_RenderCopyEx(renderer, board->getTexture(), &positionStart, &positionEnd, 0, NULL, SDL_FLIP_NONE);

	int cellUnit = board->getPositionEnd().w / 3;
	positionStart.h = 100;
	positionStart.w = 100;
	positionEnd.h = cellUnit;
	positionEnd.w = cellUnit;

	// Iterate through the board's grid to find which values to render
	for (int i = 0; i < 3; i++) {
		positionEnd.x = board->getLeftX() + (i * cellUnit);
		for (int j = 0; j < 3; j++) {
			positionEnd.y = board->getLeftY() + (j * cellUnit);
			switch (board->getBoardGrid(i, j)) {
			case 0:
				SDL_RenderCopyEx(renderer, xTexture, &positionStart, &positionEnd, 0, NULL, SDL_FLIP_NONE);
				break;
			case 1:
				SDL_RenderCopyEx(renderer, oTexture, &positionStart, &positionEnd, 0, NULL, SDL_FLIP_NONE);
				break;
			default:
				break;
			}
		}
	}
}

void GameManager::draw(const char* message, int posX, int posY, int r, int g, int b, int size) {
	color.r = r;
	color.g = g;
	color.b = b;
}

// Things to do when game starts
void GameManager::gameStart() {
	// Set up all the images used during runtime
	SDL_Surface* surface;
	surface = IMG_Load(xJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	xTexture = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(oJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	oTexture = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(livesTextJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	livesTextTex = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(fiveJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	num5Tex = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(fourJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	num4Tex = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(threeJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	num3Tex = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(twoJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	num2Tex = SDL_CreateTextureFromSurface(renderer, surface);

	surface = IMG_Load(oneJPG.c_str());
	if (!surface) {
		std::cout << "no image, bud: " << IMG_GetError();
	}
	num1Tex = SDL_CreateTextureFromSurface(renderer, surface);


	// Set state to tictactoe and spawn first board
	currentState = ticTacToe;
	spawnBoard();
}

Board* GameManager::intersectedBoard(int mouseX, int mouseY) {
	// Make sure zBuffer has stuff before trying to do stuff with the board
	if (boardZBufferList.size() == 0) {
		return NULL;
	}
	
	// In order to check stuff at the front FIRST, we need to reverse the ZBuffer
	revZIterator = boardZBufferList.rbegin();

	int x1 = (*revZIterator)->getLeftX();
	int x2 = (*revZIterator)->getRightX();
	int y1 = (*revZIterator)->getLeftY();
	int y2 = (*revZIterator)->getRightY();

	while (revZIterator != boardZBufferList.rend()) {
		if (x1 < mouseX && mouseX < x2) {
			if (y1 < mouseY && mouseY < y2) {
				return *revZIterator;
			}
		}
		revZIterator++;
	}
	return NULL;
}

// Checking whether the board has a space to be filled at the player's mouseX and mouseY location at time of click
bool GameManager::canFillSpace(Board* board, int mouseX, int mouseY) {
	int xIndex;
	int yIndex;
	int width = board->getPositionEnd().w;
	int boardX = board->getLeftX();
	int boardY = board->getLeftY();
	int cellWidth = width / 3; // Separate board by thirds for coordinate checking

	// Handling the boards position offset against the draw image offset
	if (mouseX < boardX + cellWidth) {
		xIndex = 0;
	}
	else if (mouseX < boardX + cellWidth + cellWidth) {
		xIndex = 1;
	}
	else {
		xIndex = 2;
	}

	if (mouseY < boardY + cellWidth) {
		yIndex = 0;
	}
	else if (mouseY < boardY + cellWidth + cellWidth) {
		yIndex = 1;
	}
	else {
		yIndex = 2;
	}
	
	// Set the marker
	if (board->getBoardGrid(xIndex, yIndex) == -1) {
		board->setBoardMarker(xIndex, yIndex, xMarker);
		board->incrementTurn();
		board->setAITurn();
		return true;
	}
	
	return false; // Can't set marker in that location
}


// Handling user input
void GameManager::input() {
	SDL_Event inputEvent;
	while (SDL_PollEvent(&inputEvent)) {
		// Quit on ESCAPE
		if (inputEvent.type == SDL_QUIT) {
			running = false;
		}
		// Keydown Events
		if (inputEvent.type == SDL_KEYDOWN) {
			switch (currentState) {
			case mainMenu:
				gameStart();
				break;
			case ticTacToe:
				if (inputEvent.key.keysym.sym == SDLK_ESCAPE) { running = false; }
				break;
			default:
				std::cout << "Not in a state for some reason" << std::endl;
				break;
			}
		}
		// Mouse down events
		if (inputEvent.type == SDL_MOUSEBUTTONDOWN) {
			switch (currentState) {
			case mainMenu:
				gameStart();
				break;
			case ticTacToe:
				if (inputEvent.button.button == SDL_BUTTON_LEFT) {
					Board* decideBoard = intersectedBoard(mouseX, mouseY);
					if (decideBoard != NULL) {
						// Only fill grid if it's the player's turn on that grid
						if (decideBoard->getBoardTurn() == playerTurn) {
							canFillSpace(decideBoard, mouseX, mouseY);
							if (decideBoard->canDecideNextMove()) {}
							else {
								// Find the completed board and delete it from the list
								zIterator = boardZBufferList.begin();
								int i = 0;
								while (i < boardZBufferList.size()) {
									if ((*zIterator)->getInitialX() == decideBoard->getInitialX()) {
										if ((*zIterator)->getBoardResult() == loss) {
											playerLives--;
										}

										boardZBufferList.erase(zIterator);
										zIterator = boardZBufferList.begin();
									}
									if (zIterator != boardZBufferList.end()) {
										zIterator++;
									}
									i++;
								}
							}
						}
					}
				}
				break;
			default:
				break;
			}
		}
		SDL_GetMouseState(&mouseX, &mouseY); //update locations of mouse
	}
}