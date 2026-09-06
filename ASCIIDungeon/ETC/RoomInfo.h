#pragma once
#include <vector>
#include <ETC/Rect.h>

struct RoomInfo
{
	enum class RoomType { ENTRANCE, EXIT, STANDARD, CORRIDOR };
	Rect _rect = { 0, 0, 0, 0 };
	RoomType _type = RoomType::STANDARD;
	int _cost = 1;
	std::vector<RoomInfo*> _neighbors;
	std::vector<RoomInfo*> _connected;
};