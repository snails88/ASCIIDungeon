#pragma once
#include <Actor/Actor.h>

class Cursor : public Craft::Actor
{
	TYPE_DECLARATIONS(Cursor, Actor)

public:
	Cursor(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
private:

};
