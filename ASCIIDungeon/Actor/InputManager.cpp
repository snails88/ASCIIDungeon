#include "InputManager.h"
#include <Game/Game.h>
#include <Level/GameLevel.h>
#include <Actor/Player.h>
#include <Actor/Cursor.h>
#include <Actor/Room.h>
#include <Input/Input.h>
#include <PathFind/Dijkstra.h>
#include <Pathfind/AStar.h>

using namespace Craft;

InputManager::InputManager()
	:super("")
{
	_game = static_cast<Game*>(&Engine::Get());

	_player = nullptr;
	_cursor = nullptr;
}

void InputManager::BeginPlay()
{
	super::BeginPlay();
}

void InputManager::Tick(float deltaTime)
{
	super::Tick(deltaTime);

	if(!_player)
		_player = GetOwner()->FindActor<Player>().get();

	if(!_cursor)
		_cursor = GetOwner()->FindActor<Cursor>().get();

	if (Input::Get().GetKeyDown(VK_ESCAPE))
	{
		_game->ToggleMenu();
	}

	Vector2 dir = Vector2::Zero;

	if (Input::Get().GetKey(VK_LEFT))
		dir.x -= 1;
	if (Input::Get().GetKey(VK_RIGHT))
		dir.x += 1;
	if (Input::Get().GetKey(VK_UP))
		dir.y -= 1;
	if (Input::Get().GetKey(VK_DOWN))
		dir.y += 1;

	if (dir == Vector2::Zero)
		_pressedTime = 0.f;
	else
		_pressedTime += deltaTime;

	if (_pressedTime >= 0.05f)
	{
		Vector2 newPos = _cursor->GetPosition() + dir;
		if (newPos.x < 0)
			newPos.x = 0;
		if (newPos.x > _game->GetWidth() - 1)
			newPos.x = _game->GetWidth() - 1;
		if (newPos.y < 0)
			newPos.y = 0;
		if (newPos.y > _game->GetHeight() - 1)
			newPos.y = _game->GetHeight() - 1;
		_cursor->SetPosition(newPos);

		if (_cursor->GetPosition() == _player->GetPosition())
			_cursor->ChangeImage("P", Color::B_Cyan | Color::Magenta);
		else
			_cursor->ChangeImage(" ", Color::B_Cyan);

		_pressedTime = 0.f;
	}

	if (Input::Get().GetKeyDown(VK_RETURN))
	{
		Vector2 playerPos = _player->GetPosition();

		if (playerPos == _cursor->GetPosition())
			return;

		std::weak_ptr<GameLevel> level = Cast<GameLevel>(_cursor->GetOwner());

		RoomInfo* currentRoom = level.lock()->FindRoomInfo(playerPos).first;
		RoomInfo* goalRoom = level.lock()->FindRoomInfo(_cursor->GetPosition()).first;

		if (currentRoom && goalRoom)
		{
			std::vector<RoomInfo*> route;
			std::vector<Vector2> path;

			if (Dijkstra::Get().FindRoute(currentRoom, goalRoom, route))
			{
				if (route.size() == 1)	// 방 내 이동
				{
					RoomInfo* info = level.lock()->FindRoomInfo(playerPos).first;
					Rect rect = info->_rect;
					const std::vector<Vector2>& walls = level.lock()->GetRoom(level.lock()->GetRoomIndex(info)).lock()->GetWalls();
					
					AStar::Get().FindPath(playerPos, _cursor->GetPosition(), rect, walls, path);
				}
				else
				{
					Vector2 prevPos = playerPos;
					for (size_t i = 0; i < route.size() - 1; i++)
					{
						std::vector<Vector2> tmp;
						Vector2 doorPos;
						std::pair<RoomInfo*, RoomInfo*> infos = level.lock()->FindRoomInfo(prevPos);
						RoomInfo* info = infos.first;

						if (!info)
							return;

						// 문이라 방 두개 검출되면 루트상 마지막거
						if (infos.second)
						{
							for (size_t i = 0; i < route.size(); i++)
							{
								if (route[i] == infos.first)
									info = infos.first;
								else if (route[i] == infos.second)
									info = infos.second;
							}
						}

						Rect rect = info->_rect;
						const std::vector<Vector2>& walls = level.lock()->GetRoom(level.lock()->GetRoomIndex(info)).lock()->GetWalls();

						level.lock()->FindDoorPosition(route[i], route[i + 1], doorPos);
						AStar::Get().FindPath(prevPos, doorPos, rect, walls, tmp);
						path.insert(path.end(), tmp.begin(), tmp.end());
						prevPos = doorPos;
					}
					if (path.size() == 0)
						return;

					Vector2 currentPos = path[path.size() - 1];
					std::vector<Vector2> tmp;
					std::pair<RoomInfo*, RoomInfo*> infos = level.lock()->FindRoomInfo(currentPos);
					RoomInfo* info = infos.first;
					if (infos.second)
					{
						for (size_t i = 0; i < route.size(); i++)
						{
							if (route[i] == infos.first)
								info = infos.first;
							else if (route[i] == infos.second)
								info = infos.second;
						}
					}

					Rect rect = info->_rect;
					const std::vector<Vector2>& walls = level.lock()->GetRoom(level.lock()->GetRoomIndex(info)).lock()->GetWalls();
					AStar::Get().FindPath(currentPos, _cursor->GetPosition(), rect, walls, tmp);
					path.insert(path.end(), tmp.begin(), tmp.end());
				}
			}

			_player->SetPosition(path[path.size() - 1]);
		}
	}

}

void InputManager::Draw()
{
}
