#include "game.h"

#include "base/nel_state.h"
#include "states/menustate.h"



//#define FULLSCREEN



// --- TGame ---

TGame::TGame()
:	nel::TTGUIApplication()
{
}

sf::RenderWindow* TGame::CreateWindow()
{
	#ifdef FULLSCREEN
	sf::Uint32 windowstyle = sf::Style::Fullscreen;
	#else
	sf::Uint32 windowstyle = sf::Style::Titlebar | sf::Style::Close;
	#endif

	return new sf::RenderWindow{sf::VideoMode{800, 600, 32}, "Bomberman", windowstyle};
}

nel::IGameState* TGame::CreateInitialGameState()
{
	return new TMenuState(GUI);
}

void TGame::AfterInitialization()
{
	nel::TTGUIApplication::AfterInitialization();
}

std::string TGame::GetDefaultFontName()
{
	return "OpenSans-Regular.ttf";
}

