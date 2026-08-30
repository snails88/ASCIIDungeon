#pragma once

#include <Core/Core.h>
#include <Windows.h>

namespace Craft
{
	enum class CRAFT_API Color : WORD
	{
		Red = FOREGROUND_RED,
		Green = FOREGROUND_GREEN,
		Blue = FOREGROUND_BLUE,
		Yellow = Red | Green,
		Cyan = Green | Blue,
		Magenta = Red | Blue,
		White = Red | Green | Blue,
		BrightWhite = White | FOREGROUND_INTENSITY,
		B_Red = BACKGROUND_RED,
		B_Green = BACKGROUND_GREEN,
		B_Blue = BACKGROUND_BLUE,
		B_Yellow = B_Red | B_Green,
		B_Cyan = B_Green | B_Blue,
		B_Magenta = B_Red | B_Blue,
		B_White = B_Red | B_Green | B_Blue,
		B_BrightWhite = B_White | BACKGROUND_INTENSITY
	};
}