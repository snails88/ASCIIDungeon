#include "Player.h"
#include <Define.h>
#include <Math/Color.h>
#include <Level/GameLevel.h>
#include <Pathfind/Dijkstra.h>
#include <Pathfind/AStar.h>
#include <Actor/Room.h>

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
	_level = Cast<GameLevel>(GetOwner()).get();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (_level->IsPlayerTurn())
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
				_level->SetTurnType(GameLevel::Turn::EnemyTurn);
			}
			else
				_move = false;
		}
	}

}

void Player::Draw()
{
	super::Draw();
}

void Player::Move(const Craft::Vector2& pos)
{
	_move = true;
	RequestPathFind(pos);

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
		_level->SetTurnType(GameLevel::Turn::EnemyTurn);
		return;
	}

	RoomInfo* currentRoom = _level->FindRoomInfo(position).first;
	RoomInfo* goalRoom = _level->FindRoomInfo(cursorPos).first;

	if (currentRoom && goalRoom)
	{
		std::vector<RoomInfo*> route;

		if (Dijkstra::Get().FindRoute(currentRoom, goalRoom, route))
		{
			if (route.size() == 1)	// 방 내 이동
			{
				RoomInfo* info = _level->FindRoomInfo(position).first;
				Rect rect = info->_rect;
				const std::vector<Vector2>& walls = _level->GetRoom(_level->GetRoomIndex(info)).lock()->GetWalls();

				AStar::Get().FindPath(position, cursorPos, rect, walls, _path);
			}
			else
			{
				Vector2 prevPos = position;
				for (size_t i = 0; i < route.size() - 1; i++)
				{
					std::deque<Vector2> tmp;
					Vector2 doorPos;
					std::pair<RoomInfo*, RoomInfo*> infos = _level->FindRoomInfo(prevPos);
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
					const std::vector<Vector2>& walls = _level->GetRoom(_level->GetRoomIndex(info)).lock()->GetWalls();

					_level->FindDoorPosition(route[i], route[i + 1], doorPos);
					AStar::Get().FindPath(prevPos, doorPos, rect, walls, tmp);
					_path.insert(_path.end(), tmp.begin(), tmp.end());
					prevPos = doorPos;
				}
				if (_path.size() == 0)
					return;

				Vector2 currentPos = _path[_path.size() - 1];
				std::deque<Vector2> tmp;
				std::pair<RoomInfo*, RoomInfo*> infos = _level->FindRoomInfo(currentPos);
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
				const std::vector<Vector2>& walls = _level->GetRoom(_level->GetRoomIndex(info)).lock()->GetWalls();
				AStar::Get().FindPath(currentPos, cursorPos, rect, walls, tmp);
				_path.insert(_path.end(), tmp.begin(), tmp.end());
			}
		}
	}
}
