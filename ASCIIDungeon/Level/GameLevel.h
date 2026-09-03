#pragma once
#include <Level/Level.h>
#include <ETC/Rect.h>
#include <ETC/Door.h>

template<typename T>
class BSPTree;

template<typename T>
class BSPNode;

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
	void ConnectRooms();
	void ConnectPath(std::vector<Room*>& path, int cost = 1);
	void CreateDoors();
	bool HasDoor(const Room* const a, const Room* const b) const;
private:
	BSPTree<Rect>* _bsp;
	std::vector<Room*> _connectedRooms;
	std::vector<Door> _doors;
};

