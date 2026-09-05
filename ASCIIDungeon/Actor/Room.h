#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>
#include <ETC/Rect.h>
#include <vector>

class Room : public Craft::Actor
{
	TYPE_DECLARATIONS(Room, Actor)

public:
	Room(const Rect& rect);

	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Draw();

public:
	void AddWall(const Craft::Vector2& pos);
	void AddDoor(const Craft::Vector2& pos);

	inline const std::vector<Craft::Vector2>& GetWalls() { return _walls; }

private:
	Rect _rect;
	std::vector<Craft::Vector2> _walls;
	std::vector<Craft::Vector2> _doors;
};

