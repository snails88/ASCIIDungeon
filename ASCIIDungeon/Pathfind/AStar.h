#pragma once
#include <deque>
#include <vector>
#include <memory>
#include <Math/Vector2.h>
#include <ETC/Rect.h>

using namespace Craft;

class AStar
{
private:
	struct Node
	{
		Vector2 _position;
		// 대각선 비용도 1이라 int여도 상관없지만 추후 확장성 고려해서 float
		float _gCost = 0.f;
		float _hCost = 0.f;

		Node* _parent = nullptr;
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

	bool FindPath(const Vector2& startPos, const Vector2& goalPos, const Rect& rect, const std::vector<Vector2>& blockedList, std::deque<Vector2>& outPath);

	static AStar& Get();

private:
	void ConstructPath(Node* destination, std::deque<Vector2>& outPath) const;
	bool IsInClosedList(const Vector2& pos) const;
	bool IsBlocked(const Vector2& pos, const std::vector<Vector2>& blockedList) const;
	bool IsDiagonalBlocked(const Vector2& current, const Vector2& dir, const std::vector<Vector2>& blockedList) const;
	bool IsInRange(const Vector2& pos, const Rect& rect);
	void Clear();
	float CalculateHeuristic(const Vector2& current, const Vector2& goal) const;
	int FindOpenNodeIndex(const Vector2& pos) const;

private:
	const float _diagonalCost = 1.41421f;
	inline static std::unique_ptr<AStar> _instance;
	std::deque<Node> _openList;
	std::deque<Node> _closedList;
};

