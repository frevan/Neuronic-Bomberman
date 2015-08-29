#pragma once

#include <SFGUI/SFGUI.hpp>

#include "base/tgui/nel_game_tgui.h"

#include "scenes/overlayscene.h"



class TGame :	public nel::TTGUIApplication
{
private:
	std::shared_ptr<TOverlayScene> Overlay;
protected:
	sf::RenderWindow* CreateWindow() override;
	nel::TGameID GetInitialGameStateID() override;
	nel::IGameState* CreateGameState(nel::TGameID id) override;
	void AfterInitialization() override;
	void BeforeFinalization() override;

	std::string GetDefaultFontName() override;

public:
	TGame();
};