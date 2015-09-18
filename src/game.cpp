#include "game.h"

#include "base/nel_state.h"

#include "states/states.h"
#include "states/menustate.h"
#include "states/sessionstate.h"
#include "views/overlayview.h"



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

void TGame::AfterInitialization()
{
	nel::TTGUIApplication::AfterInitialization();

	#ifdef USE_OVERLAY
	// add the overlay scene
	Overlay = std::make_shared<TOverlayView>(GUI, StateMachine);
	AttachScene(Overlay);
	#endif

	// register our game states
	Factory.RegisterObjectType(SID_Splash, std::bind(&TGame::CreateState_Splash, this));
	Factory.RegisterObjectType(SID_Menu, std::bind(&TGame::CreateState_Menu, this));
	Factory.RegisterObjectType(SID_Options, std::bind(&TGame::CreateState_Options, this));
	Factory.RegisterObjectType(SID_Session, std::bind(&TGame::CreateState_Session, this));
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

void* TGame::CreateState_Splash()
{
	// TODO
	return nullptr;
}

void* TGame::CreateState_Menu()
{
	return new TMenuState(GUI);
}

void* TGame::CreateState_Options()
{
	// TODO
	return nullptr;
}

void* TGame::CreateState_Session()
{
	return new TSessionState(GUI);
}