#pragma once

#include "base/tgui/nel_game_tgui.h"

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>



class TGame :	public nel::TTGUIApplication
{
protected:
	sf::RenderWindow* CreateWindow() override;
	nel::TGameID GetInitialGameStateID() override;
	nel::IGameState* CreateGameState(nel::TGameID id) override;
	void AfterInitialization() override;

	std::string GetDefaultFontName() override;

public:
	TGame();
};