#pragma once

#include "gamedata.h"
#include "gamelogic.h"
#include "resourcemgr.h"

class TServer: public TLogicListener
{
private:
	enum TState
	{
		INACTIVE = 0,
		RUNNING,
		INLOBBY,
		PLAYING
	};
private:
	TGameData Data;
	TGameLogic* Logic;
	TState State;
	TMapList Maps;

	void BroadCastChange(int Command, intptr_t Index, intptr_t Value, std::string StrParam);

public:
	TServer();
	~TServer();

	void LoadMaps(const std::string& Path);

	void Start(); // activate the server
	void Stop(); // deactivate the server

	bool OpenLobby(); // create a lobby for a new game
	void CloseLobby(); // close the lobby

	bool StartMatch(); // start the first round of the match	
	bool StartNextRound(); // start a new round when the previous one has ended
	bool EndRound(); // end the current round

	void SetGameName(const std::string& SetName); // change the game's name
	uint8_t AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	bool RemovePlayer(uint8_t Slot); // remove a player from the current game
	bool SetPlayerName(int Slot, const std::string& Name); // change a player's name
	bool SelectArena(const std::string& ArenaName); // set the arena
	int SetNumRounds(int Value); // set the number of rounds to be played

	void Process(TGameTime Delta);

	// inherited from TLogicListener
	void LogicRoundEnded() override; 
};