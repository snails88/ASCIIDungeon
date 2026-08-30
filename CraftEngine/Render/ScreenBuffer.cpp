#include "ScreenBuffer.h"
#include <cassert>
#include <iostream>

namespace Craft
{
	ScreenBuffer::ScreenBuffer(const Vector2& screenSize)
		: size(screenSize)
	{
		// 콘솔 버퍼 생성
		buffer = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr,
			CONSOLE_TEXTMODE_BUFFER, nullptr);

		assert(buffer != INVALID_HANDLE_VALUE);

		// 화면 창 크기 설정
		SMALL_RECT rect = {};
		rect.Top = 0;
		rect.Left = 0;
		rect.Right = static_cast<short>(size.x - 1);
		rect.Bottom = static_cast<short>(size.y - 1);
		BOOL result = SetConsoleWindowInfo(buffer, TRUE, &rect);
		assert(result == TRUE);

		// 화면 버퍼 크기 설정
		result = SetConsoleScreenBufferSize(buffer, size);
		assert(result == TRUE);

		// 직접 만든 콘솔의 커서 Off
		CONSOLE_CURSOR_INFO info;
		result = GetConsoleCursorInfo(buffer, &info);
		assert(result == TRUE);
		info.bVisible = FALSE;
		result = SetConsoleCursorInfo(buffer, &info);
		assert(result == TRUE);
	}
	
	ScreenBuffer::~ScreenBuffer()
	{
		// 콘솔 버퍼 닫기
		if (buffer)
		{
			CloseHandle(buffer);
		}
	}

	void ScreenBuffer::Clear() const
	{
		DWORD writtenCount = 0;	//화면에 설정된 글자 수

		BOOL result = FillConsoleOutputCharacterA(buffer, ' ', size.x * size.y, Vector2::Zero, &writtenCount);
		assert(result == TRUE);
	}

	void ScreenBuffer::Draw(const CHAR_INFO* const charInfo) const
	{
		// charInfo는 2차원 배열(1차원 배열에 2차원 배열 정보를 기록)
		SMALL_RECT rect = {
			0,
			0,
			static_cast<short>(size.x - 1),
			static_cast<short>(size.y - 1)
		};

		// 콘솔에 CHAR_INFO 타입으로 글자 쓰는 함수
		BOOL result = WriteConsoleOutputA(buffer, charInfo, size, Vector2::Zero, &rect);

		assert(result == TRUE);
	}
}