#pragma once
#include <memory>

class Player;
class Cursor;
class Game;
class GameLevel;

class InputManager
{
public:
	InputManager();

	void Tick(float deltaTime);

	static InputManager& Get();

private:
	inline static std::unique_ptr<InputManager> _instance = nullptr;

	Player* _player = nullptr;
	Cursor* _cursor = nullptr;
	GameLevel* _level = nullptr;
	Game* _game = nullptr;

	float _pressedTime = 0.f;
};

