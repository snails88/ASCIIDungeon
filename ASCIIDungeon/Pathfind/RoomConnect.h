#pragma once
#include <deque>
#include <ETC/Room.h>

struct Node
{
	Room* _room = nullptr;
	Node* _parent = nullptr;
};

class RoomConnect
{
public:
	RoomConnect();
	~RoomConnect();

	bool ConnectRooms(Room*& entrance, Room*& exit, std::vector<Room*>& outPath);	// 방 연결
	void ConstructPath(Node* destination, std::vector<Room*>& outPath);
	void Clear();

	bool IsInClosedList(const Room* const room) const;
	Node* FindOpenNode(const Room* const room);
private:
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

