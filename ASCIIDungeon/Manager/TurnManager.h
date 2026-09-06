#pragma once
#include <memory>

class TurnManager
{
public:
	enum class Turn
	{
		PlayerTurn,
		EnemyTurn,
		Delay
	};

	TurnManager();
	~TurnManager();

	void Tick(float deltaTime);

	inline void SetTurnType(Turn type) { _turn = type; }
	inline bool IsPlayerTurn() const { return _turn == Turn::PlayerTurn; }

	static TurnManager& Get();

private:
	inline static std::unique_ptr<TurnManager> _instance = nullptr;

	const float _maxTurnDelay = 0.1f;
	float _turnDelay = 0.f;
	Turn _turn = Turn::PlayerTurn;
};

