#pragma once
#include <memory>
#include <ETC/RoomInfo.h>
#include <Math/Vector2.h>
#include <ETC/Door.h>

template<typename T>
class BSPNode;

template<typename T>
class BSPTree;

class Room;

class MapManager
{
public:
	MapManager();
	~MapManager();

	void BeginPlay();
	std::pair<RoomInfo*, RoomInfo*> const FindRoomInfo(const Craft::Vector2& pos) const;
	int GetRoomIndex(const RoomInfo* const info) const;
	bool FindDoorPosition(const RoomInfo* const parentA, const RoomInfo* const parentB, Craft::Vector2& outPos) const;

	inline std::weak_ptr<Room> const GetRoom(int index) const { return _rooms[index]; }

	static MapManager& Get();

private:
private:	// 맵 생성 관련 함수들
	void Split(BSPNode<Rect>& parent);
	void SetNeighbor();
	bool IsNeighbor(const Rect& a, const Rect& b);
	void ConnectRooms(RoomInfo*& outEntrance, RoomInfo*& outExit);
	void ConnectPath(std::vector<RoomInfo*>& path, int cost = 1);
	void CreateDoors();
	bool HasDoor(const RoomInfo* const a, const RoomInfo* const b) const;

private:
	inline static std::unique_ptr<MapManager> _instance = nullptr;

	BSPTree<Rect>* _bsp;
	std::vector<RoomInfo*> _connectedRooms;		// 연결된 방 정보들
	std::vector<std::weak_ptr<Room>> _rooms;	// 위 정보로 생성한 방 인스턴스들
	std::vector<Door> _doors;
};

