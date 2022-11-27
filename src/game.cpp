#include "game.h"

#include <algorithm>
#include <cstdint>
#include <cassert>
#include <fstream>
#include <sstream>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "actions.h"
#include "states/menustate.h"
#include "views/menuview.h"
#include "views/lobbyview.h"
#include "views/matchview.h"
#include "views/endofroundview.h"
#include "views/connectview.h"
#include "views/optionsview.h"
#include "comms.h"
#include "utility/stringutil.h"

#include "utility/simpleini/SimpleIni.h"

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
	CurrentState(GAMESTATE_NONE),
	NextState(GAMESTATE_NONE),
	GUI(nullptr),
	SystemGUIView(nullptr),
	AppPath(),
	Views(),
	ViewsMutex(),
	Client(nullptr),
	Server(nullptr),
	GameData(),
	Fonts(),
	Logic(nullptr),
	IsServer(false),
	ArenaNames(),
	CurrentArenaIndex(-1),
	CurrentArenaName(),
	ChosenPlayerName(),
	ServerAddresses()
{
	InitializeInputDefinitions();
	for (int i = 0; i < NUM_INPUTS; i++)
		InputSlots[i] = -1;
}

TGame::~TGame()
{
	assert(Views.size() == 0);
}

bool TGame::Initialize(const std::string& filename)
{
	// initialize some data
	DetermineAppPath(filename);
	Fonts.SetFontPath(AppPath + GetResourceSubPath() + GetFontSubPath());
	Fonts.LoadFonts();

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

	// define inputs
	InitializeInputDefinitions();
	// map some inputs
	InputMap.DefineInput(actionToPreviousScreen, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Escape, 0));
	InputMap.DefineInput(actionDoDefaultAction, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, sf::Keyboard::Key::Return, 0));
	//DefineDefaultPlayerInputs();

	// set next state
	NextState = GAMESTATE_MENU;

	// create client object
	Client = new TClient(&GameData);
	Client->Listener = this;

	// create server object
	Server = new TServer();
	Server->LoadMaps(AppPath + GetMapSubPath());

	// create the logic object
	Logic = new TClientLogic(&GameData);

	ReadSettings();
		
	return true;
}

void TGame::Finalize()
{
	StoreSettings();

	// finalize state
	NextState = GAMESTATE_QUIT;
	ActivateNextState();

	// remove the player input bindings that are always created
	RemovePlayerInputs();

	// delete the logic object
	delete Logic;
	Logic = nullptr;

	// delete the client object
	Client->Listener = nullptr;
	delete Client;
	Client = nullptr;

	// delete the server object
	delete Server;
	Server = nullptr;

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

void TGame::ReadSettings()
{
	std::ifstream ifs;

	// USERNAME
	std::string username;
	std::string fname = AppPath + "username.txt";
	ifs.open(fname, std::fstream::in);
	if (ifs.is_open())
	{
		ifs >> username;
		ifs.close();
	}
	trim(username);
	if (username.size() > 20)
		username.resize(20);
	ChosenPlayerName = username;

	// SERVER ADDRESS
	ServerAddresses.clear();
	fname = AppPath + "server.txt";
	ifs.open(fname, std::fstream::in);
	if (ifs.is_open())
	{
		std::string address;
		do
		{	
			address = "";
			ifs >> address;
			trim(address);
			if (!address.empty())
			{
				if (address.size() > 15)
					address.resize(15);
				ServerAddresses.push_back(address);
			}
		} while (!address.empty());
		ifs.close();
	}

	// INPUT BINDINGS
	CSimpleIniA ini;
	fname = AppPath + "inputs.txt";
	ini.LoadFile(fname.c_str());
	std::string value, section;
	TInputBinding left, right, up, down, dropbomb;
	for (int i = 0; i < NUM_INPUTS; i++)
	{
		section = std::to_string(i);

		left = Inputs[i].Left;
		right = Inputs[i].Right;
		up = Inputs[i].Up;
		down = Inputs[i].Down;
		dropbomb = Inputs[i].DropBomb;

		value = ini.GetValue(section.c_str(), "left", "");
		if (!value.empty())
			StringToInputBinding(value, left);
		value = ini.GetValue(section.c_str(), "right", "");
		if (!value.empty())
			StringToInputBinding(value, right);
		value = ini.GetValue(section.c_str(), "up", "");
		if (!value.empty())
			StringToInputBinding(value, up);
		value = ini.GetValue(section.c_str(), "down", "");
		if (!value.empty())
			StringToInputBinding(value, down);
		value = ini.GetValue(section.c_str(), "dropbomb", "");
		if (!value.empty())
			StringToInputBinding(value, dropbomb);

		SetInputFromValues(i, left, right, up, down, dropbomb);
	}
}

void TGame::StoreSettings()
{
	std::ofstream ofs;

	// USERNAME
	std::string fname = AppPath + "username.txt";
	ofs.open(fname, std::fstream::out);
	if (ofs.is_open())
	{
		ofs << ChosenPlayerName;
		ofs.close();
	}

	// SERVER ADDRESS
	fname = AppPath + "server.txt";
	ofs.open(fname, std::fstream::out);
	if (ofs.is_open())
	{
		int idx = 0;
		for (auto it = ServerAddresses.begin(); it != ServerAddresses.end(); it++)
		{
			ofs << *it << std::endl;
			idx++;
			if (idx > 5)
				break;
		}
		ofs.close();
	}

	// INPUT BINDINGS
	CSimpleIniA ini;
	std::string value, section;
	for (int i = 0; i < NUM_INPUTS; i++)
	{
		section = std::to_string(i);

		value = InputBindingToString(Inputs[i].Left);
		ini.SetValue(section.c_str(), "left", value.c_str());
		value = InputBindingToString(Inputs[i].Right);
		ini.SetValue(section.c_str(), "right", value.c_str());
		value = InputBindingToString(Inputs[i].Up);
		ini.SetValue(section.c_str(), "up", value.c_str());
		value = InputBindingToString(Inputs[i].Down);
		ini.SetValue(section.c_str(), "down", value.c_str());
		value = InputBindingToString(Inputs[i].DropBomb);
		ini.SetValue(section.c_str(), "dropbomb", value.c_str());
	}
	fname = AppPath + "inputs.txt";
	ini.SaveFile(fname.c_str());
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
			if (CurrentState)
				if (CurrentState->View)
					CurrentState->View->Process(delta);

			// process game logic (before the server is processed)
			Logic->Process(delta);

			// process the server object
			Server->Process(delta);

			// process client object
			Client->Process(delta);

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
	InputMap.CheckJoystickState();

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
	if (NextState == GAMESTATE_NONE)
		return;

	if (CurrentState)
	{
		if (NextState == CurrentState->ID)
			return;

		CurrentState->Finish();
		delete CurrentState;
		CurrentState = nullptr;
	}

	switch (NextState)
	{
		case GAMESTATE_NONE: break;
		case GAMESTATE_MENU: CurrentState = new TMenuState(this); break;
		case GAMESTATE_CONNECTING: CurrentState = new TConnectingState(this); break;
		case GAMESTATE_LOBBY: CurrentState = new TLobbyState(this); break;
		case GAMESTATE_MATCH: CurrentState = new TMatchState(this); break;
		case GAMESTATE_ENDOFROUND: CurrentState = new TEndOfRoundState(this); break;
		case GAMESTATE_ENDOFMATCH: CurrentState = new TEndOfMatchState(this); break;
		case GAMESTATE_CONNECTTOSERVER: CurrentState = new TConnectToServerState(this); break;
		case GAMESTATE_OPTIONS: CurrentState = new TOptionsState(this); break;
		case GAMESTATE_QUIT: break;
	};

	NextState = GAMESTATE_NONE;

	if (CurrentState)
		CurrentState->Init();
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
		case VIEW_CONNECTTOSERVER: view = new TConnectToServerView(this, GUI); break;
		case VIEW_OPTIONS: view = new TOptionsView(this, GUI); break;
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
	if (!IsServer)
		Client->JoinGame();
}

void TGame::ClientDisconnected()
{
	if (IsServer)
		Server->Stop();

	CurrentArenaIndex = -1;
	CurrentArenaName = "";
	RemovePlayerInputs();

	SwitchToState(GAMESTATE_MENU);
}

void TGame::ClientEnteredLobby()
{
	std::string name = ChosenPlayerName;
	if (ChosenPlayerName.empty())
		name = "Player";
	Client->AddPlayer(name);

	if (!IsServer)
		SwitchToState(GAMESTATE_LOBBY);
}

void TGame::ClientPlayerAdded(int Slot)
{
	TPlayer* player = &GameData.Players[Slot];
	if (player->Owned)
	{
		for (int i = 0; i < NUM_INPUTS; i++)
			if (InputSlots[i] == -1)
			{
				InputSlots[i] = Slot;
				break;
			}
	}

	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientPlayerNotAdded(int Slot)
{
	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientPlayerRemoved(int Slot)
{
	TPlayer* player = &GameData.Players[Slot];
	if (player->Owned)
	{
		for (int i = 0; i < NUM_INPUTS; i++)
			if (InputSlots[i] == Slot)
			{
				InputSlots[i] = -1;
				break;
			}
	}

	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientPlayerInfoChanged(int Slot)
{
	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientMatchStarting()
{
	AddPlayerInputs();

	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientMatchStarted()
{
	SwitchToState(GAMESTATE_MATCH);
	//CurrentStateView->StateChanged();
}

void TGame::ClientArenaName(int Count, int Index, const std::string Name)
{
	if (Index == 0)
		ArenaNames.clear();

	ArenaNames.push_back(Name);

	if (Index == Count - 1)
		if (CurrentState)
			CurrentState->GameStateChanged();
}

/*
void TGame::DefineDefaultPlayerInputs()
{
	DefineKeyboardForPlayer(0, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::RControl);
	//DefineJoystickForPlayer(1, 0, sf::Joystick::PovX, sf::Joystick::PovY, 1);
	DefineKeyboardForPlayer(1, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::Space);
	DefineKeyboardForPlayer(2, sf::Keyboard::Key::G, sf::Keyboard::Key::J, sf::Keyboard::Key::Y, sf::Keyboard::Key::H, sf::Keyboard::Key::LShift);
	DefineKeyboardForPlayer(3, sf::Keyboard::Key::L, sf::Keyboard::Key::Quote, sf::Keyboard::Key::P, sf::Keyboard::Key::SemiColon, sf::Keyboard::Key::RShift);
	DefineKeyboardForPlayer(4, sf::Keyboard::Key::E, sf::Keyboard::Key::T, sf::Keyboard::Key::Num4, sf::Keyboard::Key::R, sf::Keyboard::Key::Num3);
	DefineKeyboardForPlayer(5, sf::Keyboard::Key::U, sf::Keyboard::Key::O, sf::Keyboard::Key::Num8, sf::Keyboard::Key::I, sf::Keyboard::Key::Num7);
	DefineKeyboardForPlayer(6, sf::Keyboard::Key::C, sf::Keyboard::Key::B, sf::Keyboard::Key::F, sf::Keyboard::Key::V, sf::Keyboard::Key::X);
	DefineKeyboardForPlayer(7, sf::Keyboard::Key::M, sf::Keyboard::Key::Period, sf::Keyboard::Key::K, sf::Keyboard::Key::Comma, sf::Keyboard::Key::N);
	DefineJoystickForPlayer(8, 1, sf::Joystick::PovX, sf::Joystick::PovY, 1);

	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		int offset = i * PlayerActionCount;
		InputMap.SetInputActive(actionMatchPlayer1Left + offset, false);
		InputMap.SetInputActive(actionMatchPlayer1Right + offset, false);
		InputMap.SetInputActive(actionMatchPlayer1Up + offset, false);
		InputMap.SetInputActive(actionMatchPlayer1Down + offset, false);
		InputMap.SetInputActive(actionMatchPlayer1DropBomb + offset, false);
	}
}
*/

void TGame::AddPlayerInputs()
{
	// TODO
	for (int i = 0; i < NUM_INPUTS; i++)
	{
		if (InputSlots[i] >= 0 && InputSlots[i] < MAX_NUM_SLOTS)
		{
			int offset = InputSlots[i] * PlayerActionCount;

			InputMap.DefineInput(offset + actionMatchPlayer1Left, Inputs[i].Left);
			InputMap.DefineInput(offset + actionMatchPlayer1Right, Inputs[i].Right);
			InputMap.DefineInput(offset + actionMatchPlayer1Up, Inputs[i].Up);
			InputMap.DefineInput(offset + actionMatchPlayer1Down, Inputs[i].Down);
			InputMap.DefineInput(offset + actionMatchPlayer1DropBomb, Inputs[i].DropBomb);
		}
	}
}

void TGame::RemovePlayerInputs()
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

	// also: remove the input <> slot assignments
	for (int i = 0; i < NUM_INPUTS; i++)
		InputSlots[i] = -1;
}

/*
void TGame::DefineKeyboardForPlayer(int Slot, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb)
{
	int offset = Slot * PlayerActionCount;

	InputMap.DefineInput(actionMatchPlayer1Left + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Left, 0));
	InputMap.DefineInput(actionMatchPlayer1Right + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Right, 0));
	InputMap.DefineInput(actionMatchPlayer1Up + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Up, 0));
	InputMap.DefineInput(actionMatchPlayer1Down + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Down, 0));
	InputMap.DefineInput(actionMatchPlayer1DropBomb + offset, TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, DropBomb, 0));
}

void TGame::DefineJoystickForPlayer(int Slot, int JoystickIndex, sf::Joystick::Axis LeftRight, sf::Joystick::Axis UpDown, int DropBombBtn)
{
	int offset = Slot * PlayerActionCount;

	TInputID id = actionMatchPlayer1Left + offset;
	InputMap.DefineInput(id, TInputControl::Pack(TInputControl::TType::JOYSTICKAXIS, JoystickIndex, TInputControl::AxisToButtonIndex(LeftRight), 0), 0.f);
	InputMap.SetInputRange(id, 0.f, 0.5f);
	InputMap.SetInputInverted(id, true);
	InputMap.SetInputThreshold(id, 0.1f);
	id = actionMatchPlayer1Right + offset;
	InputMap.DefineInput(id, TInputControl::Pack(TInputControl::TType::JOYSTICKAXIS, JoystickIndex, TInputControl::AxisToButtonIndex(LeftRight), 0), 0.f);
	InputMap.SetInputRange(id, 0.5f, 1.f);
	InputMap.SetInputInverted(id, false);
	InputMap.SetInputThreshold(id, 0.1f);
	id = actionMatchPlayer1Up + offset;
	InputMap.DefineInput(id, TInputControl::Pack(TInputControl::TType::JOYSTICKAXIS, JoystickIndex, TInputControl::AxisToButtonIndex(UpDown), 0), 0.f);
	InputMap.SetInputRange(id, 0.5f, 1.f);
	InputMap.SetInputInverted(id, false);
	InputMap.SetInputThreshold(id, 0.1f);
	id = actionMatchPlayer1Down + offset;
	InputMap.DefineInput(id, TInputControl::Pack(TInputControl::TType::JOYSTICKAXIS, JoystickIndex, TInputControl::AxisToButtonIndex(UpDown), 0), 0.f);
	InputMap.SetInputRange(id, 0.f, 0.5f);
	InputMap.SetInputInverted(id, true);
	InputMap.SetInputThreshold(id, 0.1f);
	id = actionMatchPlayer1DropBomb + offset;
	InputMap.DefineInput(id, TInputControl::Pack(TInputControl::TType::JOYSTICKBUTTON, JoystickIndex, DropBombBtn, 0));
}
*/

void TGame::ClientRoundStarted()
{
	SwitchToState(GAMESTATE_MATCH);
}

void TGame::ClientRoundEnded()
{
	SwitchToState(GAMESTATE_ENDOFROUND);
}

void TGame::ClientGameOptionsChanged()
{
	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::ClientArenaSelected(int Index, const std::string Name)
{
	CurrentArenaIndex = Index;
	CurrentArenaName = Name;

	if (CurrentState)
		CurrentState->GameStateChanged();
}

void TGame::InitializeInputDefinitions()
{
	DefineKeyboardForInputNum(0, sf::Keyboard::Key::Left, sf::Keyboard::Key::Right, sf::Keyboard::Key::Up, sf::Keyboard::Key::Down, sf::Keyboard::Key::RControl);
	DefineKeyboardForInputNum(1, sf::Keyboard::Key::A, sf::Keyboard::Key::D, sf::Keyboard::Key::W, sf::Keyboard::Key::S, sf::Keyboard::Key::LControl);
	DefineKeyboardForInputNum(2, sf::Keyboard::Key::G, sf::Keyboard::Key::J, sf::Keyboard::Key::Y, sf::Keyboard::Key::H, sf::Keyboard::Key::B);
	DefineKeyboardForInputNum(3, sf::Keyboard::Key::L, sf::Keyboard::Key::Quote, sf::Keyboard::Key::P, sf::Keyboard::Key::SemiColon, sf::Keyboard::Key::RShift);
}

void TGame::DefineKeyboardForInputNum(int InputNum, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb)
{
	// left
	Inputs[InputNum].Left.Active = false;
	Inputs[InputNum].Left.InputID = 0;
	Inputs[InputNum].Left.DefaultControl = TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Left, 0);
	Inputs[InputNum].Left.Control = Inputs[InputNum].Left.DefaultControl;
	Inputs[InputNum].Left.Scale = 1.f;
	Inputs[InputNum].Left.RangeStart = 0.f;
	Inputs[InputNum].Left.RangeEnd = 1.f;
	Inputs[InputNum].Left.DefaultValue = 0.f;
	Inputs[InputNum].Left.Threshold = 0.f;
	Inputs[InputNum].Left.Inverted = false;

	// right
	Inputs[InputNum].Right.Active = false;
	Inputs[InputNum].Right.InputID = 0;
	Inputs[InputNum].Right.DefaultControl = TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Right, 0);
	Inputs[InputNum].Right.Control = Inputs[InputNum].Right.DefaultControl;
	Inputs[InputNum].Right.Scale = 1.f;
	Inputs[InputNum].Right.RangeStart = 0.f;
	Inputs[InputNum].Right.RangeEnd = 1.f;
	Inputs[InputNum].Right.DefaultValue = 0.f;
	Inputs[InputNum].Right.Threshold = 0.f;
	Inputs[InputNum].Right.Inverted = false;

	// up
	Inputs[InputNum].Up.Active = false;
	Inputs[InputNum].Up.InputID = 0;
	Inputs[InputNum].Up.DefaultControl = TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Up, 0);
	Inputs[InputNum].Up.Control = Inputs[InputNum].Up.DefaultControl;
	Inputs[InputNum].Up.Scale = 1.f;
	Inputs[InputNum].Up.RangeStart = 0.f;
	Inputs[InputNum].Up.RangeEnd = 1.f;
	Inputs[InputNum].Up.DefaultValue = 0.f;
	Inputs[InputNum].Up.Threshold = 0.f;
	Inputs[InputNum].Up.Inverted = false;

	// down
	Inputs[InputNum].Down.Active = false;
	Inputs[InputNum].Down.InputID = 0;
	Inputs[InputNum].Down.DefaultControl = TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, Down, 0);
	Inputs[InputNum].Down.Control = Inputs[InputNum].Down.DefaultControl;
	Inputs[InputNum].Down.Scale = 1.f;
	Inputs[InputNum].Down.RangeStart = 0.f;
	Inputs[InputNum].Down.RangeEnd = 1.f;
	Inputs[InputNum].Down.DefaultValue = 0.f;
	Inputs[InputNum].Down.Threshold = 0.f;
	Inputs[InputNum].Down.Inverted = false;

	// drop bomb
	Inputs[InputNum].DropBomb.Active = false;
	Inputs[InputNum].DropBomb.InputID = 0;
	Inputs[InputNum].DropBomb.DefaultControl = TInputControl::Pack(TInputControl::TType::KEYBOARD, 0, DropBomb, 0);
	Inputs[InputNum].DropBomb.Control = Inputs[InputNum].DropBomb.DefaultControl;
	Inputs[InputNum].DropBomb.Scale = 1.f;
	Inputs[InputNum].DropBomb.RangeStart = 0.f;
	Inputs[InputNum].DropBomb.RangeEnd = 1.f;
	Inputs[InputNum].DropBomb.DefaultValue = 0.f;
	Inputs[InputNum].DropBomb.Threshold = 0.f;
	Inputs[InputNum].DropBomb.Inverted = false;
}

void TGame::DefineJoystickForInputNum(int Slot, int JoystickIndex, sf::Joystick::Axis LeftRight, sf::Joystick::Axis UpDown, int DropBombBtn)
{
}

bool TGame::SetInputForSlot(int InputIndex, uint8_t Slot)
{
	bool result = false;

	if (InputSlots[InputIndex] == -1 || InputSlots[InputIndex] == Slot)
	{
		InputSlots[InputIndex] = Slot;
		result = true;
	}

	// remove slot from other index
	for (int i = 0; i < NUM_INPUTS; i++)
	{
		if (InputSlots[i] == Slot && i != InputIndex)
			InputSlots[i] = -1;
	}

	return result;
}

std::string TGame::InputBindingToString(const TInputBinding& Binding)
{
	std::string result = "";

	result += std::to_string(Binding.Control.Type) + "|" + std::to_string(Binding.Control.ControllerIndex) + "|" + std::to_string(Binding.Control.Button) + "|" + std::to_string(Binding.Control.Flags);
	result += "|" + std::to_string(Binding.Scale) + "|" + std::to_string(Binding.RangeStart) + "|" + std::to_string(Binding.RangeEnd) + "|" + std::to_string(Binding.Threshold) + "|" + std::to_string(Binding.Inverted);

	return result;
}

void TGame::StringToInputBinding(const std::string& String, TInputBinding& Binding)
{
	std::stringstream ss(String);
	std::string s;

	int intvalue;

	int idx = 0;
	while (std::getline(ss, s, '|'))
	{
		switch (idx)
		{
			case 0: 
				intvalue = string_to_int(s, Binding.Control.Type); 
				if (intvalue == 0)
					Binding.Control.Type = TInputControl::TType::INVALID;
				else if (intvalue == 1)
					Binding.Control.Type = TInputControl::TType::KEYBOARD;
				else if (intvalue == 2)
					Binding.Control.Type = TInputControl::TType::MOUSE;
				else if (intvalue == 0)
					Binding.Control.Type = TInputControl::TType::MOUSEWHEEL;
				else if (intvalue == 0)
					Binding.Control.Type = TInputControl::TType::JOYSTICKAXIS;
				else if (intvalue == 0)
					Binding.Control.Type = TInputControl::TType::JOYSTICKBUTTON;
				break;
			case 1: 
				Binding.Control.ControllerIndex = string_to_int(s, Binding.Control.ControllerIndex); 
				break;
			case 2: 
				Binding.Control.Button = string_to_int(s, Binding.Control.Button);
				break;
			case 3: 
				Binding.Control.Flags = string_to_int(s, Binding.Control.Flags);
				break;
			case 4: 
				Binding.Scale = string_to_float(s, Binding.Scale);
				break;
			case 5: 
				Binding.RangeStart = string_to_float(s, Binding.RangeStart);
				break;
			case 6: 
				Binding.RangeEnd = string_to_float(s, Binding.RangeEnd);
				break;
			case 7: 
				Binding.Threshold = string_to_float(s, Binding.Threshold);
				break;
			case 8: 
				Binding.Inverted = string_to_int(s, Binding.Inverted) != 0;
				break;
		};
		idx++;
	}
}

void TGame::SetInputFromValues(int Index, const TInputBinding& Left, const TInputBinding& Right, const TInputBinding& Up, const TInputBinding& Down, const TInputBinding& DropBomb)
{
	if (Index < 0 || Index >= NUM_INPUTS)
		return;

	if (Left.Control.Type != TInputControl::TType::INVALID)
		Inputs[Index].Left = Left;
	if (Right.Control.Type != TInputControl::TType::INVALID)
		Inputs[Index].Right = Right;
	if (Up.Control.Type != TInputControl::TType::INVALID)
		Inputs[Index].Up = Up;
	if (Down.Control.Type != TInputControl::TType::INVALID)
		Inputs[Index].Down = Down;
	if (DropBomb.Control.Type != TInputControl::TType::INVALID)
		Inputs[Index].DropBomb = DropBomb;
}

void TGame::SetChosenServerAddress(const std::string& Address)
{
	// remove address from the list
	for (auto it = ServerAddresses.begin(); it != ServerAddresses.end(); )
	{
		if (*it == Address)
			it = ServerAddresses.erase(it);
		else
			++it;
	}

	ServerAddresses.insert(ServerAddresses.begin(), Address);
}

std::string TGame::GetChosenServerAddress()
{
	std::string result = "127.0.0.1";
	if (ServerAddresses.size() >= 1)
		result = ServerAddresses[0];

	return result;
}

std::string TGame::GetPreviousServerAddress(int Index)
{
	std::string result = "";

	int cur = 0;
	for (auto it = ServerAddresses.begin(); it != ServerAddresses.end(); it++)
	{
		if (cur == Index)
		{
			result = *it;
			break;
		}
		cur++;
	}

	return result;
}