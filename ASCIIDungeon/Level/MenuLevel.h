#pragma once
#include <Level/Level.h>
#include <string>

struct MenuItem
{
	using OnSelected = void(*)();

	MenuItem(const std::string& text, OnSelected onSelected)
		: _text(text), _onSelected(onSelected)
	{

	}


	std::string _text;

	OnSelected _onSelected = nullptr;
};

class MenuLevel : public Craft::Level
{
public:
	MenuLevel();

private:
	virtual void Tick(float deltaTime) override;
	virtual void Draw() override;

private:
	int _currentIndex = 0;

	Craft::Color _selectedColor = Craft::Color::Green;
	Craft::Color _unselectedColor = Craft::Color::White;

	std::vector<std::unique_ptr<MenuItem>> _itemList;
};

