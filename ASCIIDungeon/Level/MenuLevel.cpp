#include "MenuLevel.h"
#include "MenuLevel.h"
#include <Game/Game.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <cassert>

using namespace Craft;

MenuLevel::MenuLevel()
{
	_itemList.emplace_back(std::make_unique<MenuItem>("Resume Game", []()
		{
			Game& game = dynamic_cast<Game&>(Engine::Get());

			if (game.IsGameOver())
				return;

			game.ToggleMenu();
		}));

	_itemList.emplace_back(std::make_unique<MenuItem>("Start Game", []()
		{
			Game& game = dynamic_cast<Game&>(Engine::Get());
			game.StartGame();
		}));

	_itemList.emplace_back(std::make_unique<MenuItem>("Quit Game", []()
		{
			Engine::Get().Quit();
		}));
}

void MenuLevel::Tick(float deltaTime)
{
	Level::Tick(deltaTime);

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		Game& game = dynamic_cast<Game&>(Engine::Get());
		game.ToggleMenu();

		_currentIndex = 0;
	}

	// 배열의 요소 개수.
	const int length = static_cast<int>(_itemList.size());
	if (Input::Get().GetKeyDown(VK_UP))
	{
		// 인덱스 돌리기 (-방향).
		_currentIndex = (_currentIndex - 1 + length) % length;
	}

	if (Input::Get().GetKeyDown(VK_DOWN))
	{
		// 인덱스 돌리기 (+방향).
		_currentIndex = (_currentIndex + 1) % length;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		assert(_currentIndex >= 0 && _currentIndex < (int)_itemList.size() && _itemList[_currentIndex]->_onSelected);

		_itemList[_currentIndex]->_onSelected();
	}
}

void MenuLevel::Draw()
{
	Game& game = dynamic_cast<Game&>(Engine::Get());

	Renderer::Get().Submit("ASCII Dungeon", Vector2(0, 0), Color::BrightWhite);

	const int count = static_cast<int>(_itemList.size());
	for (int i = 0; i < count; i++)
	{
		Color textColor = (i == _currentIndex) ? _selectedColor : _unselectedColor;

		Renderer::Get().Submit(_itemList[i]->_text, Vector2(0, 5 + i), textColor);
	}
}
