#include "Renderer.h"
#include "ScreenBuffer.h"
#include <cassert>
#include <iostream>
#include <Windows.h>

namespace Craft
{
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = std::make_unique<CHAR_INFO[]>(bufferCount);
		sortingOrderArray = std::make_unique<int[]>(bufferCount);
	}

	Renderer::Frame::~Frame()
	{

	}

	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 이중 루프 순회하면서 값 초기화
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				const int index = (y * width) + x;	// 1차원배열을 2차원배열처럼 사용할때 인덱스 변환
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';	// 빈 문자 설정
				info.Attributes = 0;	// 색상 표기 x
				sortingOrderArray[index] = -1;
			}
		}
	}

	/// 
	/// 
	/// 

	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		assert(!instance && "instance should be null.");

		instance = this;

		// 프레임 객체 생성
		const int bufferCount = screenSize.x * screenSize.y;
		frame = std::make_unique<Frame>(bufferCount);
		frame->Clear(screenSize);

		// 이중 버퍼 구현을 위한 콘솔 버퍼 생성 및 초기화
		screenBufferArray[0] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[0]->Clear();
		screenBufferArray[1] = std::make_unique<ScreenBuffer>(screenSize);
		screenBufferArray[1]->Clear();

		SetConsoleActiveScreenBuffer(screenBufferArray[0]->GetBuffer());	// 화면에 0번 콘솔 버퍼 활성화

		// 콘솔 커서 안보이기 (표준 콘솔)
		//CONSOLE_CURSOR_INFO info;
		//GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		//info.bVisible = FALSE;

		//SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}

	Renderer::~Renderer()
	{
		instance = nullptr;

		SetConsoleActiveScreenBuffer(GetStdHandle(STD_OUTPUT_HANDLE));
		// 콘솔 커서 다시 보이게
		//CONSOLE_CURSOR_INFO info;
		//GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
		//info.bVisible = TRUE;

		//SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
	}

	void Renderer::Submit(const std::string& image, const Vector2& position, Color color, int sortingOrder)
	{
		// 렌더 명령 생성 및 값 설정
		RenderCommand command;
		command.image = image;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	void Renderer::Draw()
	{
		Clear(); //화면 지우기

		DrawRenderQueue(); // 그리기

		Present(); // 출력
	}

	Renderer& Renderer::Get()
	{
		assert(instance && "instance should not be null.");

		return *instance;
	}

	void Renderer::Clear()
	{
		frame->Clear(screenSize);
		GetCurrentBuffer()->Clear();
	}

	void Renderer::DrawRenderQueue()
	{
		for (const RenderCommand& command : renderQueue)
		{
			// 그릴 문자값이 없으면 건너뛰기
			if (command.image.empty())
				continue;

			if (command.position.y < 0 || command.position.y >= screenSize.y)
				continue;

			const int length = static_cast<int>(command.image.length());	// 그리려는 문자열 길이 값

			const int startX = command.position.x;	// 글자의 시작 위치
			const int endX = startX + length - 1;	// 글자의 끝 위치

			if (endX < 0 || startX >= screenSize.x)	// x가 완전히 화면에서 벗어났을때
				continue;

			const int visibleStart = startX < 0 ? 0 : startX;
			const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

			for (int x = visibleStart; x <= visibleEnd; x++)
			{
				const int sourceIndex = x - startX;	// 문자열에서 글자값을 가져올 때 사용할 인덱스

				const int index = (command.position.y * screenSize.x) + x;	// 2차원 배열의 인덱스. (y * width) + x

				// 정렬 순서를 비교해서 그릴지 말지 판정
				if (frame->sortingOrderArray[index] > command.sortingOrder)
					continue;

				// 2차원 배열에 글자, 속성 설정
				frame->charInfoArray[index].Char.AsciiChar = command.image[sourceIndex];
				frame->charInfoArray[index].Attributes = static_cast<DWORD>(command.color);
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		GetCurrentBuffer()->Draw(frame->charInfoArray.get());

		renderQueue.clear();

		SetConsoleTextAttribute(GetCurrentBuffer()->GetBuffer(), static_cast<DWORD>(Color::White));
	}

	void Renderer::Present()
	{
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());	// 현재 순번의 콘솔 버퍼를 활성화
		currentBufferIndex = 1 - currentBufferIndex;
	}
	const ScreenBuffer* Renderer::GetCurrentBuffer() const
	{
		return screenBufferArray[currentBufferIndex].get();
	}
}