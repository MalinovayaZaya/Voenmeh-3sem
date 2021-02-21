#include "Checker.h"

SDL_Renderer* Checker::sRenderer = nullptr;
SDL_Texture* Checker::sTextureCache[sTotalTextures] = { nullptr };
SDL_Texture* Checker::sOutline = nullptr;

Checker::Checker()
	:mCheckerType(CheckerType::NONE), mRect({ 0, 0, 0, 0 }), 
	mTexture(sTextureCache[(int)CheckerType::NONE]), mOutlineRect({0, 0, 0, 0}),
	mShowOutline(false)
{

}

void Checker::setCheckerTo(CheckerType checkerType)
{
	mCheckerType = checkerType;
	mTexture = sTextureCache[(int)checkerType];
	SDL_QueryTexture(mTexture, NULL, NULL, &mRect.w, &mRect.h);
	SDL_QueryTexture(sOutline, NULL, NULL, &mOutlineRect.w, &mOutlineRect.h);
}

CheckerType Checker::getChecker()
{
	return mCheckerType;
}

void Checker::centerTextureOn(const SDL_Rect& squareRect)
{
	mRect.y = squareRect.y + 0.5 * (squareRect.h - mRect.h);
	mRect.x = squareRect.x + 0.5 * (squareRect.w - mRect.w);

	mOutlineRect.y = squareRect.y + 0.5 * (squareRect.h - mOutlineRect.h);
	mOutlineRect.x = squareRect.x + 0.5 * (squareRect.w - mOutlineRect.w);

}

void Checker::setShowOutlineTo(const bool showOutline)
{
	mShowOutline = showOutline;
}

void Checker::renderChecker()
{
	// Only render outline if flag is set to tru
	if (mShowOutline)
	{
		// Copy Outline texture to rendering target
		SDL_RenderCopy(sRenderer, sOutline, nullptr, &mOutlineRect);
	}

	// Only render if it's not an empty square
	if (mCheckerType != CheckerType::NONE)
	{
		// Copy checker texture to rendering target
		SDL_RenderCopy(sRenderer, mTexture, nullptr, &mRect);
	}

}
