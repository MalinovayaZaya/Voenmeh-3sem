/* Шашки (пока только для двух игроков) */

// main - 184 строка

///////////////////////////////////////////////////
///												///
///			 TO DO LIST							///
///			 1. меню							///
///			 2. экран победы					///
///												///
///////////////////////////////////////////////////


#include <iostream>
#include <SDL.h>

#include "Square.h"

// определение состояния игры
static enum class GameState
{
	OUTLINE_CURRENT_PLAYER,
	SELECTING,
	SELECTED,
	MOVING,
};

// определение победителя, ну либо ничьи
static enum class Winner
{
	RED,
	BLACK,
	DRAW,
};

// получаем индекс 1D сетки 8 x 8 
static inline int getIndex(const int row, const int col)
{
	return 8 * row + col;
}

// проверка выхода за пределы игрового поля
static inline bool isOutOfBounds(const int row, const int col)
{
	if (row < 0 || col < 0 || row > 7 || col > 7) return true;
	else return false;
}

// загрузка текстур и удаление белого фона
static SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filePath)
{
	SDL_Texture* texture = nullptr;

	// загрузка картинки по спец пути
	SDL_Surface* loadedSurface = SDL_LoadBMP(filePath);
	if (loadedSurface == nullptr)
	{
		std::cout << "SDL could not load image into surface! Error: " << SDL_GetError() << std::endl;
	}
	else
	{
		// Color key image (убираем фон/белый)
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 255, 255, 255));

		// создаем текстуру
		texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (texture == nullptr)
		{
			std::cout << "SDL could not create texture from surface! Error: " << SDL_GetError() << std::endl;
		}

		// освобождаем surface
		SDL_FreeSurface(loadedSurface);
	}

	return texture;
}

// возвращаем главную фишку
static CheckerType primeChecker(CheckerType checker)
{
	if (checker == CheckerType::RED || checker == CheckerType::KING_RED) return CheckerType::RED;
	else if (checker == CheckerType::BLACK || checker == CheckerType::KING_BLACK) return CheckerType::BLACK;
	else return checker;
}

// аналогично, но для другого игрока
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

// определяем направления хода
static struct Direction
{
	int deltaRow;
	int deltaCol;
	Direction(int inDeltaRow, int inDeltaCol) : deltaRow(inDeltaRow), deltaCol(inDeltaCol) {}

}northWest(-1, -1), northEast(-1, 1), southWest(1, -1), southEast(1, 1);

// используем рекурсию для проверки направления хода
static void checkDirection(Square grid[], const Direction& direction, const CheckerType checker, const int row, const int col, const bool jumped)
{
	int nextRow = row + direction.deltaRow;
	int nextCol = col + direction.deltaCol;
	// проверяем наличие квадрата в поле
	if (!isOutOfBounds(nextRow, nextCol))
	{
		Square* squareAtDirection = &grid[getIndex(nextRow, nextCol)];
		// если квадрат занят оппонентом
		if (areOppositeCheckers(checker, squareAtDirection->getChecker()))
		{
			int nextRow2 = row + 2 * direction.deltaRow;
			int nextCol2 = col + 2 * direction.deltaCol;
			// если квадрат не выходит за пределы поля
			if (!isOutOfBounds(nextRow2, nextCol2))
			{
				Square* nextSquareAtDirection = &grid[getIndex(nextRow2, nextCol2)];
				// если следующий по направлению квадрат пуст и не выделен как возможный для хода
				if (nextSquareAtDirection->getChecker() == CheckerType::NONE && nextSquareAtDirection->getHighlight() != HighlightType::NEXT_POSSIBLE_POSITION)
				{
					std::cout << nextRow2 << ", " << nextCol2 << std::endl;

					// выделяем следующий возможный ход
					nextSquareAtDirection->setHighlightTo(HighlightType::NEXT_POSSIBLE_POSITION);

					// помечаем фишку для возможного удаления
					nextSquareAtDirection->flagChecker(squareAtDirection);

					// Store the square would occur from
					nextSquareAtDirection->setPreviousSquare(&grid[getIndex(row, col)]);

					// создаем возможные направления хода
					Direction directions[4] = { northWest, northEast, southWest, southEast };

					// рекурсивно проверяем квадраты, в зависимости от фишки
					// если белая фишка
					if (checker == CheckerType::RED)
					{
						// проверяем north west и north east
						for (int i = 0; i < 2; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
					// если черная фишка
					else if (checker == CheckerType::BLACK)
					{
						// проверяем south west и south east
						for (int i = 2; i < 4; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
					// далее проверяем дамок
					else
					{
						// проверяем все направления
						for (int i = 0; i < 4; ++i) checkDirection(grid, directions[i], checker, nextRow2, nextCol2, true);
					}
				}
			
				// если следующее направление в квадрате - это стартовая позиция (дамки могут вернуться в исходное положение)
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
	// определение размера окна
	const int windowWidth = 800;
	const int windowHeight = windowWidth;

	// инициализация SDL video subsystem (надо бы отдельную функцию для этого создать)
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		std::cout << "SDL could not intialise! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// создание окна
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

	// создание второго окна (правила игры)
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

	// создание renderer'а
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer == nullptr)
	{
		std::cout << "SDL could not create renderer! Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	// установка контура текстуры и удаление лишнего белого фона
	Checker::sOutline = loadTexture(renderer, "assets/Outline.bmp");

	// загрузка текстур шашек и удаление лишнего белого фона
	Checker::sTextureCache[(int)CheckerType::NONE] = loadTexture(renderer, "assets/Empty.bmp");
	Checker::sTextureCache[(int)CheckerType::RED] = loadTexture(renderer, "assets/RedChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::BLACK] = loadTexture(renderer, "assets/BlackChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::KING_RED] = loadTexture(renderer, "assets/KingRedChecker.bmp");
	Checker::sTextureCache[(int)CheckerType::KING_BLACK] = loadTexture(renderer, "assets/KingBlackChecker.bmp");

	// определение highlight'ов (RGBA)
	Highlight::sColourCache[(int)HighlightType::NONE] = { 0, 0, 0, 0 };
	Highlight::sColourCache[(int)HighlightType::MOUSE_OVER] = { 94, 152, 235, 150 }; // blue
	Highlight::sColourCache[(int)HighlightType::MOVABLE] = { 255, 255, 255, 150}; // white
	Highlight::sColourCache[(int)HighlightType::SELECTED] = { 94, 152, 235, 150 }; // blue
	Highlight::sColourCache[(int)HighlightType::NEXT_POSSIBLE_POSITION] = { 41, 242, 45, 120 }; // green

	// установка всех renderer'ов
	Checker::sRenderer = renderer;
	Highlight::sRenderer = renderer;
	Square::sRenderer = renderer;

	// создание сетки квадратов 8 x 8 (мб 10 х 10 ?)
	const int rows = 8;
	const int cols = 8;
	const int totalSquares = rows * cols;
	Square checkerboard[totalSquares];

	// установка двух разноцветных фонов (тип шахматная доска)
	SDL_Color backgrounds[2];
	backgrounds[0] = { 133, 71, 0, 255 };		// dark brown
	backgrounds[1] = { 230, 186, 127, 255 };	// bright brown

	// орпеделение границ и получение размеров квадратиков
	const int borderThickness = 5;
	const int numberOfBorders = cols + 1;
	const int squareWidth = (windowWidth - numberOfBorders * borderThickness) / cols;
	const int squareHeight = squareWidth;

	// определение счетчиков для отслеживания шашек на поле
	int blackCounter = 12;
	int redCounter = 12;
	int flipCounter = 1;
	int index = 0;

	// установка расположения всех квадратов, размеров, расположения шашек и цвета фонов
	int squareStartY = borderThickness;
	for (int row = 0; row < rows; ++row)
	{
		int squareStartX = borderThickness;
		for (int col = 0; col < cols; ++col)
		{
			//  установка расположения всех квадрата
			checkerboard[index].setLocation(row, col);

			// альтернативный цвет фона
			flipCounter = 1 - flipCounter;
			checkerboard[index].setBackgroundColourTo(backgrounds[flipCounter]);

			// установка стартовых позиций и размеров
			SDL_Rect rect = { squareStartX, squareStartY, squareWidth, squareHeight };
			checkerboard[index].setRectTo(rect);

			// установка черных на ВЕРХНЕЙ половине поля
			if (blackCounter > 0 && flipCounter)
			{
				checkerboard[index].setCheckerTo(CheckerType::BLACK);
				--blackCounter;
			}
			// установка белых на НИЖНЕЙ половине поля
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


	// переменные для гейм-лупа
	bool stop = false;
	SDL_Event event;
	CheckerType currentPlayer = CheckerType::RED;
	GameState gameState = GameState::OUTLINE_CURRENT_PLAYER;
	bool render = true;
	Winner winner = Winner::DRAW;

	// установка счетчиков для съеденных фишек
	int blackCheckers = 12;
	int redCheckers = 12;

	// гейм-луп (игровой цикл)
	while (!stop)
	{
		// обработка событий
		while (SDL_PollEvent(&event) != 0)
		{
			// обработка выхода (красный крэстик)
			if (event.type == SDL_QUIT)
			{
				stop = true;
			}
			// обработка событий с мышкой
			for (int i = 0; i < totalSquares; ++i)
			{
				// состояние движения
				if (gameState == GameState::MOVING)
				{
					if (event.type == SDL_MOUSEBUTTONDOWN)
					{
						// если квадрат помечен как возможная позиция для передвижения фишки
						if (checkerboard[i].getHighlight() == HighlightType::NEXT_POSSIBLE_POSITION)
						{
							if (checkerboard[i].isMouseInside())
							{
								// помечаем флагом фишку
								Square* checkerFlagged = checkerboard[i].getCheckerFlagged();

								// получение позиции из которой был прыжок (ход)
								Square* previousSquare = checkerboard[i].getPreviousSquare();

								// Back track through the pointers of checkers flagged
								while (checkerFlagged != nullptr)
								{
									// убираем фишку
									checkerFlagged->setCheckerTo(CheckerType::NONE);

									// уменьшаем количество фишек для другого игрока
									if (currentPlayer == CheckerType::RED) --blackCheckers;
									else --redCheckers;

									// отмечаем следующую фишку из предыдущего квадрата
									checkerFlagged = previousSquare->getCheckerFlagged();

									// получаем новый предыдущий квадрат
									previousSquare = previousSquare->getPreviousSquare();
								}

								// выявляем победителя
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

								// убираем все флаги и указатели на предыдущие квадраты 
								for (int i = 0; i < totalSquares; ++i)
								{
									checkerboard[i].flagChecker(nullptr);
									checkerboard[i].setPreviousSquare(nullptr);
								}

								// получаем выбранную фишку
								CheckerType checker = Square::sSelected->getChecker();

								// по достижении белой фишкой другого конца поля, превращаем её в дамку
								if (i > 0 && i < 8 && checker == CheckerType::RED)
								{
									checkerboard[i].setCheckerTo(CheckerType::KING_RED);
								}
								// по достижении черной фишкой другого конца поля, превращаем её в дамку
								else if (i > 55 && i < 64 && checker == CheckerType::BLACK)
								{
									checkerboard[i].setCheckerTo(CheckerType::KING_BLACK);
								}
								// иначе, оставляем всё как есть
								else
								{
									checkerboard[i].setCheckerTo(checker);
								}

								// даём возмодность игроку заново выбрать фишку (ставим выделение хода на - none или не выбранно)
								Square::sSelected->setCheckerTo(CheckerType::NONE);

								// передаем инициативу другому игроку
								currentPlayer = oppositeChecker(currentPlayer);
								
								// удаляем все подсказки (выделение фишек)
								for (int i = 0; i < totalSquares; ++i)
								{
									checkerboard[i].setHighlightTo(HighlightType::NONE);
								}

								// устанавливаем состояние игры для выделения текущего игрока
								gameState = GameState::OUTLINE_CURRENT_PLAYER;

								// устанавливаем render
								render = true;

							}
						}
					}
				}
				// выбираем позицию или ходим
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

		// если выбираем позицию
		if (gameState == GameState::OUTLINE_CURRENT_PLAYER)
		{
			// добавляем контур к шашкам текущего игрока
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

			// устанавливаем render
			render = true;

			// мняем состояние игры на выбор - selecting
			gameState = GameState::SELECTING;
		}

		// если квадрат был выбран
		if (gameState == GameState::SELECTED)
		{
			// убираем все предыдущие выделения
			for (int i = 0; i < totalSquares; ++i)
			{
				checkerboard[i].setHighlightTo(HighlightType::NONE);
			}

			// выделяем выбранный квадрат
			Square::sSelected->setHighlightTo(HighlightType::SELECTED);

			// получаем координаты выбранного квадрата
			int row;
			int col;
			Square::sSelected->getLocation(&row, &col);

			// получаем выбранную фишку
			CheckerType checker = Square::sSelected->getChecker();

			// выделяем возможные направления хода
			Direction directions[4] = { northWest, northEast, southWest, southEast };

			// если белая фишка
			if (checker == CheckerType::RED)
			{
				// проверяем north west и north east
				for (int i = 0; i < 2; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}
			// если черная фишка
			else if (checker == CheckerType::BLACK)
			{
				// проверяем south west и south east
				for (int i = 2; i < 4; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}
			// далее проверяем дамок
			else
			{
				// проверяем все направления
				for (int i = 0; i < 4; ++i) checkDirection(checkerboard, directions[i], checker, row, col, false);
			}

			// устанавливаем состояние игры на moving
			gameState = GameState::MOVING;

			// устанавливаем флаг render
			render = true;
		}

		// рендерим только тогда, когда походили, либо необходим highlighting
		if (render)
		{
			// красим экран в черный
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
		
			// рендерим поле
			for (int i = 0; i < totalSquares; ++i)
			{
				checkerboard[i].renderSquare();
			}

			// обновляем экран из буфера, чистим буфер
			SDL_RenderPresent(renderer);

			// устанавливаем флаг render
			render = false;
		}

		// замедляем программу
		SDL_Delay(10);

	}

	// выводим победителя в консоль (хорошо бы экран победы допилить)
	if (winner == Winner::RED) std::cout << "The winner is RED!" << std::endl;
	else if (winner == Winner::BLACK) std::cout << "The winner is BLACK!" << std::endl;
	else std::cout << "It's a DRAW!" << std::endl;

	// освобождаем текстуру outline
	SDL_DestroyTexture(Checker::sOutline);
	Checker::sOutline = nullptr;

	// освобождаем текстуру highlight 
	for (int i = 0; i < Checker::sTotalTextures; ++i)
	{
		SDL_DestroyTexture(Checker::sTextureCache[i]);
		Checker::sTextureCache[i] = nullptr;
	}
	
	// уничтожаем renderer и window
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_DestroyWindow(window2);
	renderer = nullptr;
	window = nullptr;
	window2 = nullptr;

	//Deallocate surface
	SDL_FreeSurface(mAbout);
	mAbout = NULL;

	// выходим из SDL
	SDL_Quit();

	return 0;
}
