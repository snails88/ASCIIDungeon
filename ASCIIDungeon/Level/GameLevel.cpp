#include "GameLevel.h"
#include <Game/Game.h>
#include <Manager/InputManager.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Actor/Enemy.h>

void GameLevel::ResetActors()
{
	for (const std::shared_ptr<Actor>& actor : actorList)
	{
		actor->SetActive(false);
	}
}

void GameLevel::AddEnemy(std::weak_ptr<Enemy> enemy)
{
	_enemies.emplace_back(enemy);
}

void GameLevel::EraseEnemy(std::weak_ptr<Enemy> enemy)
{
	if (enemy.expired())
	{
		auto iter = _enemies.begin();

		while (iter != _enemies.end())
		{
			if (iter->expired())
			{
				_enemies.erase(iter);
				return;
			}

			++iter;
		}
	}
	else
	{
		auto iter = _enemies.begin();

		while (iter != _enemies.end())
		{
			if (enemy.lock() == iter->lock())
			{
				_enemies.erase(iter);
				return;
			}

			++iter;
		}
	}
}

bool GameLevel::IsOccupied(const Vector2& pos) const
{
	auto iter = _enemies.begin();

	while (iter != _enemies.end())
	{
		if (iter->lock()->GetPosition() == pos)
			return true;

		++iter;
	}

	return false;
}


void GameLevel::OnInitialized()
{
	super::OnInitialized();
	MapManager::Get().BeginPlay();
}

void GameLevel::Tick(float deltaTime)
{
	InputManager::Get().Tick(deltaTime);
	super::Tick(deltaTime);
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
