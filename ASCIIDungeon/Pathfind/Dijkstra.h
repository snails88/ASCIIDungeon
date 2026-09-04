#pragma once
#include <deque>
#include <memory>
#include <ETC/RoomInfo.h>

class Dijkstra
{
private:
	struct Node
	{
		RoomInfo* _room = nullptr;
		Node* _parent = nullptr;
	};

public:
	Dijkstra();
	~Dijkstra();

	bool FindRoute(RoomInfo*& entrance, RoomInfo*& exit, std::vector<RoomInfo*>& outRoute, bool isConnected = true);	// 방 연결
	
	static Dijkstra& Get();

private:
	void ConstructRoute(Node* destination, std::vector<RoomInfo*>& outRoute);
	bool IsInClosedList(const RoomInfo* const room) const;
	Node* FindOpenNode(const RoomInfo* const room);
	void ClearCost();
	void Clear();

private:
	inline static std::unique_ptr<Dijkstra> _instance;
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

