#include <algorithm>
#include "GameLevel.h"
#include <Game/Game.h>
#include <ETC/BSP.h>
#include <Define.h>
#include <Util/Util.h>
#include <Pathfind/Dijkstra.h>
#include <Actor/Player.h>
#include <Actor/Stairs.h>
#include <Actor/Cursor.h>
#include <Actor/InputManager.h>
#include <Actor/Room.h>

GameLevel::~GameLevel()
{
	delete _bsp;
	_bsp = nullptr;
}

void GameLevel::ResetActors()
{
	for (const std::shared_ptr<Actor>& actor : actorList)
	{
		actor->SetActive(false);
	}
}

std::pair<RoomInfo*, RoomInfo*> const GameLevel::FindRoomInfo(const Craft::Vector2& pos) const
{
	RoomInfo* first = nullptr;
	RoomInfo* second = nullptr;

	for (size_t i = 0; i < _connectedRooms.size(); i++)
	{
		const Rect& current = _connectedRooms[i]->_rect;

		if (pos.x >= current._left && pos.x <= current._right &&
			pos.y >= current._top && pos.y <= current._bottom)
		{
			if (!first)
				first = _connectedRooms[i];
			else
			{
				second = _connectedRooms[i];
				break;
			}
		}
			
	}

	return std::make_pair(first, second);
}

int GameLevel::GetRoomIndex(const RoomInfo* const info) const
{
	for (size_t i = 0; i < _connectedRooms.size(); i++)
	{
		if (info == _connectedRooms[i])
			return i;
	}

	return -1;
}

std::weak_ptr<Room> const GameLevel::GetRoom(int index) const
{
	return _rooms[index];
}

bool GameLevel::FindDoorPosition(const RoomInfo* parentA, const RoomInfo* const parentB, Vector2& outPos) const
{
	for (size_t i = 0; i < _doors.size(); i++)
	{
		if ((parentA == _doors[i]._parents.first && parentB == _doors[i]._parents.second) ||
			(parentB == _doors[i]._parents.first && parentA == _doors[i]._parents.second))
		{
			outPos = _doors[i]._position;
			return true;
		}
	}
	return false;
}


void GameLevel::OnInitialized()
{
	super::OnInitialized();

	Util::SetRandomSeed();

	Game& engine = static_cast<Game&>(Engine::Get());

	Rect rect;
	rect._left = 0;
	rect._right = engine.GetWidth() - 1;
	rect._top = 0;
	rect._bottom = engine.GetHeight() - 1;
	
	_bsp = new BSPTree<Rect>(rect);
	_connectedRooms.reserve(MIN_ROOM_COUNT);
	RoomInfo* entrance = nullptr;
	RoomInfo* exit = nullptr;

	Split(*(_bsp->GetRoot()));
 	SetNeighbor();
	ConnectRooms(entrance, exit);
	CreateDoors();

	int startPosX = entrance->_rect._left + ((entrance->_rect._right - entrance->_rect._left) / 2);
	int startPosY = entrance->_rect._top + ((entrance->_rect._bottom - entrance->_rect._top) / 2);

	std::shared_ptr<Player> player = SpawnActor<Player>(Vector2(startPosX, startPosY));
	std::shared_ptr<Cursor> cursor = SpawnActor<Cursor>(Vector2(startPosX, startPosY));

	startPosX = exit->_rect._left + ((exit->_rect._right - exit->_rect._left) / 2);
	startPosY = exit->_rect._top + ((exit->_rect._bottom - exit->_rect._top) / 2);

	std::shared_ptr<Stairs> stairs = SpawnActor<Stairs>(Vector2(startPosX, startPosY));
	std::shared_ptr<InputManager> inputManager = SpawnActor<InputManager>();

	for (size_t i = 0; i < _connectedRooms.size(); i++)
		_rooms.emplace_back(SpawnActor<Room>(_connectedRooms[i]->_rect));

	for (size_t i = 0; i < _doors.size(); i++)
	{
		for (size_t j = 0; j < _connectedRooms.size(); j++)
		{
			if (_doors[i]._parents.first == _connectedRooms[j])
				_rooms[j].lock()->AddDoor(_doors[i]._position);

			if (_doors[i]._parents.second == _connectedRooms[j])
				_rooms[j].lock()->AddDoor(_doors[i]._position);
		}
	}
}

void GameLevel::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	Game& engine = static_cast<Game&>(Engine::Get());
	if (engine.IsGameOver())
	{
		ResetActors();
		engine.GameOver();
	}
}

void GameLevel::Draw()
{
	super::Draw();
}

void GameLevel::Split(BSPNode<Rect>& parent)
{
	int width = parent.GetData()._right - parent.GetData()._left;
	int height = parent.GetData()._bottom - parent.GetData()._top;

	int equalWidthSplit = 0;

	if (width == height * 2)
	{
		equalWidthSplit = Util::RandomRange(0, 1);
	}

	if (width * height < MIN_PARTITION_SIZE)
	{
		_bsp->AddLeaf(parent);
		parent.AddRoom(parent.GetData());
		return;
	}
	else if (width > height * 2 || (width == height * 2 && equalWidthSplit))
	{
		float widthf = static_cast<float>(width);

		int randSplit = static_cast<int>(Util::RandomRange(widthf * 0.3f, widthf * 0.7f));

		Rect rectLeft = parent.GetData();
		rectLeft._right = rectLeft._left + randSplit;
		BSPNode<Rect>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		Rect rectRight = parent.GetData();
		rectRight._left = rectLeft._right;
		BSPNode<Rect>* rightChild = _bsp->AddRightChild(&parent, rectRight);

		Split(*leftChild);
		Split(*rightChild);
	}
	else if (height * 2 > width || (width == height * 2 && !equalWidthSplit))
	{
		float heightf = static_cast<float>(height);

		int randSplit = static_cast<int>(Util::RandomRange(heightf * 0.4f, heightf * 0.6f));

		Rect rectLeft = parent.GetData();
		rectLeft._bottom = rectLeft._top + randSplit;
		BSPNode<Rect>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		Rect rectRight = parent.GetData();
		rectRight._top = rectLeft._bottom;
		BSPNode<Rect>* rightChild = _bsp->AddRightChild(&parent, rectRight);

		Split(*leftChild);
		Split(*rightChild);
	}
}

void GameLevel::SetNeighbor()
{
	std::vector<BSPNode<Rect>*>& leaves = _bsp->GetLeaves();

	for (int i = 0; i < leaves.size(); i++)
	{
		for (int j = i + 1; j < leaves.size(); j++)
		{
			if (IsNeighbor(leaves[i]->GetData(), leaves[j]->GetData()))
			{
				RoomInfo* iRoom = leaves[i]->GetRoom();
				RoomInfo* jRoom = leaves[j]->GetRoom();

				if (iRoom && jRoom)
				{
					iRoom->_neighbors.emplace_back(jRoom);
					jRoom->_neighbors.emplace_back(iRoom);
				}
			}
		}
	}
}

bool GameLevel::IsNeighbor(const Rect& a, const Rect& b)
{
	bool horizontal = (a._right == b._left || a._left == b._right) &&
		(a._top < b._bottom && a._bottom > b._top) &&
		(min(a._bottom, b._bottom) - max(a._top, b._top)) >= 3;

	bool vertical = (a._bottom == b._top || a._top == b._bottom) &&
		(a._left < b._right && a._right > b._left) &&
		(min(a._right, b._right) - max(a._left, b._left)) >= 3;

	return horizontal || vertical;
}

void GameLevel::ConnectRooms(RoomInfo*& outEntrance, RoomInfo*& outExit)
{
	std::vector<BSPNode<Rect>*> leaves = _bsp->GetLeaves();

	Dijkstra& d = Dijkstra::Get();
	std::vector<RoomInfo*> route;
	int randEntrance = 0;
	int randExit = 0;
	RoomInfo* entrance = nullptr;
	RoomInfo* exit = nullptr;
	do
	{
		route.clear();
		randEntrance = Util::RandomRange(0, static_cast<int>(leaves.size()) - 1);
		randExit = Util::RandomRange(0, static_cast<int>(leaves.size()) - 1);
		entrance = leaves[randEntrance]->GetRoom();
		exit = leaves[randExit]->GetRoom();
		d.FindRoute(entrance, exit, route, false);
	} while (route.size() < MIN_PATH_SIZE);

	ConnectPath(route, 100);

	route.clear();

	entrance = leaves[randEntrance]->GetRoom();
	exit = leaves[randExit]->GetRoom();
	d.FindRoute(entrance, exit, route, false);

	ConnectPath(route);

	for (size_t i = 0; i < leaves.size(); i++)
	{
		if (leaves[i]->GetRoom()->_connected.size() > 0)
			_connectedRooms.emplace_back(leaves[i]->GetRoom());
	}

	int size = static_cast<int>(_connectedRooms.size());	// 기존 연결된 방에서만 추가 방 붙이는용

	if (size < MIN_ROOM_COUNT)
	{
		while (_connectedRooms.size() != MIN_ROOM_COUNT)
		{
			int randIndex = -1, randIndex2 = -1;
			RoomInfo* current = nullptr;
			RoomInfo* candidate = nullptr;
			
			do
			{
				randIndex = Util::RandomRange(0, size - 1);
				current = _connectedRooms[randIndex];
				randIndex2 = Util::RandomRange(0, static_cast<int>(current->_neighbors.size()) - 1);
				candidate = current->_neighbors[randIndex2];

				for (RoomInfo* room : _connectedRooms)
				{
					if (room == candidate)
					{
						randIndex = -1;
						randIndex2 = -1;
					}
				}

			} while ( !(randIndex >= 0 && randIndex2 >= 0) );

			current->_connected.emplace_back(candidate);
			candidate->_connected.emplace_back(current);
			_connectedRooms.emplace_back(candidate);
		}
	}

	outEntrance = entrance;
	outExit = exit;
}

void GameLevel::ConnectPath(std::vector<RoomInfo*>& path, int cost)
{
	std::vector<BSPNode<Rect>*> leaves = _bsp->GetLeaves();

	for (int i = 0; i < path.size() - 1; i++)
	{
		for (int j = 0; j < leaves.size(); j++)
		{
			if (path[i] == leaves[j]->GetRoom())
			{
				std::vector<RoomInfo*>& connected = leaves[j]->GetRoom()->_connected;

				auto iter = std::find(connected.begin(), connected.end(), path[i + 1]);

				if (iter != connected.end())
					continue;

				connected.emplace_back(path[i + 1]);
				path[i + 1]->_connected.emplace_back(leaves[j]->GetRoom());
				leaves[j]->GetRoom()->_cost = cost;
				break;
			}
		}
	}
}

void GameLevel::CreateDoors()
{
	for (size_t i = 0; i < _connectedRooms.size(); i++)
	{
		// 최종 방들에서 연결된 방들 순회하며 문 만들기
		for (size_t j = 0; j < _connectedRooms[i]->_connected.size(); j++)
		{
			RoomInfo* current = _connectedRooms[i]->_connected[j];

			if (HasDoor(_connectedRooms[i], current))
				continue;

			bool isHorizonal = false;
			int pos = 0;
			Vector2 doorPos = Vector2::Zero;

			if (_connectedRooms[i]->_rect._left == current->_rect._right ||
				_connectedRooms[i]->_rect._right == current->_rect._left)
			{
				if (_connectedRooms[i]->_rect._left == current->_rect._right)
					pos = _connectedRooms[i]->_rect._left;
				else
					pos = _connectedRooms[i]->_rect._right;

				isHorizonal = true;
			}
			else
			{
				if (_connectedRooms[i]->_rect._top == current->_rect._bottom)
					pos = _connectedRooms[i]->_rect._top;
				else
					pos = _connectedRooms[i]->_rect._bottom;
			}
				
			int a = 0, b = 0;

			if (isHorizonal)
			{
				a = max(_connectedRooms[i]->_rect._top, current->_rect._top);
				b = min(_connectedRooms[i]->_rect._bottom, current->_rect._bottom);
				doorPos = Vector2(pos, a + ((b - a) / 2));
			}
			else
			{
				a = max(_connectedRooms[i]->_rect._left, current->_rect._left);
				b = min(_connectedRooms[i]->_rect._right, current->_rect._right);
				doorPos = Vector2(a + ((b - a) / 2), pos);
			}
			auto parents = std::make_pair(_connectedRooms[i], current);
			_doors.emplace_back(Door{ parents, doorPos });
		}
	}
}

bool GameLevel::HasDoor(const RoomInfo* const a, const RoomInfo* const b) const
{
	for (size_t i = 0; i < _doors.size(); i++)
	{
		const auto& parents = _doors[i]._parents;
		if ((parents.first == a && parents.second == b) || (parents.first == b && parents.second == a))
			return true;
	}
	return false;
}

