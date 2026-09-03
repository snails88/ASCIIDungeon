#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>

class Player : public Craft::Actor
{
	TYPE_DECLARATIONS(Player, Actor)

public:
	Player(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
private:

};

