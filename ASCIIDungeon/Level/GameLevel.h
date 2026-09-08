#pragma once
#include <Level/Level.h>
#include <list>
#include <memory>
class Enemy;

using namespace Craft;
class GameLevel : public Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	void ResetActors();

	void AddEnemy(std::weak_ptr<Enemy> enemy);
	void EraseEnemy(std::weak_ptr<Enemy> enemy);
	inline const std::list<std::weak_ptr<Enemy>>& GetEnemies() const { return _enemies; }
	bool IsOccupied(const Vector2& pos) const;
	
private:
	virtual void OnInitialized() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	
private:
	std::list<std::weak_ptr<Enemy>> _enemies;
};

