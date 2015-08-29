#include "game.h"

#include "base/nel_state.h"
#include "states/states.h"
#include "states/menustate.h"



//#define FULLSCREEN
#define FRAMERATE_60FPS



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

	auto w = new sf::RenderWindow{sf::VideoMode{800, 600, 32}, "Bomberman", windowstyle};

	#ifdef FRAMERATE_60FPS
	w->setFramerateLimit(60);
	#endif

	return w;
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