/* ����� ��� ����� */
#pragma once
#include <SDL.h>

// ����������� ����� �����
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
	// ����������� renderer'�
	static SDL_Renderer* sRenderer;

	// �������� ������������ ������� ��� ���������� ������� �����
	static const int sTotalTextures = 5;
	static SDL_Texture* sTextureCache[sTotalTextures];

	// ����������� checker outline ��������
	static SDL_Texture* sOutline;

private:
	// ����������� ����� �����
	CheckerType mCheckerType;

	// ����������� �������� ����� � � ����������
	SDL_Rect mRect;
	SDL_Texture* mTexture;

	// ����������� ������� �������, ������� � ����� 
	SDL_Rect mOutlineRect;
	bool mShowOutline;

public:
	// �����������
	Checker();

	// ��������� ���� ����� � � ���������
	void setCheckerTo(CheckerType checkerType);
	CheckerType getChecker();

	// Center checker and outline texture on square
	void centerTextureOn(const SDL_Rect& squareRect);

	// Set show outline flag
	void setShowOutlineTo(const bool showOutline);

	// Render �����
	void renderChecker();
	
};

