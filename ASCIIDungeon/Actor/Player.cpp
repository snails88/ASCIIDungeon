#include "Player.h"
#include <Define.h>
#include <Math/Color.h>
#include <Actor/Room.h>
#include <Actor/Cursor.h>
#include <Level/Level.h>
#include <Pathfind/Dijkstra.h>
#include <Pathfind/AStar.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Render/Renderer.h>

using namespace Craft;

Player::Player(const Vector2& pos)
	: super("P", pos)
{
	color = Color::B_White | Color::Red;
	sortingOrder = Sort::Player;
	_move = false;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (TurnManager::Get().IsPlayerTurn())
	{
		if (_move)
		{
			if (!_path.empty())
			{
				Vector2 nextPos = _path.front();
				_path.pop_front();

				// 문은 방끼리 겹쳐있어서 같은경로 두번씩 저장됨
				if (!_path.empty() && nextPos == _path.front())
				{
					nextPos = _path.front();
					_path.pop_front();
				}
				
				position = nextPos;

				std::shared_ptr<Cursor> cursor = GetOwner()->FindActor<Cursor>();

				if (cursor->GetPosition() == position)
					cursor->ChangeImage("P", Color::B_Cyan | Color::Magenta);

				MapManager::Get().RevealRoom(position);
				TurnManager::Get().SetTurnType(TurnManager::Turn::EnemyTurn);
			}
			else
				_move = false;
		}
	}

}

void Player::Draw()
{
	super::Draw();

	if (MapManager::Get().IsDebugMode() && !_path.empty())
	{
		for (size_t i = 0; i < _path.size(); i++)
		{
			Renderer::Get().Submit("*", _path[i], Color::Green | Color::B_BrightWhite, Sort::SortingOrder::Visualize);
		}
	}
}

void Player::Move(const Craft::Vector2& pos)
{
	if (_move)
	{
		StopMove();
		return;
	}

	if (MapManager::Get().IsDebugMode() && !_path.empty() && _path.back() == pos)
	{
		_move = true;
		return;
	}

	std::pair<RoomInfo*, RoomInfo*> info = MapManager::Get().FindRoomInfo(pos);

	bool visited1 = false, visited2 = false;

	if (info.first)
		visited1 = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info.first)).lock()->IsVisited();
	if (info.second)
		visited2 = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info.second)).lock()->IsVisited();

	if (!visited1 && !visited2 && !MapManager::Get().IsDebugMode())
		return;

	RequestPathFind(pos);

	if (!MapManager::Get().IsDebugMode())
		_move = true;

	// 현재위치부터라 하나빼줌
	if (!_path.empty())
		_path.pop_front();
}

void Player::StopMove()
{
	_path.clear();
	_move = false;
}

void Player::RequestPathFind(const Craft::Vector2& cursorPos)
{
	_path.clear();

	if (position == cursorPos)
	{
		TurnManager::Get().SetTurnType(TurnManager::Turn::EnemyTurn);
		return;
	}

	RoomInfo* currentRoom = MapManager::Get().FindRoomInfo(position).first;
	std::pair<RoomInfo*, RoomInfo*> goalRoom = MapManager::Get().FindRoomInfo(cursorPos);

	int goalIndex = MapManager::Get().GetRoomIndex(goalRoom.first);

	if (AStar::Get().IsBlocked(cursorPos, MapManager::Get().GetRoom(goalIndex).lock()->GetWalls()))
		return;

	if (currentRoom && goalRoom.first)
	{
		std::vector<RoomInfo*> route;

		if (Dijkstra::Get().FindRoute(currentRoom, goalRoom, route, true, true))
		{
			if (route.size() == 1)	// 방 내 이동
			{
				RoomInfo* info = MapManager::Get().FindRoomInfo(position).first;
				Rect rect = info->_rect;
				const std::vector<Vector2>& walls = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info)).lock()->GetWalls();

				AStar::Get().FindPath(position, cursorPos, rect, walls, _path);
			}
			else
			{
				Vector2 prevPos = position;
				for (size_t i = 0; i < route.size() - 1; i++)
				{
					std::deque<Vector2> tmp;
					Vector2 doorPos;
					std::pair<RoomInfo*, RoomInfo*> infos = MapManager::Get().FindRoomInfo(prevPos);
					RoomInfo* info = infos.first;

					if (!info)
						return;

					// 문이라 방 두개 검출되면 루트상 마지막거
					if (infos.second)
					{
						for (size_t i = 0; i < route.size(); i++)
						{
							if (route[i] == infos.first)
								info = infos.first;
							else if (route[i] == infos.second)
								info = infos.second;
						}
					}

					Rect rect = info->_rect;
					const std::vector<Vector2>& walls = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info)).lock()->GetWalls();

					MapManager::Get().FindDoorPosition(route[i], route[i + 1], doorPos);
					AStar::Get().FindPath(prevPos, doorPos, rect, walls, tmp);
					_path.insert(_path.end(), tmp.begin(), tmp.end());
					prevPos = doorPos;
				}
				if (_path.size() == 0)
					return;

				Vector2 currentPos = _path[_path.size() - 1];
				std::deque<Vector2> tmp;
				std::pair<RoomInfo*, RoomInfo*> infos = MapManager::Get().FindRoomInfo(currentPos);
				RoomInfo* info = infos.first;
				if (infos.second)
				{
					for (size_t i = 0; i < route.size(); i++)
					{
						if (route[i] == infos.first)
							info = infos.first;
						else if (route[i] == infos.second)
							info = infos.second;
					}
				}

				Rect rect = info->_rect;
				const std::vector<Vector2>& walls = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info)).lock()->GetWalls();
				AStar::Get().FindPath(currentPos, cursorPos, rect, walls, tmp);
				_path.insert(_path.end(), tmp.begin(), tmp.end());
			}
		}
	}
}
