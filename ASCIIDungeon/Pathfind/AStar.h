#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <Math/Vector2.h>

using namespace Craft;

class AStar
{
private:
	struct Node
	{
		Vector2 _position;
		float _gCost = 0.f;
		float _hCost = 0.f;

		Node* _parent = nullptr;
	};

	struct Direction
	{
		int _x = 0;
		int _y = 0;
		float _cost = 0.f;
	};

	enum class TileType
	{
		Floor,
		Blocked,
		Start,
		Goal,
		Visited
	};

public:
	AStar();
	~AStar();

	bool FindPath(const Vector2& startPos, const Vector2& goalPos, const Rect& rect, const std::vector<Vector2>& blockedList, std::vector<Vector2>& outPath);

	static AStar& Get();

private:
	void ConstructPath(Node* destination, std::vector<Vector2>& outPath);
	bool IsInClosedList(const Vector2& pos);
	bool IsDiagonalBlocked(const Vector2& current, const Direction& dir);
	void Clear();


private:
	inline static std::unique_ptr<AStar> _instance;
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

