#pragma once
#include <Actor/Actor.h>

class Stairs : public Craft::Actor
{
	TYPE_DECLARATIONS(Stairs, Actor)

public:
	Stairs(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
private:

};
