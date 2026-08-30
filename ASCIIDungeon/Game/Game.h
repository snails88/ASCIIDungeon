#pragma once
#include <Engine/Engine.h>

enum class State
{
	GamePlay = 0,
	Menu = 1,
	Length
};

class Game : public Craft::Engine
{
public:
	Game();
	~Game() = default;

	void ToggleMenu();
	void StartGame();
	void GameOver();
	void RequestGameOver();

	inline bool IsGameOver() const { return isGameOver; }
private:
	virtual void ProcessCollision() override;	// 충돌 제거용(부모꺼 호출안함)

private:
	std::shared_ptr<Craft::Level> levelList[2]{};

	State state = State::GamePlay;
	bool isGameOver = true;
};

