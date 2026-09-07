#pragma once
#include <Actor/Actor.h>

class Player;

class Enemy : public Craft::Actor
{
	TYPE_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	inline void SetSleep(bool isSleep) { _isSleep = isSleep; }
	inline const bool IsSleep() const { return _isSleep; }

private:
	bool _isSleep = true;
	std::weak_ptr<Player> _player;
};

