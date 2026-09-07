#pragma once
#include <Actor/Actor.h>
#include <Math/Vector2.h>
#include <ETC/Rect.h>
#include <vector>
#include <list>

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
	void AddActor(std::shared_ptr<Actor> actor);

	void SetVisible(bool visible);
	inline bool IsVisited() const { return _visited; }
	inline const std::vector<Craft::Vector2>& GetWalls() { return _walls; }

private:
	inline void Visit() { _visited = true; }
private:
	bool _isVisible = false;
	bool _visited = false;
	Rect _rect;
	std::vector<Craft::Vector2> _walls;
	std::vector<Craft::Vector2> _doors;
	std::list<std::weak_ptr<Actor>> _actors;
};

