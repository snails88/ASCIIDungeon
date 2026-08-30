#pragma once
#include <Level/Level.h>

template<typename T>
class BSPTree;

template<typename T>
class BSPNode;

using namespace Craft;
class GameLevel : public Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	virtual ~GameLevel();

	void ResetActors();
private:
	virtual void OnInitialized() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

	void Split(BSPNode<RECT>& parent);

private:
	BSPTree<RECT>* _bsp;
};

