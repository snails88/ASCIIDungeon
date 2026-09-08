#pragma once
#include <Actor/Actor.h>
#include <ETC/RoomInfo.h>

class Player;

class Enemy : public Craft::Actor, public std::enable_shared_from_this<Enemy>
{
	TYPE_DECLARATIONS(Enemy, Actor)

public:
	Enemy(const Craft::Vector2& pos);

	virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	inline void SetCurrentRoomInfo(std::pair<RoomInfo*, RoomInfo*> info) { _currentRoom = info; }

	inline void SetSleep(bool isSleep) { _isSleep = isSleep; }
	inline const bool IsSleep() const { return _isSleep; }

private:
private:
	RoomInfo* ResolveRoomOnRoute(const std::pair<RoomInfo*, RoomInfo*>& infos, const std::vector<RoomInfo*>& route);
	void BuildObstacles(const RoomInfo* const info, std::vector<Craft::Vector2>& outObstacles, const Craft::Vector2& pos);

private:
	bool _isSleep = true;
	std::pair<RoomInfo*, RoomInfo*> _currentRoom;
	std::weak_ptr<Player> _player;
};

