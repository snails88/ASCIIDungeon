#include "Player.h"
#include <Define.h>
#include <Math/Color.h>

using namespace Craft;

Player::Player(const Vector2& pos)
	: super("P", pos)
{
	color = Color::B_White | Color::Red;
	sortingOrder = Sort::Player;
}

void Player::BeginPlay()
{
	super::BeginPlay();
}

void Player::Tick(float deltaTime)
{
	super::Tick(deltaTime);
}

void Player::Draw()
{
	super::Draw();
}
