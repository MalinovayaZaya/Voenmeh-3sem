/* ����� (���� ������ ��� ���� �������) */

// main - 184 ������

///////////////////////////////////////////////////
///												///
///			 TO DO LIST							///
///			 1. ����							///
///			 2. ����� ������					///
///												///
///////////////////////////////////////////////////


#include <iostream>
#include <SDL.h>

#include "Square.h"

// ����������� ��������� ����
static enum class GameState
{
	OUTLINE_CURRENT_PLAYER,
	SELECTING,
	SELECTED,
	MOVING,
};

// ����������� ����������, �� ���� �����
static enum class Winner
{
	RED,
	BLACK,
	DRAW,
};

// �������� ������ 1D ����� 8 x 8 
static inline int getIndex(const int row, const int col)
{
	return 8 * row + col;
}

// �������� ������ �� ������� �������� ����
static inline bool isOutOfBounds(const int row, const int col)
{
	if (row < 0 || col < 0 || row > 7 || col > 7) return true;
	else return false;
}

// �������� ������� � �������� ������ ����
static SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filePath)
{
	SDL_Texture* texture = nullptr;

	// �������� �������� �� ���� ����
	SDL_Surface* loadedSurface = SDL_LoadBMP(filePath);
	if (loadedSurface == nullptr)
	{
		std::cout << "SDL could not load image into surface! Error: " << SDL_GetError() << std::endl;
	}
	else
	{
		// Color key image (������� ���/�����)
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 255, 255));

		// ������� ��������
		texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (texture == nullptr)
		{
			std::cout << "SDL could not create texture from surface! Error: " << SDL_GetError() << std::endl;
		}

		// ����������� surface
		SDL_FreeSurface(loadedSurface);
	}

	return texture;
}

// ���������� ������� �����
static CheckerType primeChecker(CheckerType checker)
{
	if (checker == CheckerType::RED || checker == CheckerType::KING_RED) return CheckerType::RED;
	else if (checker == CheckerType::BLACK || checker == CheckerType::KING_BLACK) return CheckerType::BLACK;
	else return checker;
}

// ����������, �� ��� ������� ������
static CheckerType oppositeChecker(CheckerType checker)
{
	if (checker == CheckerType::RED || checker == CheckerType::KING_RED) return CheckerType::BLACK;
	if (checker == CheckerType::BLACK || checker == CheckerType::KING_BLACK) return CheckerType::RED;
}

// Return true if arguments are opposite checkers
static bool areOppositeCheckers(CheckerType checker1, CheckerType checker2)
{	
	if (checker1 == CheckerType::NONE || checker2 == CheckerType::NONE) return false;
	else if (oppositeChecker(checker1) != oppositeChecker(checker2)) return true;
	else return false;
}

// ���������� ����������� ����
static struct Direction
{
	int deltaRow;
	int deltaCol;
	Direction(int inDeltaRow, int inDeltaCol) : deltaRow(inDeltaRow), deltaCol(inDeltaCol) {}

}northWest(-1, -1), northEast(-1, 1), southWest(1, -1), southEast(1, 1);

// ���������� �������� ��� �������� ����������� ����
static void checkDirection(Square grid[], const Direction& direction, const CheckerType checker, const int row, const int col, const bool jumped)
{
	int nextRow = row + direction.deltaRow;
	int nextCol = col + direction.deltaCol;
	// ��������� ������� �������� � ����
	if (!isOutOfBounds(nextRow, nextCol))
	{
		Square* squareAtDirection = &grid[getIndex(nextRow, nextCol)];
		// ���� ������� ����� ����������
		if (areOppositeCheckers(checker, squareAtDirection->getChecker()))
		{
			int nextRow2 = row + 2 * direction.deltaRow;
			int nextCol2 = col + 2 * direction.deltaCol;
			// ���� ������� �� ������� �� ������� ����
			if (!isOutOfBounds(nextRow2, nextCol2))
			{
				Square* nextSquareAtDirection = &grid[getIndex(nextRow2, nextCol2)];
				// ���� ��������� �� ����������� ������� ���� � �� ������� ��� ��������� ��� ����
				if (nextSquareAtDirection->getChecker() == CheckerType::NONE && nextSquareAtDirection->getHighlight() != HighlightType::NEXT_POSSIBLE_POSITION)
				{
					std::cout << nextRow2 << ", " << nextCol2 << std::endl;

					// �������� ��������� ��������� ���
					nextSquareAtDirection->setHighlightTo(HighlightType::NEXT_POSSIBLE_POSITION);

					// �������� ����� ��� ���������� ��������
					nextSquareAtDirection->flagChecker(squareAtDirection);

					// Store the square would occur from
					nextSquareAtDirection->setPreviousSquare(&grid[getIndex(row, col)]);

					// ������� ��������� ����������� ����
					Direction directions[4] = { northWest, northEast, southWest, southEast };

					// ���������� ��������� ��������, � ����������� �� �����
					// ���� ����� �����
					if (checker == CheckerType::RED)
					{
						// ��������� north west � north east
						for (int i = 0; i < 2; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
					// ���� ������ �����
					else if (checker == CheckerType::BLACK)
					{
						// ��������� south west � south east
						for (int i = 2; i < 4; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
					// ����� ��������� �����
					else
					{
						// ��������� ��� �����������
						for (int i = 0; i < 4; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
				}
			
				// ���� ��������� ����������� � �������� - ��� ��������� ������� (����� ����� ��������� � �������� ���������)
			}
		}
		// Else if square at direction is empty and no jumps have occured
		else if (squareAtDirection->getChecker() == CheckerType::NONE && !jumped)
		{
			squareAtDirection->setHighlightTo(HighlightType::NEXT_POSSIBLE_POSITION);
		}
	}

}

int main(int argc, char* argv[])
{
	// ����������� ������� ����
	const int windowWidth = 800;
	const int windowHeight = windowWidth;

	// ������������� SDL video subsystem (���� �� ��������� ������� ��� ����� �������)
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL could not intialise! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// �������� ����
	SDL_Window* window = SDL_CreateWindow("Checkers", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr)
	{
		std::cout << "SDL could not create window! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	//The surface contained by the window
	SDL_Surface* mScreenSurface = NULL;
	//The image we will load and show on the screen
	SDL_Surface* mAbout = SDL_LoadBMP("assets/ABOUT.bmp");

	// �������� ������� ���� (������� ����)
	SDL_Window* window2 = SDL_CreateWindow("About", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window2 == nullptr)
	{
		std::cout << "SDL could not create window! Error: " << SDL_GetError() << std::endl;
		return -1;
	}
	else
	{
		//Get window surface
		mScreenSurface = SDL_GetWindowSurface(window2);
	}

	//Apply the image
	SDL_BlitSurface(mAbout, NULL, mScreenSurface, NULL);
	//Update the surface
	SDL_UpdateWindowSurface(window2);

	// �������� renderer'�
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr)
	{
		std::cout << "SDL could not create renderer! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// ��������� ������� �������� � �������� ������� ������ ����
	Checker::sOutline = loadTexture(renderer, "assets/Outline.bmp");

	// �������� ������� ����� � �������� ������� ������ ����
	Checker::sTextureCache[(int)CheckerType::NONE] = loadTexture(renderer, "assets/Empty.bmp");
	Checker::sTextureCache[(int)CheckerType::RED] = loadTexture(renderer, "assets/RedChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::BLACK] = loadTexture(renderer, "assets/BlackChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::KING_RED] = loadTexture(renderer, "assets/KingRedChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::KING_BLACK] = loadTexture(renderer, "assets/KingBlackChecker.bmp");

	// ����������� highlight'�� (RGBA)
	Highlight::sColourCache[(int)HighlightType::NONE] = { 0, 0, 0, 0 };
	Highlight::sColourCache[(int)HighlightType::MOUSE_OVER] = { 94, 152, 235, 150 }; // blue
	Highlight::sColourCache[(int)HighlightType::MOVABLE] = { 255, 255, 255, 150}; // white
	Highlight::sColourCache[(int)HighlightType::SELECTED] = { 94, 152, 235, 150 }; // blue
	Highlight::sColourCache[(int)HighlightType::NEXT_POSSIBLE_POSITION] = { 41, 242, 45, 120 }; // green

	// ��������� ���� renderer'��
	Checker::sRenderer = renderer;
	Highlight::sRenderer = renderer;
	Square::sRenderer = renderer;

	// �������� ����� ��������� 8 x 8 (�� 10 � 10 ?)
	const int rows = 8;
	const int cols = 8;
	const int totalSquares = rows * cols;
	Square checkerboard[totalSquares];

	// ��������� ���� ������������ ����� (��� ��������� �����)
	SDL_Color backgrounds[2];
	backgrounds[0] = { 133, 71, 0, 255 };		// dark brown
	backgrounds[1] = { 230, 186, 127, 255 };	// bright brown

	// ����������� ������ � ��������� �������� �����������
	const int borderThickness = 5;
	const int numberOfBorders = cols + 1;
	const int squareWidth = (windowWidth - numberOfBorders * borderThickness) / cols;
	const int squareHeight = squareWidth;

	// ����������� ��������� ��� ������������ ����� �� ����
	int blackCounter = 12;
	int redCounter = 12;
	int flipCounter = 1;
	int index = 0;

	// ��������� ������������ ���� ���������, ��������, ������������ ����� � ����� �����
	int squareStartY = borderThickness;
	for (int row = 0; row < rows; ++row)
	{
		int squareStartX = borderThickness;
		for (int col = 0; col < cols; ++col)
		{
			//  ��������� ������������ ���� ��������
			checkerboard[index].setLocation(row, col);

			// �������������� ���� ����
			flipCounter = 1 - flipCounter;
			checkerboard[index].setBackgroundColourTo(backgrounds[flipCounter]);

			// ��������� ��������� ������� � ��������
			SDL_Rect rect = { squareStartX, squareStartY, squareWidth, squareHeight };
			checkerboard[index].setRectTo(rect);

			// ��������� ������ �� ������� �������� ����
			if (blackCounter > 0 && flipCounter)
			{
				checkerboard[index].setCheckerTo(CheckerType::BLACK);
				--blackCounter;
			}
			// ��������� ����� �� ������ �������� ����
			if (index > 39 && redCounter > 0 && flipCounter)
			{
				checkerboard[index].setCheckerTo(CheckerType::RED);
				--redCounter;
			}
			squareStartX += squareWidth + borderThickness;

			// Increment index
			++index;
		}
		flipCounter = 1 - flipCounter;
		squareStartY += squareHeight + borderThickness;
	}


	// ���������� ��� ����-����
	bool stop = false;
	SDL_Event event;
	CheckerType currentPlayer = CheckerType::RED;
	GameState gameState = GameState::OUTLINE_CURRENT_PLAYER;
	bool render = true;
	Winner winner = Winner::DRAW;

	// ��������� ��������� ��� ��������� �����
	int blackCheckers = 12;
	int redCheckers = 12;

	// ����-��� (������� ����)
	while (!stop)
	{
		// ��������� �������
		while (SDL_PollEvent(&event) != 0)
		{
			// ��������� ������ (������� �������)
			if (event.type == SDL_QUIT)
			{
				stop = true;
			}
			// ��������� ������� � ������
			for (int i = 0; i < totalSquares; ++i)
			{
				// ��������� ��������
				if (gameState == GameState::MOVING)
				{
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						// ���� ������� ������� ��� ��������� ������� ��� ������������ �����
						if (checkerboard[i].getHighlight() == HighlightType::NEXT_POSSIBLE_POSITION)
						{
							if (checkerboard[i].isMouseInside())
							{
								// �������� ������ �����
								Square* checkerFlagged = checkerboard[i].getCheckerFlagged();

								// ��������� ������� �� ������� ��� ������ (���)
								Square* previousSquare = checkerboard[i].getPreviousSquare();

								// Back track through the pointers of checkers flagged
								while (checkerFlagged != nullptr)
								{
									// ������� �����
									checkerFlagged->setCheckerTo(CheckerType::NONE);

									// ��������� ���������� ����� ��� ������� ������
									if (currentPlayer == CheckerType::RED) --blackCheckers;
									else --redCheckers;

									// �������� ��������� ����� �� ����������� ��������
									checkerFlagged = previousSquare->getCheckerFlagged();

									// �������� ����� ���������� �������
									previousSquare = previousSquare->getPreviousSquare();
								}

								// �������� ����������
								if (redCheckers == 0)
								{
									winner = Winner::BLACK;
									stop = true;
								}
								else if (blackCheckers == 0)
								{
									winner = Winner::RED;
									stop = true;
								}

								// ������� ��� ����� � ��������� �� ���������� �������� 
								for (int i = 0; i < totalSquares; ++i)
								{
									checkerboard[i].flagChecker(nullptr);
									checkerboard[i].setPreviousSquare(nullptr);
								}

								// �������� ��������� �����
								CheckerType checker = Square::sSelected->getChecker();

								// �� ���������� ����� ������ ������� ����� ����, ���������� � � �����
								if (i > 0 && i < 8 && checker == CheckerType::RED)
								{
									checkerboard[i].setCheckerTo(CheckerType::KING_RED);
								}
								// �� ���������� ������ ������ ������� ����� ����, ���������� � � �����
								else if (i > 55 && i < 64 && checker == CheckerType::BLACK)
								{
									checkerboard[i].setCheckerTo(CheckerType::KING_BLACK);
								}
								// �����, ��������� �� ��� ����
								else
								{
									checkerboard[i].setCheckerTo(checker);
								}

								// ��� ����������� ������ ������ ������� ����� (������ ��������� ���� �� - none ��� �� ��������)
								Square::sSelected->setCheckerTo(CheckerType::NONE);

								// �������� ���������� ������� ������
								currentPlayer = oppositeChecker(currentPlayer);
								
								// ������� ��� ��������� (��������� �����)
								for (int i = 0; i < totalSquares; ++i)
								{
									checkerboard[i].setHighlightTo(HighlightType::NONE);
								}

								// ������������� ��������� ���� ��� ��������� �������� ������
								gameState = GameState::OUTLINE_CURRENT_PLAYER;

								// ������������� render
								render = true;

							}
						}
					}
				}
				// �������� ������� ��� �����
				if (gameState == GameState::SELECTING || gameState == GameState::MOVING)
				{
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						// If checker is player or its king varient
						if ((checkerboard[i].getChecker() == currentPlayer || checkerboard[i].getChecker() == (CheckerType)((int)currentPlayer + 2)))
						{
							if (checkerboard[i].isMouseInside())
							{
								if (Square::sSelected != &checkerboard[i])
								{
									Square::sSelected = &checkerboard[i];
									gameState = GameState::SELECTED;
								}
							}
						}
					}
				}
			}
		}

		// ���� �������� �������
		if (gameState == GameState::OUTLINE_CURRENT_PLAYER)
		{
			// ��������� ������ � ������ �������� ������
			for (int i = 0; i < totalSquares; ++i)
			{
				if (primeChecker(checkerboard[i].getChecker()) == currentPlayer)
				{
					checkerboard[i].setShowOutlineTo(true);
				}
				else
				{
					checkerboard[i].setShowOutlineTo(false);
				}
			}

			// ������������� render
			render = true;

			// ����� ��������� ���� �� ����� - selecting
			gameState = GameState::SELECTING;
		}

		// ���� ������� ��� ������
		if (gameState == GameState::SELECTED)
		{
			// ������� ��� ���������� ���������
			for (int i = 0; i < totalSquares; ++i)
			{
				checkerboard[i].setHighlightTo(HighlightType::NONE);
			}

			// �������� ��������� �������
			Square::sSelected->setHighlightTo(HighlightType::SELECTED);

			// �������� ���������� ���������� ��������
			int row;
			int col;
			Square::sSelected->getLocation(&row, &col);

			// �������� ��������� �����
			CheckerType checker = Square::sSelected->getChecker();

			// �������� ��������� ����������� ����
			Direction directions[4] = { northWest, northEast, southWest, southEast };

			// ���� ����� �����
			if (checker == CheckerType::RED)
			{
				// ��������� north west � north east
				for (int i = 0; i < 2; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}
			// ���� ������ �����
			else if (checker == CheckerType::BLACK)
			{
				// ��������� south west � south east
				for (int i = 2; i < 4; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}
			// ����� ��������� �����
			else
			{
				// ��������� ��� �����������
				for (int i = 0; i < 4; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}

			// ������������� ��������� ���� �� moving
			gameState = GameState::MOVING;

			// ������������� ���� render
			render = true;
		}

		// �������� ������ �����, ����� ��������, ���� ��������� highlighting
		if (render)
		{
			// ������ ����� � ������
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
		
			// �������� ����
			for (int i = 0; i < totalSquares; ++i)
			{
				checkerboard[i].renderSquare();
			}

			// ��������� ����� �� ������, ������ �����
			SDL_RenderPresent(renderer);

			// ������������� ���� render
			render = false;
		}

		// ��������� ���������
		SDL_Delay(10);

	}

	// ������� ���������� � ������� (������ �� ����� ������ ��������)
	if (winner == Winner::RED) std::cout << "The winner is RED!" << std::endl;
	else if (winner == Winner::BLACK) std::cout << "The winner is BLACK!" << std::endl;
	else std::cout << "It's a DRAW!" << std::endl;

	// ����������� �������� outline
	SDL_DestroyTexture(Checker::sOutline);
	Checker::sOutline = nullptr;

	// ����������� �������� highlight 
	for (int i = 0; i < Checker::sTotalTextures; ++i)
	{
		SDL_DestroyTexture(Checker::sTextureCache[i]);
		Checker::sTextureCache[i] = nullptr;
	}
	
	// ���������� renderer � window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyWindow(window2);
	renderer = nullptr;
	window = nullptr;
	window2 = nullptr;

	//Deallocate surface
	SDL_FreeSurface(mAbout);
	mAbout = NULL;

	// ������� �� SDL
	SDL_Quit();

	return 0;
}