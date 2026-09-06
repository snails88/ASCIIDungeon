#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>
#include <deque>

class GameLevel;

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void Move(const Craft::Vector2& pos);
	void StopMove();
	void RequestPathFind(const Craft::Vector2& cursorPos);

private:
	std::deque<Craft::Vector2> _path;
	bool _move;
};

