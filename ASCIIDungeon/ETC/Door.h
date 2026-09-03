#pragma once
#include "Room.h"
#include <Math/Vector2.h>

struct Door
{
	std::pair<Room*, Room*> _parents;
	Craft::Vector2 _position;
};