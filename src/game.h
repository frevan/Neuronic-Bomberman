#pragma once

class TGame;

#include <string>
#include <mutex>

#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <TGUI/TGUI.hpp>

#include "inputmap.h"
#include "state.h"
#include "view.h"
#include "client.h"
#include "gamedata.h"
#include "resourcemgr.h"
#include "gamelogic.h"
#include "server.h"

const int STATE_NONE = 0;
const int STATE_MENU = 1;
const int STATE_CONNECTING = 2;
const int STATE_LOBBY = 3;
const int STATE_MATCH = 4;
const int STATE_ENDOFROUND = 5;
const int STATE_ENDOFMATCH = 6;
const int STATE_CONNECTTOSERVER = 7;
const int STATE_QUIT = 1000;

const int VIEW_OVERLAY = 0;
const int VIEW_TGUISYSTEM = 1;
const int VIEW_MENU = 11;
const int VIEW_LOBBY = 12;
const int VIEW_MATCH = 13;
const int VIEW_ENDOFROUND = 14;
const int VIEW_CONNECTTOSERVER = 15;

typedef struct
{
	std::string Name;
	std::string FileName;
} TMapInfo;

class TGame:	public TClientListener
{
private:
	sf::RenderWindow* Window;
	int NextState;
	TView* CurrentStateView;
	TView* SystemGUIView;
	tgui::Gui* GUI;	
	std::vector<TView*> Views;
	std::mutex ViewsMutex;
	TClientLogic* Logic;

	void DetermineAppPath(const std::string& Filename);
	std::string GetResourceSubPath();
	std::string GetFontSubPath();
	std::string GetMapSubPath();
	std::string GetFontFileName(const std::string& FontIdentifier = "");
	std::string GetDefaultFontName();

	void ProcessInputs();
	void ActivateNextState();
	void SetupCurrentState();
	void FinalizeCurrentState();

	//void LoadMapInfos();

	void DefineDefaultPlayerInputs();
	void RemovePlayerInputs();
	void DefineKeyboardForPlayer(int Slot, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb);
	void DefineJoystickForPlayer(int Slot, int JoystickIndex, sf::Joystick::Axis LeftRight, sf::Joystick::Axis UpDown, int DropBombBtn);

public:
	int CurrentState;
	bool ShouldQuit;
	TInputMap InputMap;
	TClient* Client;
	TServer* Server;
	TGameData GameData;
	TFontManager Fonts;
	std::string AppPath, MapPath;
	TMapList Maps;
	std::vector<std::string> ArenaNames;
	bool IsServer;
	int CurrentArenaIndex;
	std::string CurrentArenaName;

	TGame();
	~TGame();

	bool Initialize(const std::string& filename);
	void Finalize();

	void Execute();

	// from TGameInterface
	void SwitchToState(int NewState); // see STATE_ constants
	void RequestQuit();
	TView* AttachView(int NewView); // see VIEW_ constants 
	void DetachView(TViewID ID);

	// from TClientListener
	void ClientConnected() override;
	void ClientDisconnected() override;
	void ClientEnteredLobby() override;
	void ClientGameOptionsChanged() override;
	void ClientPlayerAdded(int Slot) override;
	void ClientPlayerNotAdded(int Slot) override;
	void ClientPlayerRemoved(int Slot) override;
	void ClientPlayerInfoChanged(int Slot) override;
	void ClientMatchStarting() override;
	void ClientMatchStarted() override;
	void ClientRoundStarted() override;
	void ClientRoundEnded() override;
	void ClientArenaSelected(int Index, const std::string Name) override;
	void ClientArenaName(int Count, int Index, const std::string Name) override;
};