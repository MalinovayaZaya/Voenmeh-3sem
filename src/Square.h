/* ����� Square �������� �� ������� ����, �� � �� ������ */
#pragma once
#include <SDL.h>

#include "Checker.h"
#include "Highlight.h"

class Square
{
public:
	// ����������� renderer'�
	static SDL_Renderer* sRenderer;

	// ����������� ���������� ��������
	static Square* sSelected;

private:
	// ���������� ��� �������� ��������� ��������� �� ����
	int mRow;
	int mCol;

	// ����������� ������� � �������� ��������
	SDL_Rect mRect;

	// ����������� �����
	Checker mChecker;
	
	// ����������� highlighting'�
	Highlight mHighlight;

	// ����������� ����� ����
	SDL_Color mBackgroundColour;

	// ���� �����(��������) ��� ���������� ��������
	Square* mCheckerFlagged;

	// ��� ������ �� ���������, �� ������� ��� ������ ��� (������� �����)
	Square* mPreviousSquare;

public:
	// �����������
	Square();

	// ��������� � ��������� ���������� ��������� �� ����
	void setLocation(const int row, const int col);
	void getLocation(int* row, int* col);

	// ��������� ������� � �������� ��������
	void setRectTo(SDL_Rect& rect);

	// ��������� ����� ����
	void setBackgroundColourTo(SDL_Color& colour);

	// Set'n'get checker
	void setCheckerTo(CheckerType checkerType);
	CheckerType getChecker();

	// ��������� highlight'�
	void setHighlightTo(HighlightType highlightType);
	HighlightType getHighlight();
	
	// ��������� ����� ������ �������
	void setShowOutlineTo(const bool showOutline);

	// render ����� � ��������
	void renderSquare();

	// �������� ������� ����� � ��������
	bool isMouseInside();

	// ������� � ��������� ����� ��� ���������� ��������
	void flagChecker(Square* checker);
	Square* getCheckerFlagged();

	// ��� ������ �� ���������, �� ������� ��� ������ ��� (������� �����)
	void setPreviousSquare(Square* square);
	Square* getPreviousSquare();

};