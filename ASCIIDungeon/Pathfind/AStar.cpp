#include "AStar.h"
#include <Level/GameLevel.h>
#include <algorithm>
#include <Define.h>

AStar::AStar()
{
}

AStar::~AStar()
{
    Clear();
}

bool AStar::FindPath(const Vector2& startPos, const Vector2& goalPos, const Rect& rect, const std::vector<Vector2>& blockedList, std::deque<Vector2>& outPath, bool allowDiagonal)
{
    Clear();

    if (IsBlocked(goalPos, blockedList))
        return false;

    Node startNode{ startPos };
    Node GoalNode{ goalPos };

    startNode._hCost = CalculateHeuristic(startPos, goalPos);

    _openList.emplace_back(startNode);

    static const std::vector<Vector2> straightDirs =
    {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}
    };

    static const std::vector<Vector2> allDirs =
    {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
    };

    const std::vector<Vector2>& dirs = allowDiagonal ? allDirs : straightDirs;

    while (!_openList.empty())
    {
        Node* currentNode = &_openList[0];
        float currentFCost = currentNode->_gCost + currentNode->_hCost;
        for (Node& node : _openList)
        {
            float fCost = node._gCost + node._hCost;

            if (fCost < currentFCost || (fCost == currentFCost && node._hCost < currentNode->_hCost))
            {
                currentNode = &node;
                currentFCost = fCost;
            }
        }

        if (currentNode->_position == goalPos)
        {
            ConstructPath(currentNode, outPath);
            Clear();
            return true;
        }

        auto iter = _openList.begin();

        while (iter != _openList.end())
        {
            if (iter->_position == currentNode->_position)
                break;

            ++iter;
        }

        _closedList.emplace_back(*currentNode);

        if (iter != _openList.end())
            _openList.erase(iter);

        currentNode = &_closedList.back();

        for (const Vector2& dir : dirs)
        {
            Vector2 newPos = currentNode->_position + dir;

            if (!IsInRange(newPos, rect))
                continue;

            if (IsBlocked(newPos, blockedList))
                continue;

            if (IsDiagonalBlocked(currentNode->_position, dir, blockedList))
                continue;

            if (IsInClosedList(newPos))
                continue;

            float gCost = (dir.x == 0 || dir.y == 0) ? 1.f : _diagonalCost;

            float newGCost = currentNode->_gCost + gCost;

            int index = FindOpenNodeIndex(newPos);
            if (index >= 0)
            {
                Node& openNode = _openList[index];

                if (newGCost < openNode._gCost)
                {
                    openNode._gCost = newGCost;
                    openNode._parent = currentNode;
                }

                continue;
            }

            Node neighborNode = Node{ newPos, newGCost, CalculateHeuristic(newPos, goalPos, allowDiagonal), currentNode};

            _openList.emplace_back(neighborNode);
        }
    }

    return false;
}

AStar& AStar::Get()
{
    if (!_instance)
        _instance = std::make_unique<AStar>();

    return *_instance;
}

void AStar::ConstructPath(Node* destination, std::deque<Vector2>& outPath) const
{
    Node* current = destination;

    while (current)
    {
        outPath.emplace_back(current->_position);
        current = current->_parent;
    }

    std::reverse(outPath.begin(), outPath.end());
}

bool AStar::IsInClosedList(const Vector2& pos) const
{
    for (size_t i = 0; i < _closedList.size(); i++)
    {
        if (_closedList[i]._position == pos)
            return true;
    }

    return false;
}

bool AStar::IsBlocked(const Vector2& pos, const std::vector<Vector2>& blockedList) const
{
    return blockedList.end() != std::find(blockedList.begin(), blockedList.end(), pos);
}

bool AStar::IsDiagonalBlocked(const Vector2& current, const Vector2& dir, const std::vector<Vector2>& blockedList) const
{
    if (dir.x == 0 || dir.y == 0)
        return false;

    Vector2 xVec = current;
    xVec.x += dir.x;
    Vector2 yVec = current;
    yVec.y += dir.y;

    bool foundX = blockedList.end() != std::find(blockedList.begin(), blockedList.end(), xVec);
    bool foundY = blockedList.end() != std::find(blockedList.begin(), blockedList.end(), yVec);
    
    return foundX && foundY;
}

bool AStar::IsInRange(const Vector2& pos, const Rect& rect)
{
    return !(pos.x < rect._left || pos.x > rect._right || pos.y < rect._top || pos.y > rect._bottom);
}

void AStar::Clear()
{
    _openList.clear();
    _closedList.clear();
}

float AStar::CalculateHeuristic(const Vector2& current, const Vector2& goal, bool allowDiagonal) const
{
    int diffX = std::abs(current.x - goal.x);
    int diffY = std::abs(current.y - goal.y);

    if (!allowDiagonal)
        return static_cast<float>(diffX + diffY);

    int diagonalDistance = min(diffX, diffY);
    int straightDistance = max(diffX, diffY) - diagonalDistance;


    return diagonalDistance * _diagonalCost + straightDistance * 1.f;
}

int AStar::FindOpenNodeIndex(const Vector2& pos) const
{
    for (size_t i = 0; i < _openList.size(); i++)
    {
        if (_openList[i]._position == pos)
            return i;
    }
    return -1;
}
