#include <Game/Game.h>

///
#ifdef _DEBUG
	#define _CRTDBG_MAP_ALLOC
	#include <stdlib.h>
	#include <crtdbg.h>
	#define new new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#endif
///

int main()
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Game game;
	game.Run();
}