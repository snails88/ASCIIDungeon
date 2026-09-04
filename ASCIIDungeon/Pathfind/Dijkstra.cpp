#include "Dijkstra.h"
#include <ETC/RoomInfo.h>

Dijkstra::Dijkstra()
{
}

Dijkstra::~Dijkstra()
{
    Clear();
}

bool Dijkstra::FindRoute(RoomInfo*& entrance, RoomInfo*& exit, std::vector<RoomInfo*>& outRoute, bool isConnected)
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
            ConstructRoute(current, outRoute);
            ClearCost();
            Clear();
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

        std::vector<RoomInfo*>& near = isConnected ? current->_room->_connected : current->_room->_neighbors;

        for (int i = 0; i < near.size(); i++)
        {
            RoomInfo* room = near[i];

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

    ClearCost();
    Clear();
    return false;
}

void Dijkstra::ConstructRoute(Node* destination, std::vector<RoomInfo*>& outRoute)
{
    Node* current = destination;
    while (current)
    {
        outRoute.emplace_back(current->_room);
        current = current->_parent;
    }
    std::reverse(outRoute.begin(), outRoute.end());
}

void Dijkstra::ClearCost()
{
    for (size_t i = 0; i < _openList.size(); i++)
        _openList[i]._room->_cost = 1;

    for (size_t i = 0; i < _closedList.size(); i++)
        _closedList[i]._room->_cost = 1;
}

void Dijkstra::Clear()
{
    _openList.clear();
    _closedList.clear();
}

bool Dijkstra::IsInClosedList(const RoomInfo* const room) const
{
    for (const Node& n : _closedList)
    {
        if (n._room == room)
            return true;
    }
    return false;
}

Node* Dijkstra::FindOpenNode(const RoomInfo* const room)
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

Dijkstra& Dijkstra::Get()
{
    if (!_instance)
        _instance = std::make_unique<Dijkstra>();

    return *_instance;
}
