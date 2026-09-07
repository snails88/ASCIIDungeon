#include "Enemy.h"
#include <Level/Level.h>
#include <Actor/Player.h>
#include <Actor/Room.h>
#include <Manager/MapManager.h>
#include <Manager/TurnManager.h>
#include <Pathfind/AStar.h>
#include <Define.h>

using namespace Craft;

Enemy::Enemy(const Craft::Vector2& pos)
	: super("M", pos, Color::Magenta)
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
		//Vector2 nextPos = 

	}
}

void Enemy::Draw()
{
	std::weak_ptr<Room> room = MapManager::Get().GetRoom(MapManager::Get().GetRoomIndex(MapManager::Get().FindRoomInfo(position).first));
	if (!room.lock()->IsVisible() && !MapManager::Get().IsDebugMode())
		return;
	super::Draw();
}
