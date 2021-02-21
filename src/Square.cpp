#include "Square.h"

SDL_Renderer* Square::sRenderer = nullptr;
Square* Square::sSelected = nullptr;

Square::Square()
	: mRow(0), mCol(0), mRect({ 0, 0, 0, 0 }), mBackgroundColour({0, 0, 0, 0}),
	mCheckerFlagged(nullptr), mPreviousSquare(nullptr)
{
	
}

void Square::setLocation(const int row, const int col)
{
	mRow = row;
	mCol = col;
}

void Square::getLocation(int* row, int* col)
{
	*row = mRow;
	*col = mCol;
}

void Square::setRectTo(SDL_Rect& rect)
{
	mRect = rect;
}

void Square::setBackgroundColourTo(SDL_Color& colour)
{
	mBackgroundColour = colour;
}

void Square::setCheckerTo(CheckerType checkerType)
{
	mChecker.setCheckerTo(checkerType);
	mChecker.centerTextureOn(mRect);

}

CheckerType Square::getChecker()
{
	return mChecker.getChecker();
}

void Square::setHighlightTo(HighlightType highlightType)
{
	mHighlight.setHighlightTo(highlightType);
	mHighlight.setRectTo(mRect);
}

HighlightType Square::getHighlight()
{
	return mHighlight.getHighlight();
}

void Square::setShowOutlineTo(const bool showOutline)
{
	mChecker.setShowOutlineTo(showOutline);
}

void Square::renderSquare()
{
	// render фона
	SDL_SetRenderDrawColor(sRenderer, mBackgroundColour.r, mBackgroundColour.g, mBackgroundColour.b, mBackgroundColour.a);
	SDL_RenderFillRect(sRenderer, &mRect);

	// render highlight'а (если он тут есть)
	mHighlight.renderHighlight();

	// render фишки (если он тут есть)
	mChecker.renderChecker();
}

bool Square::isMouseInside()
{
	int x, y;
	SDL_GetMouseState(&x, &y);

	// мышь левее квадрата
	if (x < mRect.x)
	{
		return false;
	}
	// мышь правее квадрата
	else if (x > mRect.x + mRect.w)
	{
		return false;
	}
	// мышь выше квадрата
	else if (y < mRect.y)
	{
		return false;
	}
	// мышь ниже квадрата
	else if (y > mRect.y + mRect.h)
	{
		return false;
	}
	// мышь в квадрате
	else
	{
		return true;
	}
}

void Square::flagChecker(Square* checker)
{
	mCheckerFlagged = checker;
}

Square* Square::getCheckerFlagged()
{
	return mCheckerFlagged;
}

void Square::setPreviousSquare(Square* square)
{
	mPreviousSquare = square;
}

Square* Square::getPreviousSquare()
{
	return mPreviousSquare;
}
