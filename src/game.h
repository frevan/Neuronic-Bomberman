#pragma once

#include "base/sfgui/nel_game_sfgui.h"

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>



class TGame :	public nel::TSFGUIApplication
{
protected:
	sf::RenderWindow* createWindow() override;
	nel::IGameState* createInitialGameState() override;
	void afterInitialization() override;

public:
	TGame();
};