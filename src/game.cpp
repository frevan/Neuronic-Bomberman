#include "game.h"

#include "base/nel_state.h"
#include "states/states.h"
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

nel::TGameID TGame::GetInitialGameStateID()
{
	return SID_Menu;
}

nel::IGameState* TGame::CreateGameState(nel::TGameID id)
{
	nel::IGameState* state = nullptr;

	if (id == SID_Menu)	state = new TMenuState(GUI);

	return state;
}

void TGame::AfterInitialization()
{
	nel::TTGUIApplication::AfterInitialization();
}

std::string TGame::GetDefaultFontName()
{
	return "OpenSans-Regular.ttf";
}