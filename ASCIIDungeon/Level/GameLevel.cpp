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

	Split(*(_bsp->GetRoot()));
 	SetNeighbor();
	
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
				leaves[j]->GetRoom()->_cost = 100;
				break;
			}
		}
	}

	path.clear();
	entrance = leaves[randEntrance]->GetRoom();
	exit = leaves[randExit]->GetRoom();
	rc.ConnectRooms(entrance, exit, path);

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
				break;
			}
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

	std::vector<BSPNode<Rect>*>& leaves = _bsp->GetLeaves();

	if (!hasInitialized)
		return;

	for (int i = 0; i < leaves.size(); i++)
	{
		if (leaves[i]->GetRoom()->_connected.size() == 0)
			continue;

		Rect rect = leaves[i]->GetData();

		std::string str = "";
		for (int j = rect.left; j <= rect.right; j++)
		{
			str += "#";
		}
		Renderer::Get().Submit(str, Vector2(rect.left, rect.top));
		Renderer::Get().Submit(str, Vector2(rect.left, rect.bottom));
		std::string dotStr = "#";
		for (int j = rect.left + 1; j <= rect.right - 1; j++)
		{
			dotStr += ".";
		}
		dotStr += "#";
		for (int j = rect.top + 1; j <= rect.bottom - 1; j++)
		{
			Renderer::Get().Submit(dotStr, Vector2(rect.left, j));
		}
	}
}

void GameLevel::Split(BSPNode<Rect>& parent)
{
	int width = parent.GetData().right - parent.GetData().left;
	int height = parent.GetData().bottom - parent.GetData().top;

	int equalWidthSplit = 0;

	if (width == height)
	{
		equalWidthSplit = Util::RandomRange(0, 1);
	}

	if (width * height < MIN_PARTITION_SIZE)
	{
		_bsp->AddLeaf(parent);
		parent.AddRoom(parent.GetData());
		return;
	}
	else if (width > height || (width == height && equalWidthSplit))
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
	else if (height > width || (width == height && !equalWidthSplit))
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
