/* класс Square отвечает за игровое поле, ну и по мелочи */
#pragma once
#include <SDL.h>

#include "Checker.h"
#include "Highlight.h"

class Square
{
public:
	// определение renderer'а
	static SDL_Renderer* sRenderer;

	// определение выбранного квадрата
	static Square* sSelected;

private:
	// переменные для хранения координат квадратов на поле
	int mRow;
	int mCol;

	// определение позиции и размеров квадрата
	SDL_Rect mRect;

	// определение фишки
	Checker mChecker;
	
	// определение highlighting'а
	Highlight mHighlight;

	// определение цвета фона
	SDL_Color mBackgroundColour;

	// флаг фишки(квадрата) для возможного удаления
	Square* mCheckerFlagged;

	// для слежки за квадратом, из кторого был сделан ход (пометка фишки)
	Square* mPreviousSquare;

public:
	// конструктор
	Square();

	// установка и получение геолокации квадратов на поле
	void setLocation(const int row, const int col);
	void getLocation(int* row, int* col);

	// установка позиции и размеров квадрата
	void setRectTo(SDL_Rect& rect);

	// установка цвета фона
	void setBackgroundColourTo(SDL_Color& colour);

	// Set'n'get checker
	void setCheckerTo(CheckerType checkerType);
	CheckerType getChecker();

	// установка highlight'а
	void setHighlightTo(HighlightType highlightType);
	HighlightType getHighlight();
	
	// установка флага показа контура
	void setShowOutlineTo(const bool showOutline);

	// render всего в квадрате
	void renderSquare();

	// проверка наличия мышки в квадрате
	bool isMouseInside();

	// пометка и получение фишки для возможного удаления
	void flagChecker(Square* checker);
	Square* getCheckerFlagged();

	// для слежки за квадратом, из кторого был сделан ход (пометка фишки)
	void setPreviousSquare(Square* square);
	Square* getPreviousSquare();

};