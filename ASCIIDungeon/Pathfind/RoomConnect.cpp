#include "RoomConnect.h"
#include <ETC/Room.h>

RoomConnect::RoomConnect()
{
}

RoomConnect::~RoomConnect()
{
    Clear();
}

bool RoomConnect::ConnectRooms(Room*& entrance, Room*& exit, std::vector<Room*>& outPath)
{
    Clear();

    Node entranceNode;
    entranceNode._room = entrance;
    entranceNode._room->_cost = 0;

    _openList.emplace_back(entranceNode);

    while (!_openList.empty())
    {
        Node* current = &_openList[0];

        for (Node& node : _openList)
        {
            if (node._room->_cost < current->_room->_cost)
                current = &node;
        }

        if (current->_room == exit)
        {
            ConstructPath(current, outPath);
            return true;
        }

        auto iter = _openList.begin();
        auto iter_end = _openList.end();

        while (iter != iter_end)
        {
            if (current->_room == iter->_room)
                break;

            ++iter;
        }

        _closedList.emplace_back(*current);

        if (iter != _openList.end())
            _openList.erase(iter);

        current = &_closedList.back();

        for (int i = 0; i < current->_room->_neighbors.size(); i++)
        {
            Room* room = current->_room->_neighbors[i];

            if (IsInClosedList(room))
                continue;

            int newCost = current->_room->_cost + room->_cost;

            Node* openNode = FindOpenNode(room);
            if (openNode)
            {
                if (newCost < openNode->_room->_cost)
                {
                    openNode->_room->_cost = newCost;
                    openNode->_parent = current;
                }

                continue;
            }
            Node newNode;
            newNode._room = room;
            newNode._room->_cost = newCost;
            newNode._parent = current;
            _openList.emplace_back(newNode);
        }
    }

    return false;
}

void RoomConnect::ConstructPath(Node* destination, std::vector<Room*>& outPath)
{
    Node* current = destination;
    while (current)
    {
        outPath.emplace_back(current->_room);
        current = current->_parent;
    }
    std::reverse(outPath.begin(), outPath.end());
}

void RoomConnect::Clear()
{
    _openList.clear();
    _closedList.clear();
}

bool RoomConnect::IsInClosedList(const Room* const room) const
{
    for (const Node& n : _closedList)
    {
        if (n._room == room)
            return true;
    }
    return false;
}

Node* RoomConnect::FindOpenNode(const Room* const room)
{
    auto iter = _openList.begin();
    auto iter_end = _openList.end();

    while (iter != iter_end)
    {
        if (iter->_room == room)
            return &(*iter);

        ++iter;
    }

    return nullptr;
}
