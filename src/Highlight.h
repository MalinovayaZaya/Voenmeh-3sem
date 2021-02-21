/* ����� ��� ��������� � ��*/
#pragma once
#include <SDL.h>

// ����������� ����� highlight'�
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
	// ����������� renderer'�
	static SDL_Renderer* sRenderer;

	// �������� ������������ ������� ��� ���������� ������
	static const int sTotalColours = 5;
	static SDL_Color sColourCache[sTotalColours];

private:
	// ����������� ����� highlight'�
	HighlightType mHighlightType;

	// ����������� ������
	SDL_Color* mColour;

	// ����������� ���� highlight'�
	SDL_Rect mRect;

public:
	// �����������
	Highlight();

	// ��������� ���� highlight'�
	void setRectTo(const SDL_Rect& rect);

	// Set'n'get highlight
	void setHighlightTo(HighlightType highlightType);
	HighlightType getHighlight();

	// Render highlight'�
	void renderHighlight();
	
};