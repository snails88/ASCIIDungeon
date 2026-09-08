#include "Enemy.h"
#include <Level/GameLevel.h>
#include <Actor/Player.h>
#include <Actor/Room.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Pathfind/Dijkstra.h>
#include <Pathfind/AStar.h>
#include <Define.h>
#include <Engine/Engine.h>

using namespace Craft;

Enemy::Enemy(const Craft::Vector2& pos)
	: super("M", pos, Color::Magenta | Color::B_White)
{
	sortingOrder = Sort::Monster;
}

void Enemy::BeginPlay()
{
	super::BeginPlay();
	_player = GetOwner()->FindActor<Player>();
}

void Enemy::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if (!_isSleep && TurnManager::Get().GetCurrentTurn() == TurnManager::Turn::EnemyTurn)
	{
		std::shared_ptr<Player> player = _player.lock();
		auto infos = MapManager::Get().FindRoomInfo(position);
		auto playerRoomInfos = MapManager::Get().FindRoomInfo(player->GetPosition());

		std::vector<RoomInfo*> route;
		std::deque<Vector2> path;

		if (Dijkstra::Get().FindRoute(infos.first, playerRoomInfos, route))
		{
			if (route.size() == 1)	// 방 내 이동
			{
				RoomInfo* info = infos.first;
				Rect rect = info->_rect;
				std::vector<Vector2> obstacles;
				BuildObstacles(info, obstacles, position);

				if (!AStar::Get().FindPath(position, player->GetPosition(), rect, obstacles, path))
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

					BuildObstacles(info, obstacles, position);

					MapManager::Get().FindDoorPosition(route[i], route[i + 1], doorPos);

					if (!AStar::Get().FindPath(prevPos, doorPos, rect, obstacles, tmp))
						return;

					path.insert(path.end(), tmp.begin(), tmp.end());
					prevPos = doorPos;
				}
				if (path.size() == 0)
					return;

				Vector2 currentPos = path[path.size() - 1];
				std::deque<Vector2> tmp;
				const std::pair<RoomInfo*, RoomInfo*>& infos = MapManager::Get().FindRoomInfo(currentPos);
				
				RoomInfo* info = infos.first;

				if (!info)
					return;

				// 문이라 방 두개 검출되면 루트상 마지막거
				info = ResolveRoomOnRoute(infos, route);

				Rect rect = info->_rect;

				std::vector<Vector2> obstacles;
				BuildObstacles(info, obstacles, position);

				if (!AStar::Get().FindPath(currentPos, player->GetPosition(), rect, obstacles, tmp))
					return;

				path.insert(path.end(), tmp.begin(), tmp.end());
			}
		}
	
		if (!path.empty())
		{
			path.pop_front();
			if (path.size() == 1 || path.empty())
			{
				_player.lock()->StopMove();
			}
			else
			{
				if(position == path.front())
					path.pop_front();

				position = path.front();
			}
		}
		_isSleep = true;	
	}
}

void Enemy::Draw()
{
	bool visible = false;
	const std::pair<RoomInfo*, RoomInfo*>& infos = MapManager::Get().FindRoomInfo(position);
	for (RoomInfo* info : { infos.first, infos.second })
	{
		if (!info)
			break;

		if (MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(info)).lock()->IsVisible())
			visible = true;
	}

	if (!visible && !MapManager::Get().IsDebugMode())
		return;
	super::Draw();
}

RoomInfo* Enemy::ResolveRoomOnRoute(const std::pair<RoomInfo*, RoomInfo*>& infos, const std::vector<RoomInfo*>& route)
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

void Enemy::BuildObstacles(const RoomInfo* const info, std::vector<Craft::Vector2>& outObstacles, const Craft::Vector2& pos)
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
		if (enemy && enemy->GetPosition() == pos)
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
