#pragma once

class TClientListener
{
public:
	virtual void OnConnected() = 0;
	virtual void OnDisconnected() = 0;
	virtual void OnEnteredLobby() = 0;
	virtual void OnPlayerAdded() = 0;
	virtual void OnPlayerRemoved() = 0;
};

class TClient;

#include <string>

#include "gamedata.h"
#include "game.h"

const int CMD_None = 0;
const int CMD_OpenLobby = 1;
const int CMD_AddPlayer = 2;
const int CMD_RemovePlayer = 3;

class TClient
{
private:
	TGame* Game;
	int NextCommand;
public:
	TClientListener* Listener;

	TClient(TGame* SetGame);

	void Process(TGameTime Delta);

	// from TClientInterface
	void CreateGame(const std::string& LobbyName);
	void CloseGame();
	bool AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT);
	void RemovePlayer(uint8_t Slot);
	void StartMatch();
};