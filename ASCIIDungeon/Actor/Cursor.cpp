#include "Cursor.h"
#include <Define.h>

using namespace Craft;

Cursor::Cursor(const Craft::Vector2& pos)
	: super(" ", pos, Color::B_Cyan)
{
	sortingOrder = Sort::Cursor;
}

void Cursor::BeginPlay()
{
	super::BeginPlay();
}

void Cursor::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void Cursor::Draw()
{
	super::Draw();
}
