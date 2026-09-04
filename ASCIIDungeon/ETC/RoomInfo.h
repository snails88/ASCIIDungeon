#pragma once
#include <vector>
#include <ETC/Rect.h>

struct RoomInfo
{
	enum class RoomType { NONE, ENTRANCE, EXIT, STANDARD, PATH };
	Rect _rect = { 0, 0, 0, 0 };
	RoomType _type = RoomType::NONE;
	int _cost = 1;
	std::vector<RoomInfo*> _neighbors;
	std::vector<RoomInfo*> _connected;
};