#include "InputManager.h"
#include <Game/Game.h>
#include <Level/Level.h>
#include <Actor/Player.h>
#include <Actor/Cursor.h>
#include <Input/Input.h>

using namespace Craft;

InputManager::InputManager()
	:super("")
{
	_game = static_cast<Game*>(&Engine::Get());

	_player = nullptr;
	_cursor = nullptr;
}

void InputManager::BeginPlay()
{
	super::BeginPlay();
}

void InputManager::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if(!_player)
		_player = GetOwner()->FindActor<Player>().get();

	if(!_cursor)
		_cursor = GetOwner()->FindActor<Cursor>().get();

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		_game->ToggleMenu();
	}

	Vector2 dir = Vector2::Zero;

	if (Input::Get().GetKey(VK_LEFT))
		dir.x -= 1;
	if (Input::Get().GetKey(VK_RIGHT))
		dir.x += 1;
	if (Input::Get().GetKey(VK_UP))
		dir.y -= 1;
	if (Input::Get().GetKey(VK_DOWN))
		dir.y += 1;

	if (dir == Vector2::Zero)
		_pressedTime = 0.f;
	else
		_pressedTime += deltaTime;

	if (_pressedTime >= 0.05f)
	{
		Vector2 newPos = _cursor->GetPosition() + dir;
		if (newPos.x < 0)
			newPos.x = 0;
		if (newPos.x > _game->GetWidth() - 1)
			newPos.x = _game->GetWidth() - 1;
		if (newPos.y < 0)
			newPos.y = 0;
		if (newPos.y > _game->GetHeight() - 1)
			newPos.y = _game->GetHeight() - 1;
		_cursor->SetPosition(newPos);
		
		_pressedTime = 0.f;
	}

}

void InputManager::Draw()
{
}
