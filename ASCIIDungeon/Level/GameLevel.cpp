#include "GameLevel.h"
#include <Game/Game.h>
#include <Manager/InputManager.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>

void GameLevel::ResetActors()
{
	for (const std::shared_ptr<Actor>& actor : actorList)
	{
		actor->SetActive(false);
	}
}


void GameLevel::OnInitialized()
{
	super::OnInitialized();
	MapManager::Get().BeginPlay();
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	InputManager::Get().Tick(deltaTime);
	TurnManager::Get().Tick(deltaTime);

	Game& engine = static_cast<Game&>(Engine::Get());
	if (engine.IsGameOver())
	{
		ResetActors();
		engine.GameOver();
		return;
	}
}

void GameLevel::Draw()
{
	super::Draw();
}
