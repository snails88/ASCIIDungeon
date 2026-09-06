#include "TurnManager.h"

TurnManager::TurnManager()
{
}

TurnManager::~TurnManager()
{
}

void TurnManager::Tick(float deltaTime)
{
	switch (_turn)
	{
	case TurnManager::Turn::EnemyTurn:
		_turn = Turn::Delay;
		break;

	case TurnManager::Turn::Delay:
		_turnDelay += deltaTime;
		if (_turnDelay >= _maxTurnDelay)
		{
			_turnDelay = 0.f;
			_turn = Turn::PlayerTurn;
		}
		break;
	default:
		break;
	}
}

TurnManager& TurnManager::Get()
{
    if (!_instance)
        _instance = std::make_unique<TurnManager>();
    return *_instance;
}
