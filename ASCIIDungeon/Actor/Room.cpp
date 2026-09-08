#include "Room.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Define.h>
#include <Actor/Stairs.h>
#include <ETC/Door.h>
#include <Pathfind/AStar.h>
#include <Level/GameLevel.h>
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
			if (!AStar::Get().FindPath(doors[i], center, _rect, _walls, tmp, false))
				return;
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

	if (_isVisible && TurnManager::Get().GetCurrentTurn() != TurnManager::Turn::PlayerTurn)
	{
		WakeUpEnemies();

		Vector2 center = { _rect._left + ((_rect._right - _rect._left) / 2), _rect._top + ((_rect._bottom - _rect._top) / 2) };

		RoomInfo* info = MapManager::Get().FindRoomInfo(center).first;

		for (size_t i = 0; i < info->_connected.size(); i++)
		{
			Rect rect = info->_connected[i]->_rect;

			Vector2 connectedCenter = { rect._left + ((rect._right - rect._left) / 2), rect._top + ((rect._bottom - rect._top) / 2) };
			RoomInfo* connectedInfo = MapManager::Get().FindRoomInfo(connectedCenter).first;
			int index = MapManager::Get().GetRoomIndex(connectedInfo);
			MapManager::Get().GetRoom(index).lock()->WakeUpEnemies();
		}
	}
}

void Room::Draw()
{
	/// 바닥 타일 깔기
	std::string str = "";
	for (int j = _rect._left + 1; j < _rect._right; j++)
		str += " ";

	for (int j = _rect._top + 1; j < _rect._bottom; j++)
	{
		if(_isVisible)
			Renderer::Get().Submit(str, Vector2(_rect._left + 1, j), Color::B_White, Sort::Floor);
		else if(_visited || MapManager::Get().IsDebugMode())
			Renderer::Get().Submit(str, Vector2(_rect._left + 1, j), Color::B_Gray, Sort::Floor);
	}
		
	///

	for (size_t i = 0; i < _walls.size(); i++)
		Renderer::Get().Submit(" ", _walls[i], 0, Sort::Wall);

	for (size_t i = 0; i < _doors.size(); i++)
	{
		if(_visited || MapManager::Get().IsDebugMode())
			Renderer::Get().Submit(" ", _doors[i], Color::B_Yellow, Sort::SortingOrder::Door);
	}
		
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

void Room::SetVisible(bool visible)
{
	if (visible)
	{
		Visit();
		
		if (!_stairs.expired())
			_stairs.lock()->Visit();
	}

	_isVisible = visible;
}

void Room::WakeUpEnemies()
{
	std::shared_ptr<GameLevel> level = Cast<GameLevel>(Engine::Get().GetLevel().lock());

	const std::list<std::weak_ptr<Enemy>> enemies = level->GetEnemies();

	auto iter = enemies.begin();

	while (iter != enemies.end())
	{
		std::shared_ptr<Enemy> enemy = iter->lock();

		if (!enemy)
		{
			level->EraseEnemy(enemy);
			iter = enemies.begin();
			continue;
		}

		std::pair<RoomInfo*, RoomInfo*> infos = MapManager::Get().FindRoomInfo(enemy->GetPosition());

		for (RoomInfo* info : { infos.first, infos.second })
		{
			if (!info)
				break;

			if (info->_rect == _rect)
				enemy->SetSleep(false);
		}

		++iter;
	}
}
