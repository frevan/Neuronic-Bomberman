#include "game.h"

#include "base/nel_state.h"

#include "states/states.h"
#include "states/menustate.h"
#include "states/lobbystate.h"
#include "scenes/overlayscene.h"



//#define FULLSCREEN
#define FRAMERATE_60FPS
#define USE_OVERLAY



// --- TGame ---

TGame::TGame()
:	nel::TTGUIApplication(),
	Overlay()
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

	if (id == SID_Menu)			state = new TMenuState(GUI);
	else if (id == SID_Lobby)	state = new TLobbyState(GUI);

	return state;
}

void TGame::AfterInitialization()
{
	nel::TTGUIApplication::AfterInitialization();

	#ifdef USE_OVERLAY
	// add the overlay scene
	Overlay = std::make_shared<TOverlayScene>(GUI);
	AttachScene(Overlay);
	#endif
}

void TGame::BeforeFinalization()
{
	#ifdef USE_OVERLAY
	DetachScene(Overlay);
	Overlay.reset();
	#endif
}

std::string TGame::GetDefaultFontName()
{
	return "OpenSans-Regular.ttf";
}