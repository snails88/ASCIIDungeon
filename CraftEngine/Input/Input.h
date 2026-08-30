#pragma once
#include <Core/Core.h>

namespace Craft
{
	class CRAFT_API Input
	{
		friend class Engine;

		struct KeyState			// 키 입력 상태 저장을 위한 구조체
		{
			bool isKeyDown;		// 현재 프레임에 키가 눌렸는지 여부
			bool wasKeyDown;	// 이전 프레임에 키가 눌렸는지 여부
		};

	public:
		Input();
		~Input() = default;

		// 키 눌림/해제 여부.
		bool GetKeyDown(int keyCode) const;	// 이전x 이번프레임o면 true
		bool GetKeyUp(int keyCode) const;	// 이전 프레임o 이번프레임x면 true
		bool GetKey(int keyCode) const;		// 현재프레임에 입력이 눌리면 반복해서 true반환 

		static Input& Get();	// 싱글톤
	private:
		void ProcessInput();		// 현재 프레임에 특정 키 입력이 발생했는지
		void SavePreviousStates();	// 이전 프레임의 키 눌림상태 저장

	private:
		const int keyCount = 256;	// 가상 키의 수
		KeyState keyStates[256] = {};

		inline static Input* instance = nullptr;
	};
}
