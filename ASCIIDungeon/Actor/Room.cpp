#include "Room.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>
#include <Define.h>

using namespace Craft;

Room::Room(const Rect& rect)
	: super(""), _rect(rect)
{
	for (size_t i = _rect._left; i <= _rect._right; i++)
	{
		_walls.emplace_back(Vector2(i, _rect._top));
		_walls.emplace_back(Vector2(i, _rect._bottom));
	}

	for (size_t i = _rect._top + 1; i < _rect._bottom; i++)
	{
		_walls.emplace_back(Vector2(_rect._left, i));
		_walls.emplace_back(Vector2(_rect._right, i));
	}
}

void Room::BeginPlay()
{
	super::BeginPlay();
}

void Room::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void Room::Draw()
{
	/// 바닥 타일 깔기
	std::string str = "";
	for (int j = _rect._left + 1; j <= _rect._right - 1; j++)
		str += " ";

	for (int j = _rect._top + 1; j <= _rect._bottom - 1; j++)
		Renderer::Get().Submit(str, Vector2(_rect._left + 1, j), Color::B_GRAY);
	///

	for (size_t i = 0; i < _walls.size(); i++)
		Renderer::Get().Submit(" ", _walls[i], Color::B_White);

	for (size_t i = 0; i < _doors.size(); i++)
		Renderer::Get().Submit(" ", _doors[i], Color::B_Yellow, Sort::SortingOrder::Door);
}

void Room::AddWall(const Craft::Vector2& pos)
{
	Engine& e = Engine::Get();
	
	if (pos.x < 0 || pos.x >= e.GetWidth() || pos.y < 0 || pos.y >= e.GetHeight())
		return;

	_walls.emplace_back(pos);
}

void Room::AddDoor(const Craft::Vector2& pos)
{
	Engine& e = Engine::Get();

	if (pos.x < 0 || pos.x >= e.GetWidth() || pos.y < 0 || pos.y >= e.GetHeight())
		return;

	_doors.emplace_back(pos);
}
