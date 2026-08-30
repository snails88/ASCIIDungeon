#include "Input.h"
#include <cassert>
#include <Windows.h>

namespace Craft
{
	Input::Input()
	{
		assert(!instance && "instance should be null here.");
		instance = this;
	}
	bool Input::GetKeyDown(int keyCode) const
	{
		return !keyStates[keyCode].wasKeyDown && keyStates[keyCode].isKeyDown;
	}
	bool Input::GetKeyUp(int keyCode) const
	{
		return keyStates[keyCode].wasKeyDown && !keyStates[keyCode].isKeyDown;
	}
	bool Input::GetKey(int keyCode) const
	{
		return keyStates[keyCode].isKeyDown;
	}

	Input& Input::Get()
	{
		assert(instance && "instance should not be null here.");
		return *instance;
	}

	void Input::ProcessInput()
	{
		// 현재 프레임 키 입력 확인
		for (int i = 0; i < keyCount; i++)
		{
			keyStates[i].isKeyDown = ((GetAsyncKeyState(i) & 0x8000) != 0);	// 0x8000은 최상위 비트 의미
		}
	}

	void Input::SavePreviousStates()
	{
		for (KeyState& state : keyStates)
		{
			state.wasKeyDown = state.isKeyDown;
		}
	}
}