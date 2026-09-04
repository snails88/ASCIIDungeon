#pragma once
#include <deque>
#include <memory>
#include <ETC/RoomInfo.h>

struct Node
{
	RoomInfo* _room = nullptr;
	Node* _parent = nullptr;
};

class Dijkstra
{
public:
	Dijkstra();
	~Dijkstra();

	bool FindRoute(RoomInfo*& entrance, RoomInfo*& exit, std::vector<RoomInfo*>& outRoute, bool isConnected = true);	// 방 연결
	void ConstructRoute(Node* destination, std::vector<RoomInfo*>& outRoute);
	void ClearCost();
	void Clear();

	bool IsInClosedList(const RoomInfo* const room) const;
	Node* FindOpenNode(const RoomInfo* const room);

	static Dijkstra& Get();

private:
	inline static std::unique_ptr<Dijkstra> _instance;
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

