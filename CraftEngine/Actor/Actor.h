#pragma once
#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/Color.h>
#include <Core/CraftObject.h>
#include <memory>	// weak_ptr 사용
#include <string>

namespace Craft
{
	class Level;

	class CRAFT_API Actor : public CraftObject	// 가상 공간에 배치될 모든 액터의 기본 클래스
	{
		TYPE_DECLARATIONS(Actor, CraftObject)

	public:
		Actor(const std::string& image = "", const Vector2& position = Vector2::Zero, Color color = Color::White);
		virtual ~Actor();

		// 게임 플레이 이벤트 함수
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		// 충돌 이벤트 함수
		virtual void OnCollision(const std::shared_ptr<Actor>& other);

		void Destroy();		// 액터 제거 함수
		void QuitGame();	// 게임(엔진) 종료 함수
		
		inline void SetActive(bool _active) { isActive = _active; }
		inline bool HasBeganPlay() const { return hasBeganPlay; }
		inline bool IsActive() const { return isActive && !hasExpired; }
		inline bool HasExpired() const { return hasExpired; }

		inline std::shared_ptr<Level> GetOwner() const { return owner.lock(); }
		inline void SetOwner(std::weak_ptr<Level> newOwner) { owner = newOwner; }

		inline Vector2 GetPosition() const { return position; }
		void SetPosition(const Vector2& newPosition);

		inline Vector2 GetPreviousPosition() const { return previousPosition; }
		inline void SavePreviousState() { previousPosition = position; }

		inline int GetWidth() const { return width; }

		inline void ChangeImage(const std::string& newImage)
		{
			width = static_cast<int>(newImage.length());
			image = newImage;
		}

	protected:
		bool hasBeganPlay = false;	// BeginPlay 이벤트 처리 여부 플래그
		bool isActive = true;		// 액터 활성화 여부 플래그
		bool hasExpired = false;	// 삭제 요청 여부 플래그
		std::weak_ptr<Level> owner;	// 오너십 - 이 액터를 소유하는 레벨 객체
		std::string image;			// 화면에 그릴 글자
		Color color = Color::White;	// 색상
		int width = 0;				// 글자길이
		int sortingOrder = 0;		// 렌더링 순서
		Vector2 position;			// 위치
		Vector2 previousPosition;	// 이전 프레임 위치
	};
}