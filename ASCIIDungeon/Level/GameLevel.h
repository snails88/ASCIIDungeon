#pragma once
#include <Level/Level.h>
#include <ETC/Rect.h>
#include <ETC/Door.h>

template<typename T>
class BSPTree;

template<typename T>
class BSPNode;

class Room;
class Player;

using namespace Craft;
class GameLevel : public Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	enum class Turn
	{
		PlayerTurn,
		EnemyTurn,
		Delay
	};

	virtual ~GameLevel();

	void ResetActors();

	std::pair<RoomInfo*, RoomInfo*> const FindRoomInfo(const Craft::Vector2& pos) const;
	int GetRoomIndex(const RoomInfo* const info) const;
	std::weak_ptr<Room> const GetRoom(int index) const;
	bool FindDoorPosition(const RoomInfo* const parentA, const RoomInfo* const parentB, Vector2& outPos) const;
	
	inline void SetTurnType(Turn type) { _turn = type; }
	inline bool IsPlayerTurn() const { return _turn == Turn::PlayerTurn; }

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
	/// 나중에 턴 매니저로 옮길 수 있음
	const float _maxTurnDelay = 0.1f;
	float _turnDelay = 0.f;
	Turn _turn = Turn::PlayerTurn;
	Player* _player = nullptr;
	///

	BSPTree<Rect>* _bsp;
	std::vector<RoomInfo*> _connectedRooms;		// 연결된 방 정보들
	std::vector<std::weak_ptr<Room>> _rooms;	// 위 정보로 생성한 방 인스턴스들
	std::vector<Door> _doors;
};

