#pragma once
#include <Core/Core.h>
#include <Math/Vector2.h>
#include <Math/Color.h>
#include <string>
#include <vector>
#include <memory>

namespace Craft
{
	class ScreenBuffer;

	class CRAFT_API Renderer
	{
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			void Clear(const Vector2& screenSize);

			std::unique_ptr<CHAR_INFO[]> charInfoArray;	// 화면에 그릴 2차원 배열 문자값.
			std::unique_ptr<int[]> sortingOrderArray;	// 그리기 정렬 값 이차원 배열
		};

		struct RenderCommand	// 화면에 그릴 데이터를 명령 단위로 저장하는 구조체
		{
			std::string image;
			Vector2 position = Vector2::Zero;
			DWORD color = Color::White;
			int sortingOrder = -1;	// 클수록 우선순위 높음
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		void Submit(const std::string& image, const Vector2& position, DWORD color = Color::White, int sortingOrder = 0);
		void Draw();

		static Renderer& Get();

		
	private:
		void Clear();				// 그리기 작업을 시작할때 화면 클리어
		void DrawRenderQueue();		// 전달받은 렌더명령을 활용해 화면 그리는함수(출력아님)
		void Present();				// 그린 결과를 화면에 출력
		
		const ScreenBuffer* GetCurrentBuffer() const;
	private:
		static Renderer* instance;

		std::vector<RenderCommand> renderQueue;				// 이번 프레임에 그릴 렌더 명령을 모아두는 배열. Queue처럼 사용
		Vector2 screenSize;
		std::unique_ptr<Frame> frame;						// 글자/그리기 순서 2차원 배열 관리하는 프레임 객체
		std::unique_ptr<ScreenBuffer> screenBufferArray[2];	// 이중 버퍼 
		int currentBufferIndex = 0;
	};
}