#include "game.h"

#include "base/nel_state.h"

#include "states/states.h"
#include "states/menu.h"
#include "states/lobby.h"
#include "states/serverselect.h"
#include "states/servercreate.h"
#include "states/loadmatch.h"
#include "states/play.h"
#include "states/roundend.h"
#include "states/matchend.h"
#include "views/overlayview.h"

#include "gameinterfaces.h"



//#define FULLSCREEN
#define FRAMERATE_60FPS
#define USE_OVERLAY



// --- TGame ---

TGame::TGame()
:	nel::TTGUIApplication(),
	Overlay(),
	Server(nullptr),
	Client(nullptr)
{
	Server = std::make_shared<TServer>();
	Client = std::make_shared<TClient>();	
}

TGame::~TGame()
{
	Server.reset();
	Client.reset();
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
	Overlay = std::make_shared<TOverlayView>(GameState);
	AttachView(Overlay);
	#endif

	// register our game states
	Factory.RegisterObjectType(SID_Splash, std::bind(&TGame::CreateState_Splash, this));
	Factory.RegisterObjectType(SID_Menu, std::bind(&TGame::CreateState_Menu, this));
	Factory.RegisterObjectType(SID_Options, std::bind(&TGame::CreateState_Options, this));	
	Factory.RegisterObjectType(SID_ServerSelect, std::bind(&TGame::CreateState_ServerSelect, this));
	Factory.RegisterObjectType(SID_ServerCreate, std::bind(&TGame::CreateState_ServerCreate, this));
	Factory.RegisterObjectType(SID_Lobby, std::bind(&TGame::CreateState_Lobby, this));
	Factory.RegisterObjectType(SID_LoadMatch, std::bind(&TGame::CreateState_LoadMatch, this));
	Factory.RegisterObjectType(SID_Play, std::bind(&TGame::CreateState_Play, this));
	Factory.RegisterObjectType(SID_RoundEnd, std::bind(&TGame::CreateState_RoundEnd, this));
	Factory.RegisterObjectType(SID_MatchEnd, std::bind(&TGame::CreateState_MatchEnd, this));

	Server->Application = this;
	Client->Application = this;

	AddLogic(Server);
	AddLogic(Client);
}

void TGame::BeforeFinalization()
{
	#ifdef USE_OVERLAY
	DetachView(Overlay);
	Overlay.reset();
	#endif
	
	Client->Disconnect();
	Server->Stop();

	RemoveLogic(Client);
	RemoveLogic(Server);
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

void* TGame::CreateState_Lobby()
{
	return new TLobbyState(GUI);
}

void* TGame::CreateState_ServerSelect()
{
	return new TServerSelectState(GUI);
}

void* TGame::CreateState_ServerCreate()
{
	return new TServerCreateState(GUI);
}

void* TGame::CreateState_LoadMatch()
{
	return new TLoadMatchState(GUI);
}

void* TGame::CreateState_Play()
{
	return new TPlayState(GUI);
}

void* TGame::CreateState_RoundEnd()
{
	return new TRoundEndState(GUI);
}

void* TGame::CreateState_MatchEnd()
{
	return new TMatchEndState(GUI);
}

nel::Interface* TGame::RetrieveInterface(nel::TGameID id)
{
	nel::Interface* result = nullptr;

	if (id == IID_IServer)
		result = (Interface*)(Server.get());
	else if (id == IID_IClient)
		result = (Interface*)(Client.get());
	else
		result = TTGUIApplication::RetrieveInterface(id);

	return result;
}
