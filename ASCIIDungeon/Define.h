#pragma once

const int MIN_PARTITION_SIZE = 120;
const BYTE MIN_PATH_SIZE = 7;
const BYTE MIN_ROOM_COUNT = 15;
const BYTE CORRIDOR_COUNT = 3;
const BYTE ENEMY_COUNT = 10;

namespace Sort
{
	enum SortingOrder
	{
		Default,
		Floor,
		Wall,
		Door,
		Stairs,
		Visualize,
		Monster,
		Player,
		Cursor,
	};
}