#pragma once

class TClientListener
{
public:
	virtual void ClientConnected() = 0;
	virtual void ClientDisconnected() = 0;
	virtual void ClientEnteredLobby() = 0;
	virtual void ClientPlayerAdded() = 0;
	virtual void ClientPlayerRemoved() = 0;
	virtual void ClientMatchStarting() = 0;
	virtual void ClientMatchStarted() = 0;
};

class TClient;

#include <string>

#include "gamedata.h"
#include "game.h"

const int CMD_None = 0;
const int CMD_OpenLobby = 1;
//const int CMD_AddPlayer = 2;
//const int CMD_RemovePlayer = 3;
const int CMD_StartMatch = 4;

class TClient
{
private:
	TGame* Game;
	std::queue<int> Commands;
public:
	TClientListener* Listener;

	TClient(TGame* SetGame);

	void Process(TGameTime Delta);

	void CreateGame(const std::string& LobbyName);
	void CloseGame();
	bool AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT);
	void RemovePlayer(uint8_t Slot);
	void StartMatch();
	void SelectArena(int Index);

	void UpdatePlayerMovement(int Slot, TPlayerDirection Direction, bool SetActive);
};