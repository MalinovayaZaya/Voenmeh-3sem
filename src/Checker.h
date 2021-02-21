/* класс для ШаШеК */
#pragma once
#include <SDL.h>

// определение типов фишек
enum class CheckerType
{
	NONE,
	RED,
	BLACK,
	KING_RED,
	KING_BLACK,
};

class Checker
{
public:
	// определение renderer'а
	static SDL_Renderer* sRenderer;

	// создание статического массива для сохранения текстур шашек
	static const int sTotalTextures = 5;
	static SDL_Texture* sTextureCache[sTotalTextures];

	// определение checker outline текстуры
	static SDL_Texture* sOutline;

private:
	// определение типов фишек
	CheckerType mCheckerType;

	// определение текстуры фишки и её параметров
	SDL_Rect mRect;
	SDL_Texture* mTexture;

	// определение размера контура, позиции и флага 
	SDL_Rect mOutlineRect;
	bool mShowOutline;

public:
	// конструктор
	Checker();

	// утавновка типа фишки и её получение
	void setCheckerTo(CheckerType checkerType);
	CheckerType getChecker();

	// Center checker and outline texture on square
	void centerTextureOn(const SDL_Rect& squareRect);

	// Set show outline flag
	void setShowOutlineTo(const bool showOutline);

	// Render фишки
	void renderChecker();
	
};

