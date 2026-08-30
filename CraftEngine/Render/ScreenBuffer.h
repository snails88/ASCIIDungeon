#pragma once
#include <Math/Vector2.h>
#include <Windows.h>

namespace Craft
{
	// 더블 버퍼링 구현을 위한 버퍼 클래스
	class ScreenBuffer
	{
	public:
		ScreenBuffer(const Vector2& screenSize);
		~ScreenBuffer();

		void Clear() const;									// 콘솔 초기화(화면지우기)
		void Draw(const CHAR_INFO* const charInfo) const;	// 전달된 글자 값 그리는 함수
		inline HANDLE GetBuffer() const { return buffer; }
	private:
		HANDLE buffer = nullptr;	// 화면 버퍼 핸들
		Vector2 size;				// 화면 크기
	};
}