#include <string>				// 방 분할 테스트
#include "GameLevel.h"
#include <Game/Game.h>
#include <ETC/BSP.h>
#include <Define.h>
#include <Util/Util.h>
#include <Render/Renderer.h>	// 방 분할 테스트
#include <Pathfind/RoomConnect.h>
#include <algorithm>

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

void GameLevel::OnInitialized()
{
	super::OnInitialized();

	Util::SetRandomSeed();

	Game& engine = static_cast<Game&>(Engine::Get());

	Rect rect;
	rect.left = 0;
	rect.right = engine.GetWidth() - 1;
	rect.top = 0;
	rect.bottom = engine.GetHeight() - 1;
	
	_bsp = new BSPTree<Rect>(rect);
	_connectedRooms.reserve(MIN_ROOM_COUNT);

	Split(*(_bsp->GetRoot()));
 	SetNeighbor();
	ConnectRooms();
	CreateDoors();
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

	if (!hasInitialized)
		return;

	for (size_t i = 0; i < _connectedRooms.size(); i++)
	{
		Rect rect = _connectedRooms[i]->_rect;

		std::string str = "";
		for (int j = rect.left; j <= rect.right; j++)
		{
			str += " ";
		}
		Renderer::Get().Submit(str, Vector2(rect.left, rect.top), Color::B_White);
		Renderer::Get().Submit(str, Vector2(rect.left, rect.bottom), Color::B_White);
		
		std::string dotStr = "";
		for (int j = rect.left + 1; j <= rect.right - 1; j++)
		{
			dotStr += " ";
		}

		for (int j = rect.top + 1; j <= rect.bottom - 1; j++)
		{
			Renderer::Get().Submit(" ", Vector2(rect.left, j), Color::B_White);
			Renderer::Get().Submit(" ", Vector2(rect.right, j), Color::B_White);
			Renderer::Get().Submit(dotStr, Vector2(rect.left + 1, j), Color::B_GRAY);
		}
	}

	for (size_t i = 0; i < _doors.size(); i++)
		Renderer::Get().Submit(" " , _doors[i]._position, Color::B_Yellow, Sort::SortingOrder::Door);
}

void GameLevel::Split(BSPNode<Rect>& parent)
{
	int width = parent.GetData().right - parent.GetData().left;
	int height = parent.GetData().bottom - parent.GetData().top;

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
		rectLeft.right = rectLeft.left + randSplit;
		BSPNode<Rect>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		Rect rectRight = parent.GetData();
		rectRight.left = rectLeft.right;
		BSPNode<Rect>* rightChild = _bsp->AddRightChild(&parent, rectRight);

		Split(*leftChild);
		Split(*rightChild);
	}
	else if (height * 2 > width || (width == height * 2 && !equalWidthSplit))
	{
		float heightf = static_cast<float>(height);

		int randSplit = static_cast<int>(Util::RandomRange(heightf * 0.3f, heightf * 0.7f));

		Rect rectLeft = parent.GetData();
		rectLeft.bottom = rectLeft.top + randSplit;
		BSPNode<Rect>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		Rect rectRight = parent.GetData();
		rectRight.top = rectLeft.bottom;
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
				Room* iRoom = leaves[i]->GetRoom();
				Room* jRoom = leaves[j]->GetRoom();

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
	bool horizontal = (a.right == b.left || a.left == b.right) &&
		(a.top < b.bottom && a.bottom > b.top) &&
		(min(a.bottom, b.bottom) - max(a.top, b.top)) >= 3;

	bool vertical = (a.bottom == b.top || a.top == b.bottom) &&
		(a.left < b.right && a.right > b.left) &&
		(min(a.right, b.right) - max(a.left, b.left)) >= 3;

	return horizontal || vertical;
}

void GameLevel::ConnectRooms()
{
	std::vector<BSPNode<Rect>*> leaves = _bsp->GetLeaves();

	RoomConnect rc;
	std::vector<Room*> path;
	int randEntrance = 0;
	int randExit = 0;
	Room* entrance = nullptr;
	Room* exit = nullptr;
	do
	{
		path.clear();
		randEntrance = Util::RandomRange(0, leaves.size() - 1);
		randExit = Util::RandomRange(0, leaves.size() - 1);
		entrance = leaves[randEntrance]->GetRoom();
		exit = leaves[randExit]->GetRoom();
		rc.ConnectRooms(entrance, exit, path);
	} while (path.size() < MIN_PATH_SIZE);

	for (size_t i = 0; i < leaves.size(); i++)
		leaves[i]->GetRoom()->_cost = 1;

	ConnectPath(path, 100);

	path.clear();

	entrance = leaves[randEntrance]->GetRoom();
	exit = leaves[randExit]->GetRoom();
	rc.ConnectRooms(entrance, exit, path);

	ConnectPath(path);

	for (size_t i = 0; i < leaves.size(); i++)
	{
		if (leaves[i]->GetRoom()->_connected.size() > 0)
			_connectedRooms.emplace_back(leaves[i]->GetRoom());
	}

	int size = _connectedRooms.size();	// 기존 연결된 방에서만 추가 방 붙이는용

	if (size < MIN_ROOM_COUNT)
	{
		while (_connectedRooms.size() != MIN_ROOM_COUNT)
		{
			int randIndex = -1, randIndex2 = -1;
			Room* current = nullptr;
			Room* candidate = nullptr;
			
			do
			{
				randIndex = Util::RandomRange(0, size - 1);
				current = _connectedRooms[randIndex];
				randIndex2 = Util::RandomRange(0, current->_neighbors.size() - 1);
				candidate = current->_neighbors[randIndex2];

				for (Room* room : _connectedRooms)
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
}

void GameLevel::ConnectPath(std::vector<Room*>& path, int cost)
{
	std::vector<BSPNode<Rect>*> leaves = _bsp->GetLeaves();

	for (int i = 0; i < path.size() - 1; i++)
	{
		for (int j = 0; j < leaves.size(); j++)
		{
			if (path[i] == leaves[j]->GetRoom())
			{
				std::vector<Room*>& connected = leaves[j]->GetRoom()->_connected;

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
			Room* current = _connectedRooms[i]->_connected[j];

			if (HasDoor(_connectedRooms[i], current))
				continue;

			bool isHorizonal = false;
			int pos = 0;
			Vector2 doorPos = Vector2::Zero;

			if (_connectedRooms[i]->_rect.left == current->_rect.right ||
				_connectedRooms[i]->_rect.right == current->_rect.left)
			{
				if (_connectedRooms[i]->_rect.left == current->_rect.right)
					pos = _connectedRooms[i]->_rect.left;
				else
					pos = _connectedRooms[i]->_rect.right;

				isHorizonal = true;
			}
			else
			{
				if (_connectedRooms[i]->_rect.top == current->_rect.bottom)
					pos = _connectedRooms[i]->_rect.top;
				else
					pos = _connectedRooms[i]->_rect.bottom;
			}
				
			int a = 0, b = 0;

			if (isHorizonal)
			{
				a = max(_connectedRooms[i]->_rect.top, current->_rect.top);
				b = min(_connectedRooms[i]->_rect.bottom, current->_rect.bottom);
				doorPos = Vector2(pos, a + ((b - a) / 2));
			}
			else
			{
				a = max(_connectedRooms[i]->_rect.left, current->_rect.left);
				b = min(_connectedRooms[i]->_rect.right, current->_rect.right);
				doorPos = Vector2(a + ((b - a) / 2), pos);
			}
			auto parents = std::make_pair(_connectedRooms[i], current);
			_doors.emplace_back(Door{ parents, doorPos });
		}
	}
}

bool GameLevel::HasDoor(const Room* const a, const Room* const b) const
{
	for (size_t i = 0; i < _doors.size(); i++)
	{
		const auto& parents = _doors[i]._parents;
		if ((parents.first == a && parents.second == b) || (parents.first == b && parents.second == a))
			return true;
	}
	return false;
}
