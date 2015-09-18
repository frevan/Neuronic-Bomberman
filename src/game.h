#pragma once

#include "base/tgui/nel_game_tgui.h"

#include "views/overlayview.h"



class TGame :	public nel::TTGUIApplication
{
private:
	std::shared_ptr<TOverlayView> Overlay;

	// factory functions
	void* CreateState_Splash();
	void* CreateState_Menu();
	void* CreateState_Options();
	void* CreateState_Session();

protected:
	sf::RenderWindow* CreateWindow() override;
	nel::TGameID GetInitialGameStateID() override;
	void AfterInitialization() override;
	void BeforeFinalization() override;

	std::string GetDefaultFontName() override;

public:
	TGame();
};