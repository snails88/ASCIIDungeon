#pragma once
#include <Windows.h>
#include <memory>	// 스마트 포인터 사용
#include <Core/Core.h>

namespace Craft
{
	class Level;
	class Input;
	class Renderer;
	class CollisionSystem;

	class CRAFT_API Engine
	{
	private:
		struct Setting
		{
			float framerate = 0.f;
			int width = 0;
			int height = 0;
		};

	public:
		Engine();
		virtual ~Engine();

		void Run();		// 엔진 실행
		void Quit();	// 엔진 종료

		template<typename T, typename = std::enable_if_t<std::is_base_of<Level, T>::value>>
		void AddNewLevel()
		{
			nextLevel = std::make_shared<T>();
		}

		static Engine& Get();
		inline int GetWidth() const { return setting.width; }
		inline int GetHeight() const { return setting.height; }
		inline int64_t GetTime() const { return currentTime; }
		float GetFrequency() const;

		std::weak_ptr<Level> GetLevel() const { return mainLevel; }

	protected:
		void ProcessInput();			// 입력 처리 함수 (입력 폴링)
		void OnInitilized();			// 초기화 함수
		void BeginPlay();				// 게임 플레이 초기화
		void Tick(float deltaTime);		// 게임 플레이 업데이트
		void Draw();					// 레벨 렌더
		void SavePreviousInputStates();	// 프레임 간 입력 값 저장
		void ShutDown();				// 엔진 종료 시 정리
		void LoadEngineSetting();		// 엔진 설정 로드
		virtual void ProcessCollision();

	protected:
		bool isQuit = false;		// 엔진 종료 요청 여부
		int64_t currentTime;		// 현재 시간
		LARGE_INTEGER frequency;	// 시간 해상도
		Setting setting;			// 엔진 설정
		static Engine* instance;	// 전역 접근 가능하게 변수 선언

		std::shared_ptr<Level> mainLevel;	// 메인 레벨
		std::shared_ptr<Level> nextLevel;	// 추가 요청된 레벨
		std::unique_ptr<Input> input;		// 입력 시스템 변수
		std::unique_ptr<Renderer> renderer;
		std::unique_ptr<CollisionSystem> collisionSystem;
	};
}


