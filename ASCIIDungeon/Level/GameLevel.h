#pragma once
#include <Level/Level.h>
#include <ETC/Rect.h>
#include <ETC/Door.h>

template<typename T>
class BSPTree;

template<typename T>
class BSPNode;

class Room;

using namespace Craft;
class GameLevel : public Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	virtual ~GameLevel();

	void ResetActors();
private:
	virtual void OnInitialized() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:	// 맵 생성 관련 함수들
	void Split(BSPNode<Rect>& parent);
	void SetNeighbor();
	bool IsNeighbor(const Rect& a, const Rect& b);
	void ConnectRooms(RoomInfo*& outEntrance, RoomInfo*& outExit);
	void ConnectPath(std::vector<RoomInfo*>& path, int cost = 1);
	void CreateDoors();
	bool HasDoor(const RoomInfo* const a, const RoomInfo* const b) const;
private:
	BSPTree<Rect>* _bsp;
	std::vector<RoomInfo*> _connectedRooms;		// 연결된 방 정보들
	std::vector<std::weak_ptr<Room>> _rooms;	// 위 정보로 생성한 방 인스턴스들
	std::vector<Door> _doors;
};

