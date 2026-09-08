#include "Stairs.h"
#include <Define.h>
#include <Math/Color.h>
#include <Manager/MapManager.h>
#include <Game/Game.h>

using namespace Craft;

Stairs::Stairs(const Craft::Vector2& pos)
	: super(" ", pos, Color::B_Magenta)
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
	if (!_visited && !MapManager::Get().IsDebugMode())
		return;
	super::Draw();
}

void Stairs::Reset()
{
	Game& game = dynamic_cast<Game&>(Engine::Get());
	game.ToggleMenu();
}
