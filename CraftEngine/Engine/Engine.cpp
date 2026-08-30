#include <iostream>
#include <Windows.h>
#include <cassert>

#include "Engine.h"
#include <Level/Level.h>
#include <Input/Input.h>
#include <Render/Renderer.h>
#include <Physics/CollisionSystem.h>

namespace Craft
{
	Engine* Engine::instance = nullptr;

	Engine::Engine()
	{
		assert(!instance && "instance is !null");
		instance = this;	// 인스턴스 초기화

		LoadEngineSetting();

		input = std::make_unique<Input>();	// 입력 객체 생성

		renderer = std::make_unique<Renderer>(Vector2(setting.width, setting.height));

		collisionSystem = std::make_unique<CollisionSystem>();
	}
	Engine::~Engine()
	{
		instance = nullptr;
	}
	
	// 엔진 루프
	void Engine::Run()
	{
		// 고해상도 타이머
		// ms - 1/1000초 -> 해상도 1000
		QueryPerformanceFrequency(&frequency);	// 타이머 해상도

		// 현재 시간 읽기
		LARGE_INTEGER counter;
		QueryPerformanceCounter(&counter);

		// 프레임 계산 변수
		currentTime = counter.QuadPart;
		int64_t previous = currentTime;

		// 고정 프레임으로 만들기 위한 값
		float oneFrameTime = 1.f / setting.framerate;

		while (true)
		{
			// 종료 조건 처리
			if (isQuit)
			{
				break;
			}

			// 프레임 처리

			// 입력 처리
			ProcessInput();

			/// 프레임 시간 계산
			// 현재 시간 읽기
			QueryPerformanceCounter(&counter);
			currentTime = counter.QuadPart;

			// (현재 시간 - 이전시간) / 시간단위(해상도) -> 초단위로 변환
			float deltaTime = static_cast<float>(currentTime - previous) /
				static_cast<float>(frequency.QuadPart);

			// 고정프레임처리.
			if (deltaTime >= oneFrameTime)
			{
				// 게임 이벤트 함수 호출
				OnInitilized();

				// 게임 이벤트의 초기화
				BeginPlay();

				// 게임 업데이트
				Tick(deltaTime);

				ProcessCollision();

				// 화면 렌더
				Draw();

				if (nextLevel)
				{
					if (mainLevel)
					{
						mainLevel.reset();
					}

					mainLevel = nextLevel;

					nextLevel.reset();	// 포인터 정리
				}

				if (mainLevel)
				{
					mainLevel->ProcessAddandDestroyActors();

					mainLevel->SavePreviousActorStates();
				}

				// 입력 상태 저장
				SavePreviousInputStates();

				// 현재 시간을 이전 시간에 저장
				previous = currentTime;
			}
			///
		}

		// 종료 처리 함수 호출
		ShutDown();
	}

	void Engine::Quit()
	{
		isQuit = true;
	}

	Engine& Engine::Get()
	{
		assert(instance && "instance is null");	// assert는 디버그에서만 동작

		return *instance;
	}

	float Engine::GetFrequency() const
	{
		return static_cast<float>(frequency.QuadPart);
	}

	void Engine::ProcessInput()
	{
		assert(input && "input should not be null here.");

		if (!input)
			return;

		input->ProcessInput();
	}

	void Engine::OnInitilized()
	{
		if (!mainLevel || mainLevel->HasInitilized())
		{
			return;
		}

		mainLevel->OnInitialized();
	}

	void Engine::BeginPlay()
	{
		if (!mainLevel)
			return;

		mainLevel->BeginPlay();
	}
	
	void Engine::Tick(float deltaTime)
	{
		if (!mainLevel)
			return;

		mainLevel->Tick(deltaTime);
	}

	void Engine::Draw()
	{
		if (!mainLevel)
			return;

		mainLevel->Draw();

		if (!renderer)
			return;

		renderer->Draw();
	}

	void Engine::ProcessCollision()
	{
		if (!mainLevel || !collisionSystem)
		{
			return;
		}
		collisionSystem->ProcessCollision(mainLevel->actorList);	// 의존성 주입
	}

	void Engine::SavePreviousInputStates()
	{
		assert(input && "input should not null here.");

		if (!input)
			return;

		input->SavePreviousStates();
	}

	void Engine::ShutDown()
	{
	}

	void Engine::LoadEngineSetting()
	{
		FILE* file = nullptr;
		fopen_s(&file, "../Config/Setting.txt", "rt");

		if (!file)
		{
			std::cout << "Engine::LoadEngineSetting() Failed\n";
			__debugbreak();	// 디버그 모드에서 강제 중단 시키는 기능
			return;
		}

		const int bufferSize = 2048;
		char buffer[bufferSize] = {};

		size_t readSize = fread(buffer, sizeof(char), bufferSize, file);

		// 값 저장을 위한 서식 해석(Parsing)
		// 문자열 자르기(Split)
		char* context = nullptr;
		char* token = nullptr;
		token = strtok_s(buffer, "\n", &context);

		while (token)
		{
			char key[15] = {};	// 공백 전까지 읽은 문자열을 저장할 변수
			sscanf_s(token, "%s", key, 15);	// sscanf_s는 공백 문자 만나면 그 전까지 읽어서 저장해줌
			if (strcmp(key, "framerate") == 0) // 키 값 비교해서 값 설정
			{
				sscanf_s(token, "framerate = %f", &setting.framerate);
			}
			else if (strcmp(key, "width") == 0)
			{
				sscanf_s(token, "width = %d", &setting.width);
			}
			else if (strcmp(key, "height") == 0)
			{
				sscanf_s(token, "height = %d", &setting.height);
			}

			token = strtok_s(nullptr, "\n", &context);
		}

		fclose(file);
		file = nullptr;
	}
}