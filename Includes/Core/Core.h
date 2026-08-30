#pragma once

#pragma warning(disable: 4251) // C4251 Warning Off

// 빌드 모드에 따라서 변경될 값
#define DLLEXPORT __declspec(dllexport)
#define DLLIMPORT __declspec(dllimport)

#if ENGINE_BUILD_DLL
	#define CRAFT_API DLLEXPORT
#else
	#define CRAFT_API DLLIMPORT
#endif
