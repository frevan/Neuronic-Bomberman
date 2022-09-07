#include "game.h"

#include <algorithm>
#include <cstdint>
#include <cassert>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "actions.h"
#include "states/menustate.h"
#include "views/menuview.h"
#include "views/lobbyview.h"
#include "views/matchview.h"
#include "views/endofroundview.h"

//#define FULLSCREEN
#define FRAMERATE_LIMIT
//#define USE_OVERLAY

#ifdef FRAMERATE_LIMIT
const int MaxFrameRateConst = 60;
#endif

int NextViewID = 1;

// --- TGame ---

TGame::TGame()
	: Window(nullptr),
	ShouldQuit(false),
	InputMap(),
	CurrentState(STATE_NONE),
	NextState(STATE_NONE),
	CurrentStateView(nullptr),
	GUI(nullptr),
	SystemGUIView(nullptr),
	AppPath(),
	MapPath(),
	Views(),
	ViewsMutex(),
	Client(nullptr),
	GameData(),
	Fonts(),
	Logic(nullptr),
	Maps()
{
}

TGame::~TGame()
{
	assert(Views.size() == 0);
}

bool TGame::Initialize(const std::string& filename)
{
	// initialize some data
	DetermineAppPath(filename);
	MapPath = AppPath + GetMapSubPath();
	Fonts.SetFontPath(AppPath + GetResourceSubPath() + GetFontSubPath());
	Fonts.LoadFonts();
	Maps.LoadAllMaps(AppPath + GetMapSubPath());

	// create application window
	#ifdef FULLSCREEN
	sf::Uint32 windowstyle = sf::Style::Fullscreen;
	#else
	sf::Uint32 windowstyle = sf::Style::Titlebar | sf::Style::Close;
	#endif
	Window = new sf::RenderWindow(sf::VideoMode{800, 600, 32}, "Neuronic Bomberman", windowstyle);
	#ifdef FRAMERATE_LIMIT
	Window->setFramerateLimit(MaxFrameRateConst);
	#endif

	// create TGUI object and initialize it
	GUI = new tgui::Gui(*Window);
	tgui::setResourcePath(AppPath + GetResourceSubPath());
	GUI->setFont(GetFontFileName());
	// create and attacht the TGUI system view
	//GUIView = new TTGUISystemView(this, GUI);
	//AttachView(GUIView);
	SystemGUIView = AttachView(VIEW_TGUISYSTEM);

	// map some inputs
	InputMap.DefineInput(actionToPreviousScreen, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Escape, 0));
	InputMap.DefineInput(actionDoDefaultAction, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Return, 0));

	// set next state
	NextState = STATE_MENU;

	// create client object
	Client = new TClient(this);
	Client->Listener = this;

	// create the logic object
	Logic = new TGameLogic(&GameData, this);
		
	return true;
}

void TGame::Finalize()
{
	// finalize state
	NextState = STATE_QUIT;
	ActivateNextState();

	// delete the logic object
	delete Logic;
	Logic = nullptr;

	// delete the client object
	Client->Listener = nullptr;
	delete Client;
	Client = nullptr;

	// detach our TGUI system view
	DetachView(SystemGUIView->ID);
	SystemGUIView = nullptr;
	// delete the GUI object
	delete GUI;
	GUI = nullptr;

	// delete the application window
	delete Window;
	Window = nullptr;
}

void TGame::Execute()
{
	sf::Clock clock;

	const int TICKS_PER_SECOND = 25;
	const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
	const int MAX_FRAMESKIP = 5;

	uint32_t startTickForFPS = clock.getElapsedTime().asMilliseconds();
	uint32_t nextGameTick = startTickForFPS;
	int loops = 0;

	uint32_t prevTickTime = clock.getElapsedTime().asMilliseconds();
	uint32_t nowTime;

	while (Window->isOpen())
	{
		ActivateNextState();
		
		// handle events
		ProcessInputs();

		// let the logic objects process a tick
		loops = 0;
		nowTime = clock.getElapsedTime().asMilliseconds();
		while (nowTime > nextGameTick && loops < MAX_FRAMESKIP)
		{
			TGameTime delta = nowTime - prevTickTime;

			// let the current view process as well
			if (CurrentStateView)
				CurrentStateView->Process(delta);

			// process client object
			Client->Process(delta);

			// process game logic
			Logic->Process(delta);

			// get new time
			nextGameTick += SKIP_TICKS;
			loops++;
			prevTickTime = nowTime;
			nowTime = clock.getElapsedTime().asMilliseconds();
		}

		// draw views
		sf::RenderTarget* target = Window;
		target->clear();	// fixed to black, for now
		{
			std::lock_guard<std::mutex> g(ViewsMutex);
			for (auto it = Views.rbegin(); it != Views.rend(); it++)
			{				
				TView* view = (*it);
				if (!view)
					continue;
				if (view->IsVisible())
					view->Draw(target);
			}
		}

		/*
		if (FpsCalculator)
			FpsCalculator->NewFrame();
		*/

		// display the window on the screen
		Window->display();

		// this will most likely be set in reaction to some event, i.e. by the game state
		if (ShouldQuit)
			Window->close();
	}
}

void TGame::ProcessInputs()
{
	InputMap.CheckKeyboardState();

	sf::Event event;
	while (Window->pollEvent(event))
	{
		bool handled = false;
		TInputID inputID;
		float value;
		if (InputMap.ProcessEvent(event, inputID, value))
		{
			std::lock_guard<std::mutex> g(ViewsMutex);
			for (auto it = Views.rbegin(); it != Views.rend(); it++)
			{
				TView* view = (*it);
				if (!view)
					continue;
				if (view->IsVisible())
					handled = view->ProcessInput(inputID, value);
				if (handled)
					break;
			}
		}
		
		if (!handled)
			GUI->handleEvent(event);
	}
}

void TGame::SwitchToState(int NewState)
{
	NextState = NewState;
}

void TGame::ActivateNextState()
{
	if (NextState == STATE_NONE)
		return;

	FinalizeCurrentState();

	if (CurrentStateView)
	{
		DetachView(CurrentStateView->ID);
		CurrentStateView = nullptr;
	}

	CurrentState = NextState; // make the switch
	NextState = STATE_NONE;

	switch (CurrentState)
	{
		case STATE_MENU: CurrentStateView = AttachView(VIEW_MENU); break;
		case STATE_LOBBY: CurrentStateView = AttachView(VIEW_LOBBY); break;
		case STATE_MATCH: CurrentStateView = AttachView(VIEW_MATCH); break;
		case STATE_ENDOFROUND: CurrentStateView = AttachView(VIEW_ENDOFROUND); break;
	};

	SetupCurrentState();
}

void TGame::RequestQuit()
{
	ShouldQuit = true;
}

void TGame::DetermineAppPath(const std::string& filename)
{
	std::string path = filename;
	#ifdef WIN32
	std::replace(path.begin(), path.end(), '\\', '/');
	#endif
	size_t found = path.find_last_of('/');
	path = path.substr(0, found + 1);

	AppPath = path;
}

std::string TGame::GetResourceSubPath()
{ 
	return "Artwork/"; 
}

std::string TGame::GetFontSubPath() 
{ 
	return "Fonts/"; 
};

std::string TGame::GetMapSubPath()
{
	return "Maps/";
}

std::string TGame::GetFontFileName(const std::string& FontIdentifier)
{
	std::string name = FontIdentifier;
	if (FontIdentifier == "")	
		name = GetDefaultFontName();

	return AppPath + GetResourceSubPath() + GetFontSubPath() + name;
}

std::string TGame::GetDefaultFontName()
{
	return "OpenSans-Regular.ttf";
}

TView* TGame::AttachView(int NewView)
{
	TView* view = nullptr;

	switch (NewView)
	{
		case VIEW_TGUISYSTEM: view = new TTGUISystemView(this, GUI); break;
		case VIEW_MENU: view = new TMenuView(this, GUI); break;
		case VIEW_LOBBY: view = new TLobbyView(this, GUI); break;
		case VIEW_MATCH: view = new TMatchView(this); break;
		case VIEW_ENDOFROUND: view = new TEndOfRoundView(this); break;
	};
	assert(view);

	view->ID = NextViewID++;
	view->OnAttach();

	{
		std::lock_guard<std::mutex> g(ViewsMutex);

		auto insertat = Views.end();
		if (view->Type == TViewType::VT_OVERLAY || view->Type == TViewType::VT_SYSTEMVIEW)
		{
			for (auto it = Views.begin(); it != Views.end(); it++)
			{
				if ((*it)->Type > view->Type)
				{
					insertat = it;
					break;
				}
			}
		}

		Views.insert(insertat, view);
	}

	return view;
}

void TGame::DetachView(TViewID ID)
{
	TView* view = nullptr;

	{
		std::lock_guard<std::mutex> g(ViewsMutex);

		for (auto it = Views.begin(); it != Views.end(); it++)
		{
			if ((*it)->ID == ID)
			{
				view = (*it);
				Views.erase(it);
				break;
			}
		}
	}

	if (view)
	{
		view->OnDetach();
		delete(view);
	}
}

void TGame::ClientConnected()
{
}

void TGame::ClientDisconnected()
{
	SwitchToState(STATE_MENU);
}

void TGame::ClientEnteredLobby()
{
	Client->SelectArena(0); // select the first map initially

	Client->AddPlayer("Steve");
	Client->AddPlayer("Bob");
	// TODO: check result of AddPlayer
}

void TGame::ClientPlayerAdded(int Slot)
{
	CurrentStateView->StateChanged();
}

void TGame::ClientPlayerNotAdded(int Slot)
{
	CurrentStateView->StateChanged();
}

void TGame::ClientPlayerRemoved(int Slot)
{
	CurrentStateView->StateChanged();
}

void TGame::ClientPlayerNameChanged(int Slot)
{
}

void TGame::ClientMatchStarting()
{
	CurrentStateView->StateChanged();
}

void TGame::ClientMatchStarted()
{
	CurrentStateView->StateChanged();
}

void TGame::DefineInputsForPlayer(int Slot, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb)
{
	int offset = Slot * PlayerActionCount;

	InputMap.DefineInput(actionMatchPlayer1Left + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Left, 0));
	InputMap.DefineInput(actionMatchPlayer1Right + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Right, 0));
	InputMap.DefineInput(actionMatchPlayer1Up + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Up, 0));
	InputMap.DefineInput(actionMatchPlayer1Down + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Down, 0));
	InputMap.DefineInput(actionMatchPlayer1DropBomb + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, DropBomb, 0));
}

void TGame::SetupCurrentState()
{
	if (CurrentState == STATE_LOBBY)
	{
		Client->CreateGame("Don't Explode");

		InputMap.DefineInput(actionLobbyPrevMap, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Left, 0));
		InputMap.DefineInput(actionLobbyNextMap, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Right, 0));
	}
	else if (CurrentState == STATE_MATCH)
	{
		DefineInputsForPlayer(0, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::RControl);
		DefineInputsForPlayer(1, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::LControl);
		DefineInputsForPlayer(2, sf::Keyboard::Key::G, sf::Keyboard::Key::J, sf::Keyboard::Key::Y, sf::Keyboard::Key::H, sf::Keyboard::Key::LShift);
		DefineInputsForPlayer(3, sf::Keyboard::Key::L, sf::Keyboard::Key::Quote, sf::Keyboard::Key::P, sf::Keyboard::Key::SemiColon, sf::Keyboard::Key::RShift);
		DefineInputsForPlayer(4, sf::Keyboard::Key::E, sf::Keyboard::Key::T, sf::Keyboard::Key::Num4, sf::Keyboard::Key::R, sf::Keyboard::Key::Num3);
		DefineInputsForPlayer(5, sf::Keyboard::Key::U, sf::Keyboard::Key::O, sf::Keyboard::Key::Num8, sf::Keyboard::Key::I, sf::Keyboard::Key::Num7);
		DefineInputsForPlayer(6, sf::Keyboard::Key::C, sf::Keyboard::Key::B, sf::Keyboard::Key::F, sf::Keyboard::Key::V, sf::Keyboard::Key::X);
		DefineInputsForPlayer(7, sf::Keyboard::Key::M, sf::Keyboard::Key::Period, sf::Keyboard::Key::K, sf::Keyboard::Key::Comma, sf::Keyboard::Key::N);
		//DefineInputsForPlayer(8, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::LControl);
		//DefineInputsForPlayer(9, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::LControl);

		if (GameData.Status == GAME_NONE || GameData.Status == GAME_INLOBBY)
			Client->StartMatch();
		else
			Client->StartNextRound();
	}
	else if (CurrentState == STATE_ENDOFROUND)
	{
		Client->EndRound();
	}
}

void TGame::FinalizeCurrentState()
{
	if (CurrentState == STATE_LOBBY)
	{
		InputMap.RemoveInput(actionLobbyPrevMap);
		InputMap.RemoveInput(actionLobbyNextMap);
	}
	else if (CurrentState == STATE_MATCH)
	{
		// 1
		InputMap.RemoveInput(actionMatchPlayer1Left);
		InputMap.RemoveInput(actionMatchPlayer1Right);
		InputMap.RemoveInput(actionMatchPlayer1Up);
		InputMap.RemoveInput(actionMatchPlayer1Down);
		InputMap.RemoveInput(actionMatchPlayer1DropBomb);
		// 2
		InputMap.RemoveInput(actionMatchPlayer2Left);
		InputMap.RemoveInput(actionMatchPlayer2Right);
		InputMap.RemoveInput(actionMatchPlayer2Up);
		InputMap.RemoveInput(actionMatchPlayer2Down);
		InputMap.RemoveInput(actionMatchPlayer2DropBomb);
		// 3
		InputMap.RemoveInput(actionMatchPlayer3Left);
		InputMap.RemoveInput(actionMatchPlayer3Right);
		InputMap.RemoveInput(actionMatchPlayer3Up);
		InputMap.RemoveInput(actionMatchPlayer3Down);
		InputMap.RemoveInput(actionMatchPlayer3DropBomb);
		// 4
		InputMap.RemoveInput(actionMatchPlayer4Left);
		InputMap.RemoveInput(actionMatchPlayer4Right);
		InputMap.RemoveInput(actionMatchPlayer4Up);
		InputMap.RemoveInput(actionMatchPlayer4Down);
		InputMap.RemoveInput(actionMatchPlayer4DropBomb);
		// 5
		InputMap.RemoveInput(actionMatchPlayer5Left);
		InputMap.RemoveInput(actionMatchPlayer5Right);
		InputMap.RemoveInput(actionMatchPlayer5Up);
		InputMap.RemoveInput(actionMatchPlayer5Down);
		InputMap.RemoveInput(actionMatchPlayer5DropBomb);
		// 6
		InputMap.RemoveInput(actionMatchPlayer6Left);
		InputMap.RemoveInput(actionMatchPlayer6Right);
		InputMap.RemoveInput(actionMatchPlayer6Up);
		InputMap.RemoveInput(actionMatchPlayer6Down);
		InputMap.RemoveInput(actionMatchPlayer6DropBomb);
		// 7
		InputMap.RemoveInput(actionMatchPlayer7Left);
		InputMap.RemoveInput(actionMatchPlayer7Right);
		InputMap.RemoveInput(actionMatchPlayer7Up);
		InputMap.RemoveInput(actionMatchPlayer7Down);
		InputMap.RemoveInput(actionMatchPlayer7DropBomb);
		// 8
		InputMap.RemoveInput(actionMatchPlayer8Left);
		InputMap.RemoveInput(actionMatchPlayer8Right);
		InputMap.RemoveInput(actionMatchPlayer8Up);
		InputMap.RemoveInput(actionMatchPlayer8Down);
		InputMap.RemoveInput(actionMatchPlayer8DropBomb);
		// 9
		InputMap.RemoveInput(actionMatchPlayer9Left);
		InputMap.RemoveInput(actionMatchPlayer9Right);
		InputMap.RemoveInput(actionMatchPlayer9Up);
		InputMap.RemoveInput(actionMatchPlayer9Down);
		InputMap.RemoveInput(actionMatchPlayer9DropBomb);
		// 10
		InputMap.RemoveInput(actionMatchPlayer10Left);
		InputMap.RemoveInput(actionMatchPlayer10Right);
		InputMap.RemoveInput(actionMatchPlayer10Up);
		InputMap.RemoveInput(actionMatchPlayer10Down);
		InputMap.RemoveInput(actionMatchPlayer10DropBomb);
	}
}

void TGame::ClientRoundStarted()
{
	CurrentStateView->StateChanged();
}

void TGame::ClientRoundEnded()
{
	CurrentStateView->StateChanged();
}

void TGame::LogicRoundEnded()
{
	SwitchToState(STATE_ENDOFROUND);
}

void TGame::ClientGameOptionsChanged()
{
	CurrentStateView->StateChanged();
}