/* класс для подсказок и тп*/
#pragma once
#include <SDL.h>

// определение типов highlight'а
enum class HighlightType
{
	NONE,
	MOUSE_OVER,
	MOVABLE,
	SELECTED,
	NEXT_POSSIBLE_POSITION,
};

class Highlight
{
public:
	// определение renderer'а
	static SDL_Renderer* sRenderer;

	// создание статического массива для сохранения цветов
	static const int sTotalColours = 5;
	static SDL_Color sColourCache[sTotalColours];

private:
	// определение типов highlight'а
	HighlightType mHighlightType;

	// определение цветов
	SDL_Color* mColour;

	// определение зоны highlight'а
	SDL_Rect mRect;

public:
	// конструктор
	Highlight();

	// установка зоны highlight'а
	void setRectTo(const SDL_Rect& rect);

	// Set'n'get highlight
	void setHighlightTo(HighlightType highlightType);
	HighlightType getHighlight();

	// Render highlight'а
	void renderHighlight();
	
};