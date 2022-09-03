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
	virtual void ClientRoundStarted() = 0;
	virtual void ClientRoundEnded() = 0;
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
const int CMD_StartNextRound = 5;
const int CMD_EndRound = 6;

class TClient
{
private:
	TGame* Game;
	std::queue<int> Commands;
public:
	TClientListener* Listener;

	TClient(TGame* SetGame);

	void Process(TGameTime Delta);

	void CreateGame(const std::string& LobbyName); // create a new game / lobby
	void CloseGame(); // leave the current game (removes all players that were added by this client)
	bool AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	void RemovePlayer(uint8_t Slot); // remove a player from the current game
	void StartMatch(); // start the first round of the match	
	void StartNextRound(); // start a new round when the previous one has ended
	void EndRound(); // end the current round

	// set game properties (when in the lobby)
	void SelectArena(int Index); // set the arena
	void SetNumRounds(int Value); // set the number of rounds to be played

	void UpdatePlayerMovement(int Slot, TPlayerDirection Direction, bool SetActive);
	void DropBomb(int Slot);
};