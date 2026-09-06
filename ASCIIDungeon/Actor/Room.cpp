#include "Room.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>
#include <Manager/MapManager.h>
#include <Define.h>
#include <ETC/Door.h>
#include <Pathfind/AStar.h>
#include <deque>
#include <algorithm>

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

	Vector2 center = { _rect._left + ((_rect._right - _rect._left) / 2), _rect._top + ((_rect._bottom - _rect._top) / 2) };
	RoomInfo* info = MapManager::Get().FindRoomInfo(center).first;
	const std::vector<Door>& allDoors = MapManager::Get().GetDoors();

	if (info->_type == RoomInfo::RoomType::CORRIDOR)
	{
		std::vector<std::pair<RoomInfo*, RoomInfo*>> parents;
		std::vector<Vector2> doors;
		std::deque<Vector2> path;
		std::deque<Vector2> tmp;

		for (size_t i = 0; i < allDoors.size(); i++)
		{
			if (allDoors[i]._parents.first == info || allDoors[i]._parents.second == info)
				doors.emplace_back(allDoors[i]._position);
		}

		for (size_t i = 0; i < doors.size(); i++)
		{
			auto iter = std::find(_walls.begin(), _walls.end(), doors[i]);
			if (iter != _walls.end())
				_walls.erase(iter);
		}

		for (size_t i = 0; i < doors.size(); i++)
		{

			tmp.clear();
			AStar::Get().FindPath(doors[i], center, _rect, _walls, tmp, false);
			path.insert(path.end(), tmp.begin(), tmp.end());
		}
		
		_walls.clear();
		for (size_t i = _rect._left; i <= _rect._right; i++)
		{
			for (size_t j = _rect._top; j <= _rect._bottom; j++)
			{
				auto iter = std::find(path.begin(), path.end(), Vector2(i, j));

				if(iter == path.end())
					_walls.emplace_back(Vector2(i, j));
			}
		}
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
	for (int j = _rect._left + 1; j < _rect._right; j++)
		str += " ";

	for (int j = _rect._top + 1; j < _rect._bottom; j++)
		Renderer::Get().Submit(str, Vector2(_rect._left + 1, j), Color::B_White, Sort::Floor);
	///

	for (size_t i = 0; i < _walls.size(); i++)
		Renderer::Get().Submit(" ", _walls[i], 0, Sort::Wall);

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

	auto iter = std::find(_walls.begin(), _walls.end(), pos);
	if(iter != _walls.end())
		_walls.erase(iter);
}
