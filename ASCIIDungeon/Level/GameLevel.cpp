#include <string>				// 방 분할 테스트
#include "GameLevel.h"
#include <Game/Game.h>
#include <BSP.h>
#include <Define.h>
#include <Util/Util.h>
#include <Render/Renderer.h>	// 방 분할 테스트

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

	Game& engine = static_cast<Game&>(Engine::Get());

	RECT rect;
	rect.left = 0;
	rect.right = engine.GetWidth() - 1;
	rect.top = 0;
	rect.bottom = engine.GetHeight() - 1;
	
	_bsp = new BSPTree<RECT>(rect);

	Split(*(_bsp->GetRoot()));
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

	std::vector<BSPNode<RECT>*>& leaves = _bsp->GetLeaves();

	if (!hasInitialized)
		return;

	for (int i = 0; i < leaves.size(); i++)
	{
		RECT rect = leaves[i]->GetData();

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

void GameLevel::Split(BSPNode<RECT>& parent)
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
		return;
	}
	else if (width > height || (width == height && equalWidthSplit))
	{
		float widthf = static_cast<float>(width);

		int randSplit = static_cast<int>(Util::RandomRange(widthf * 0.35f, widthf * 0.65f));

		RECT rectLeft = parent.GetData();
		rectLeft.right = rectLeft.left + randSplit;
		BSPNode<RECT>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		RECT rectRight = parent.GetData();
		rectRight.left = rectLeft.right + 1;
		BSPNode<RECT>* rightChild = _bsp->AddRightChild(&parent, rectRight);

		Split(*leftChild);
		Split(*rightChild);
	}
	else if (height > width || (width == height && !equalWidthSplit))
	{
		float heightf = static_cast<float>(height);

		int randSplit = static_cast<int>(Util::RandomRange(heightf * 0.35f, heightf * 0.65f));

		RECT rectLeft = parent.GetData();
		rectLeft.bottom = rectLeft.top + randSplit;
		BSPNode<RECT>* leftChild = _bsp->AddLeftChild(&parent, rectLeft);

		RECT rectRight = parent.GetData();
		rectRight.top = rectLeft.bottom + 1;
		BSPNode<RECT>* rightChild = _bsp->AddRightChild(&parent, rectRight);

		Split(*leftChild);
		Split(*rightChild);
	}
}
