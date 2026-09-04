#pragma once
#include <ETC/RoomInfo.h>
#include <Math/Vector2.h>

struct Door
{
	std::pair<RoomInfo*, RoomInfo*> _parents;
	Craft::Vector2 _position;
};