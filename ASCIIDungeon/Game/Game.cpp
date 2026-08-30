#include "Game.h"
#include <Level/GameLevel.h>
#include <Level/MenuLevel.h>

Game::Game()
{
	state = State::Menu;
	levelList[(int)state] = std::make_shared<MenuLevel>();

	mainLevel = levelList[(int)state];
}

void Game::ToggleMenu()
{
	int stateIndex = static_cast<int>(state);
	int nextState = 1 - stateIndex;

	mainLevel = levelList[nextState];
	state = static_cast<State>(nextState);
}

void Game::StartGame()
{
	if (!isGameOver)
	{
		if (levelList[(int)State::GamePlay])
			static_pointer_cast<GameLevel>(levelList[(int)State::GamePlay])->ResetActors();
		GameOver();
	}

	isGameOver = false;

	if (levelList[(int)State::GamePlay] == nullptr)
		levelList[(int)State::GamePlay] = std::make_shared<GameLevel>();
	state = State::GamePlay;
	mainLevel = levelList[(int)state];
}

void Game::GameOver()
{
	state = State::Menu;
	mainLevel = levelList[(int)state];
}

void Game::RequestGameOver()
{
	isGameOver = true;
}

void Game::ProcessCollision()
{
}
