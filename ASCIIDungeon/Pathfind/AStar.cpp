#include "AStar.h"
#include <Level/GameLevel.h>

AStar::AStar()
{
}

AStar::~AStar()
{
    Clear();
}

bool AStar::FindPath(const Vector2& startPos, const Vector2& goalPos, const Rect& rect, const std::vector<Vector2>& blockedList, std::vector<Vector2>& outPath)
{
    Clear();


    return false;
}

AStar& AStar::Get()
{
    if (!_instance)
        _instance = std::make_unique<AStar>();

    return *_instance;
}

void AStar::ConstructPath(Node* destination, std::vector<Vector2>& outPath)
{
}

bool AStar::IsInClosedList(const Vector2& pos)
{
    return false;
}

bool AStar::IsDiagonalBlocked(const Vector2& current, const Direction& dir)
{
    return false;
}

void AStar::Clear()
{
    _openList.clear();
    _closedList.clear();
}
