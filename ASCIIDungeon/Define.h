#pragma once

const int MIN_PARTITION_SIZE = 120;
const BYTE MIN_PATH_SIZE = 7;
const BYTE MIN_ROOM_COUNT = 14;

namespace Sort
{
	enum SortingOrder
	{
		Default,
		Door,
		Stairs,
		Visualize,
		Monster,
		Cursor,
		Player
	};
}