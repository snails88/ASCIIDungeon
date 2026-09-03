#pragma once
#include <Actor/Actor.h>

class Player;
class Cursor;
class Game;

class InputManager : public Craft::Actor
{
	TYPE_DECLARATIONS(InputManager, Actor)
public:
	InputManager();

	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Draw();
private:
	Player* _player = nullptr;
	Cursor* _cursor = nullptr;
	Game* _game = nullptr;

	float _pressedTime = 0.f;
};

