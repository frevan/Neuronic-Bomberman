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

const int VIEW_OVERLAY = 0;
const int VIEW_TGUISYSTEM = 1;
const int VIEW_MENU = 11;
const int VIEW_LOBBY = 12;
const int VIEW_MATCH = 13;
const int VIEW_ENDOFROUND = 14;
const int VIEW_CONNECTTOSERVER = 15;
const int VIEW_OPTIONS = 16;

const int NUM_INPUTS = 4;

typedef struct
{
	std::string Name;
	std::string FileName;
} TMapInfo;

typedef struct
{
	TInputBinding Left;
	TInputBinding Right;
	TInputBinding Up;
	TInputBinding Down;
	TInputBinding DropBomb;
} TInputDefinition;

class TGame : public TClientListener
{
private:
	sf::RenderWindow* Window;
	int NextState;
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

	//void LoadMapInfos();

	void InitializeInputDefinitions(); // initialize the Inputs array with some default value
	void DefineKeyboardForInputNum(int InputNum, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb); // helper for InitializeInputDefinitions
	void DefineJoystickForInputNum(int Slot, int JoystickIndex, sf::Joystick::Axis LeftRight, sf::Joystick::Axis UpDown, int DropBombBtn); // helper for InitializeInputDefinitions

	//void DefineDefaultPlayerInputs();
	//void DefineKeyboardForPlayer(int Slot, sf::Keyboard::Key Left, sf::Keyboard::Key Right, sf::Keyboard::Key Up, sf::Keyboard::Key Down, sf::Keyboard::Key DropBomb);
	//void DefineJoystickForPlayer(int Slot, int JoystickIndex, sf::Joystick::Axis LeftRight, sf::Joystick::Axis UpDown, int DropBombBtn);
	void AddPlayerInputs(); // add assigned inputs from the Inputs array to the InputMap
	void RemovePlayerInputs(); // remove all player inputs from the InputMap
	void ReadSettings();
	void StoreSettings();

public:
	TState* CurrentState;
	bool ShouldQuit;
	bool IsServer;
	TInputMap InputMap;
	TClient* Client;
	TServer* Server;
	TGameData GameData;
	TFontManager Fonts;
	std::string AppPath;
	std::vector<std::string> ArenaNames;
	int CurrentArenaIndex;
	std::string CurrentArenaName;
	std::string ChosenPlayerName;
	std::string ChosenServerAddress;
	TInputDefinition Inputs[NUM_INPUTS];
	int InputSlots[NUM_INPUTS];

	TGame();
	~TGame();

	bool Initialize(const std::string& filename);
	void Finalize();

	void Execute();

	void SwitchToState(int NewState); // see STATE_ constants
	void RequestQuit();
	TView* AttachView(int NewView); // see VIEW_ constants 
	void DetachView(TViewID ID);

	bool SetInputForSlot(int InputIndex, uint8_t Slot);

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