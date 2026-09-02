#pragma once
#include <Level/Level.h>
#include <ETC/Rect.h>

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

	void Split(BSPNode<Rect>& parent);
	void SetNeighbor();
	bool IsNeighbor(const Rect& a, const Rect& b);
private:
	BSPTree<Rect>* _bsp;
};

