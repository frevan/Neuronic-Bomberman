#pragma once

#include "base/basegame.h"

#include <SFML/Graphics.hpp>



class TGame :	public TGameApplication
{
protected:
	sf::RenderWindow* createWindow() override;

public:
	TGame();
};