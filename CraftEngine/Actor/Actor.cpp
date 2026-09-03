#include "Actor.h"
#include <Engine/Engine.h>
#include <Render/Renderer.h>

namespace Craft
{
	Actor::Actor(const std::string& image, const Vector2& position, DWORD color)
		:image(image), position(position), color(color), width(static_cast<int>(image.length()))
	{
	}

	Actor::~Actor()
	{
	}

	void Actor::BeginPlay()
	{
		hasBeganPlay = true;
	}

	void Actor::Tick(float deltaTime)
	{
	}

	void Actor::Draw()
	{
		if (!IsActive())	// 비활성 상태면 종료
		{
			return;
		}

		Renderer::Get().Submit(image, position, color, sortingOrder);
	}

	void Actor::OnCollision(const std::shared_ptr<Actor>& other)
	{
	}

	void Actor::Destroy()
	{
		hasExpired = true;	// 삭제 예약설정
	}

	void Actor::QuitGame()
	{
		Engine::Get().Quit();	// 엔진 종료 요청
	}

	void Actor::SetPosition(const Vector2& newPosition)
	{
		if (position == newPosition)
			return;

		position = newPosition;
	}
}