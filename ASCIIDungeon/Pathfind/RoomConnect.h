#pragma once
#include <deque>
#include <memory>
#include <ETC/RoomInfo.h>

struct Node
{
	RoomInfo* _room = nullptr;
	Node* _parent = nullptr;
};

class RoomConnect
{
public:
	RoomConnect();
	~RoomConnect();

	bool ConnectRooms(RoomInfo*& entrance, RoomInfo*& exit, std::vector<RoomInfo*>& outPath, bool isConnected = true);	// 방 연결
	void ConstructPath(Node* destination, std::vector<RoomInfo*>& outPath);
	void ClearCost();
	void Clear();

	bool IsInClosedList(const RoomInfo* const room) const;
	Node* FindOpenNode(const RoomInfo* const room);

	static RoomConnect& Get();

private:
	inline static std::unique_ptr<RoomConnect> _instance;
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

