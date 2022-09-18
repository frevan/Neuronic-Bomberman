#pragma once

#include "gamedata.h"
#include "gamelogic.h"
#include "resourcemgr.h"

class TServerListener
{
public:
	virtual void ServerLobbyCreated() = 0; // new lobby created
	virtual void ServerLobbyClosed() = 0; // lobby closed
	virtual void ServerEnteredLobby(const std::string& GameName) = 0; // client entered the lobby
	virtual void ServerLeftLobby() = 0; // client left the lobby

	virtual void ServerGameNameChanged(const std::string& GameName) = 0; 
	virtual void ServerArenaChanged(const std::string& ArenaName) = 0;
	virtual void ServerNumRoundsChanged(int NumRounds) = 0;

	virtual void ServerPlayerAdded(int Slot, const std::string& PlayerName) = 0;
	virtual void ServerPlayerRemoved(int Slot) = 0;
	virtual void ServerPlayerNameChanged(int Slot, const std::string& PlayerName) = 0;

	virtual void ServerMatchStarted() = 0;
	virtual void MatchEnded() = 0;
	virtual void ServerRoundStarting() = 0;
	virtual void ServerRoundStarted() = 0;
	virtual void ServerRoundEnded() = 0;
};

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
	TServerListener* Listener;

	void BroadCastChange(int Command, intptr_t Index, intptr_t Value, std::string StrParam);

public:
	TServer(TServerListener* SetListener);
	~TServer();

	void LoadMaps(const std::string& Path);

	void Start(); // activate the server
	void Stop(); // deactivate the server

	bool OpenLobby(); // create a lobby for a new game
	void CloseLobby(); // close the lobby

	bool StartMatch(); // start the first round of the match	
	bool StartRound(); // start a new round when the previous one has ended
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