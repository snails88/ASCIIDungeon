#include "Player.h"
#include <Define.h>
#include <Math/Color.h>
#include <Actor/Room.h>
#include <Actor/Cursor.h>
#include <Level/GameLevel.h>
#include <Pathfind/Dijkstra.h>
#include <Pathfind/AStar.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Render/Renderer.h>
#include <Engine/Engine.h>

using namespace Craft;

Player::Player(const Vector2& pos)
	: super("P", pos)
{
	color = Color::B_White | Color::Red;
	sortingOrder = Sort::Player;
	_move = false;
	_attack = false;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (TurnManager::Get().GetCurrentTurn() == TurnManager::Turn::PlayerTurn)
	{
		if (_move)
		{
			if (!_path.empty())
			{
				if (_attack && _path.size() == 1)
				{
					_move = false;
					return;
				}

				Vector2 nextPos = _path.front();
				_path.pop_front();

				// 문은 방끼리 겹쳐있어서 같은경로 두번씩 저장됨
				if (!_path.empty() && nextPos == _path.front())
				{
					nextPos = _path.front();
					_path.pop_front();
				}
				
				if (nextPos == position)
					return;

				auto infos = MapManager::Get().FindRoomInfo(nextPos);

				for (RoomInfo* info : { infos.first, infos.second })
				{
					if (!info)
						break;

					int index = MapManager::Get().GetRoomIndex(info);

					if (index > 0)
					{
						std::shared_ptr<GameLevel> level = Cast<GameLevel>(Engine::Get().GetLevel().lock());

						const std::list<std::weak_ptr<Enemy>>& enemies = level->GetEnemies();

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

							if (enemy->GetPosition() == nextPos)
							{
								Vector2 newGoalPos = _path.back();

								_path.clear();

								RequestPathFind(newGoalPos, _attack);
								return;
							}

							++iter;
						}
						
					}
				}

				position = nextPos;

				std::shared_ptr<Cursor> cursor = GetOwner()->FindActor<Cursor>();

				if (cursor->GetPosition() == position)
					cursor->ChangeImage("P", Color::B_Cyan | Color::Magenta);

				MapManager::Get().RevealRoom(position);
				TurnManager::Get().SetTurnType(TurnManager::Turn::EnemyTurn);
				return;
			}
			else
				_move = false;
		}

		if (_attack && !_move && _path.size() == 1)
		{
			Vector2 targetPos = _path.front();
			_path.pop_front();

			const std::pair<RoomInfo*, RoomInfo*>& info = MapManager::Get().FindRoomInfo(position);

			int index = -1;
			
			for (RoomInfo* ri : { info.first, info.second })
			{
				if (ri)
				{
					index = MapManager::Get().GetRoomIndex(ri);

					std::shared_ptr<GameLevel> level = Cast<GameLevel>(Engine::Get().GetLevel().lock());
					const std::list<std::weak_ptr<Enemy>>& enemies = level->GetEnemies();

					auto iter = enemies.begin();

					while (iter != enemies.end())
					{
						std::shared_ptr<Enemy> enemy = iter->lock();

						if (!enemy)
						{
							level->EraseEnemy(enemy);
							iter = enemies.begin();
						}

						if (enemy && enemy->GetPosition() == targetPos)
						{
							level->EraseEnemy(enemy);
							enemy->Destroy();
							TurnManager::Get().SetTurnType(TurnManager::Turn::EnemyTurn);
							break;
						}

						++iter;
					}
				}
			}
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

	const std::pair<RoomInfo*, RoomInfo*>& info = MapManager::Get().FindRoomInfo(pos);

	bool visited1 = false, visited2 = false;
	int index1 = -1, index2 = -1;

	if (info.first)
	{
		index1 = MapManager::Get().GetRoomIndex(info.first);
		visited1 = MapManager::Get().GetRoom(index1).lock()->IsVisited();
	}
		
	if (info.second)
	{
		index2 = MapManager::Get().GetRoomIndex(info.second);
		visited2 = MapManager::Get().GetRoom(index2).lock()->IsVisited();
	}
		
	if (!visited1 && !visited2 && !MapManager::Get().IsDebugMode())
		return;

	_attack = false;

	if (info.first)
	{
		std::shared_ptr<GameLevel> level = Cast<GameLevel>(Engine::Get().GetLevel().lock());

		const std::list<std::weak_ptr<Enemy>>& enemies = level->GetEnemies();

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
			
			if (enemy->GetPosition() == pos)
			{
				_attack = true;
				break;
			}

			++iter;
		}
	}

	RequestPathFind(pos, _attack);

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

void Player::RequestPathFind(const Craft::Vector2& cursorPos, bool isAttack)
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

	if (goalIndex < 0)
		return;

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
				std::vector<Vector2> obstacles;
				BuildObstacles(info, obstacles, isAttack, cursorPos);

				if (!AStar::Get().FindPath(position, cursorPos, rect, obstacles, _path))
					return;
			}
			else
			{
				Vector2 prevPos = position;
				for (size_t i = 0; i < route.size() - 1; i++)
				{
					std::deque<Vector2> tmp;
					Vector2 doorPos;
					const std::pair<RoomInfo*, RoomInfo*>& infos = MapManager::Get().FindRoomInfo(prevPos);
					RoomInfo* info = infos.first;

					if (!info)
						return;

					// 문이라 방 두개 검출되면 루트상 마지막거
					info = ResolveRoomOnRoute(infos, route);

					Rect rect = info->_rect;

					std::vector<Vector2> obstacles;

					BuildObstacles(info, obstacles, isAttack, cursorPos);

					MapManager::Get().FindDoorPosition(route[i], route[i + 1], doorPos);

					if (!AStar::Get().FindPath(prevPos, doorPos, rect, obstacles, tmp))
						return;

					_path.insert(_path.end(), tmp.begin(), tmp.end());
					prevPos = doorPos;
				}
				if (_path.size() == 0)
					return;

				Vector2 currentPos = _path[_path.size() - 1];
				std::deque<Vector2> tmp;
				const std::pair<RoomInfo*, RoomInfo*>& infos = MapManager::Get().FindRoomInfo(currentPos);
				RoomInfo* info = infos.first;
				
				if (!info)
					return;

				info = ResolveRoomOnRoute(infos, route);

				Rect rect = info->_rect;

				std::vector<Vector2> obstacles;

				BuildObstacles(info, obstacles, isAttack, cursorPos);

				if (!AStar::Get().FindPath(currentPos, cursorPos, rect, obstacles, tmp))
					return;

				_path.insert(_path.end(), tmp.begin(), tmp.end());
			}
		}
	}
}

RoomInfo* Player::ResolveRoomOnRoute(const std::pair<RoomInfo*, RoomInfo*>& infos, const std::vector<RoomInfo*>& route)
{
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

	return info;
}

void Player::BuildObstacles(const RoomInfo* const info, std::vector<Craft::Vector2>& outObstacles, bool isAttack, const Craft::Vector2& cursorPos)
{
	const std::vector<Vector2>& walls = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info)).lock()->GetWalls();

	std::shared_ptr<GameLevel> level = Cast<GameLevel>(Engine::Get().GetLevel().lock());
	const std::list<std::weak_ptr<Enemy>>& enemies = level->GetEnemies();

	outObstacles.reserve(walls.size() + enemies.size());
	outObstacles.insert(outObstacles.end(), walls.begin(), walls.end());

	auto iter = enemies.begin();
	while (iter != enemies.end())
	{
		std::shared_ptr<Enemy> enemy = iter->lock();
		if (enemy && isAttack && enemy->GetPosition() == cursorPos)
		{
			++iter;
			continue;
		}

		if (enemy->GetPosition().x < info->_rect._left || enemy->GetPosition().x > info->_rect._right || 
			enemy->GetPosition().y < info->_rect._top || enemy->GetPosition().y > info->_rect._bottom)
		{
			++iter;
			continue;
		}


		outObstacles.emplace_back(enemy->GetPosition());
		++iter;
	}
}
