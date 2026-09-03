#include "Stairs.h"
#include <Define.h>

Stairs::Stairs(const Craft::Vector2& pos)
	: super(" ", pos, 0)
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
