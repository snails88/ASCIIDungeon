#pragma once
#include <vector>
#include <ETC/Rect.h>

struct Room
{
	enum class RoomType { NONE, ENTRANCE, EXIT, STANDARD, PATH };
	Rect _rect = { 0, 0, 0, 0 };
	RoomType _type = RoomType::NONE;
	int _cost = 1;
	std::vector<Room*> _neighbors;
	std::vector<Room*> _connected;
};