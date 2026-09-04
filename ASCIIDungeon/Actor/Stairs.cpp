#include "Stairs.h"
#include <Define.h>
#include <Math/Color.h>

using namespace Craft;

Stairs::Stairs(const Craft::Vector2& pos)
	: super(" ", pos, Color::B_GRAY)
{
	sortingOrder = Sort::Stairs;
}

void Stairs::BeginPlay()
{
	super::BeginPlay();
}

void Stairs::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void Stairs::Draw()
{
	super::Draw();
}
