#pragma once
#include <Level/Level.h>

using namespace Craft;
class GameLevel : public Level
{
	TYPE_DECLARATIONS(GameLevel, Level)

public:
	void ResetActors();
	
private:
	virtual void OnInitialized() override;
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;
	
private:
	
};

