#pragma once

#include <Actor/Actor.h>
#include <Core/Core.h>
#include <Core/CraftObject.h>
#include <memory>			// 스마트 포인터
#include <vector>

namespace Craft
{
	// 게임에 배치된 모든 액터를 관리하는 클래스
	// this를 shared_ptr나 weak_ptr로 변환할때 쓰는거 상속
	// : shared_from_this() - this 포인터를 shared_ptr로 변환.
	// : weak_from_this() - this 포인터를 weak_ptr로 변환.
	class CRAFT_API Level : public CraftObject, public std::enable_shared_from_this<Level>
	{
		TYPE_DECLARATIONS(Level, CraftObject)
		friend class Engine;

	public:
		Level();
		virtual ~Level();

		virtual void OnInitialized();	// 초기화 함수

		// 게임 플레이 이벤트 함수
		virtual void BeginPlay();
		virtual void Tick(float deltaTime);
		virtual void Draw();

		/// SFINAE
		template<typename T, typename ...Args, 
			typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>	// 액터 추가 함수(템플릿)
		std::shared_ptr<T> SpawnActor(Args&& ...args)
		{
			std::shared_ptr<T> newActor = std::make_shared<T>(std::forward<Args>(args)...);

			addRequestedActorList.emplace_back(newActor);

			// 오너십 설정
			newActor->SetOwner(weak_from_this());

			return newActor;
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of<Actor, T>::value>>	// 액터 검색 함수(템플릿)
		std::shared_ptr<T> FindActor()
		{
			for (const auto& actor : actorList)
			{
				std::shared_ptr<T> targetActor = std::dynamic_pointer_cast<T>(actor);

				if (targetActor)
				{
					return targetActor;
				}
			}

			return nullptr;
		}

		inline bool HasInitilized() const { return hasInitialized; }

	protected:
		void ProcessAddandDestroyActors(); // 이전프레임에 추가/제거 요청 된 액터 처리
		void SavePreviousActorStates();

	protected:
		bool hasInitialized = false;								// 초기화 처리 여부 플래그
		std::vector<std::shared_ptr<Actor>> actorList;				// 레벨에 배치된 모든 액터
		std::vector<std::shared_ptr<Actor>> addRequestedActorList;	// 현재 프레임을 처리하는 과정에서 액터 추가할때씀. 제거는 플래그로 확인
	};
}


