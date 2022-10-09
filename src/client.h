#pragma once

#include "server.h"

class TClientListener
{
public:
	virtual void ClientConnected() = 0;
	virtual void ClientDisconnected() = 0;

	virtual void ClientEnteredLobby() = 0;
	virtual void ClientGameOptionsChanged() = 0;

	virtual void ClientPlayerAdded(int Slot) = 0;
	virtual void ClientPlayerNotAdded(int Slot) = 0;
	virtual void ClientPlayerRemoved(int Slot) = 0;
	virtual void ClientPlayerInfoChanged(int Slot) = 0;

	virtual void ClientMatchStarting() = 0;
	virtual void ClientMatchStarted() = 0;
	virtual void ClientRoundStarted() = 0;
	virtual void ClientRoundEnded() = 0;

	virtual void ClientArenaSelected(int Index, const std::string Name) = 0;
	virtual void ClientArenaName(int Count, int Index, const std::string Name) = 0;
};

class TClient;

#include <string>

#include "gamedata.h"
#include "game.h"

const int CMD_None = 0;
const int CMD_OpenLobby = 1;
const int CMD_AddPlayer = 2;
const int CMD_RemovePlayer = 3;
const int CMD_StartMatch = 4;
const int CMD_StartNextRound = 5;
const int CMD_SetNumRounds = 6;
const int CMD_SetPlayerName = 7;
const int CMD_SetGameName = 8;
const int CMD_SelectArena = 9;
const int CMD_JoinLobby = 10;

typedef struct
{
	int Command;
	intptr_t Index, Value;
	std::string StrParam;
} TClientCommand;

class TClient
{
private:
	int ConnectionStatus;
	TGameData* Data;
	sf::TcpSocket Socket;
	std::queue<TClientCommand> Commands;

	void ProcessReceivedPacket(sf::Socket* Source, sf::Packet& Packet);
	void ConnectedToServer(); // socket was connected
	void DisconnectedFromServer(); // socket was disconnected
	void DisconnectInternal();

	void ServerConnected(); //  not the same as ServerConnected!
	void ServerLobbyCreated();
	void ServerLobbyClosed();
	void ServerEnteredLobby(const std::string& GameName);
	void ServerLeftLobby();
	void ServerGameNameChanged(const std::string& GameName);
	void ServerArenaChanged(uint16_t ArenaIndex, const std::string& ArenaName);
	void ServerNumRoundsChanged(int NumRounds);
	void ServerPlayerAdded(uint8_t Slot, const std::string& PlayerName);
	void ServerPlayerRemoved(uint8_t Slot);
	void ServerPlayerNameChanged(uint8_t Slot, const std::string& PlayerName);
	void ServerMatchStarted();
	void ServerMatchEnded();
	void ServerRoundStarting();
	void ServerRoundStarted();
	void ServerRoundEnded();
	void ServerPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	void ServerPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion);
	void ServerFullUpdate(TGameData* Data);
	void ServerPlayerInfo(uint8_t Slot, uint8_t state, std::string Name);
	void ServerPlayerPositionChanged(uint8_t Slot, float X, float Y);
	void ServerBombExploding(const TFieldPosition& Position, TGameTime TimeUntilExploded);
	void ServerBombExploded(const TFieldPosition& Position);
	void ServerPlayerDying(uint8_t Slot, uint16_t TimeUntilDeath);
	void ServerPlayerDied(uint8_t Slot);
	void ServerArenaInfo(uint8_t Width, uint8_t Height, sf::Packet& Packet);
	void ServerPlayerScore(uint8_t Slot, uint8_t Score);
	void ServerArenaName(uint16_t Count, uint16_t Index, const std::string Name);

public:
	TClientListener* Listener;

	TClient(TGameData* SetData);
	~TClient();

	void Process(TGameTime Delta);

	// connect to and disconnect from the server
	void Connect(const std::string& ServerAddress, unsigned int ServerPort);
	void Disconnect();

	// start/stop the game, rounds, ...
	void CreateGame(const std::string& SetName); // create a new game / lobby
	void JoinGame(); // join a game / lobby
	void CloseGame(); // leave the current game (removes all players that were added by this client)
	void StartMatch(); // start the first round of the match	
	void StartNextRound(); // start a new round when the previous one has ended

	// set game properties (when in the lobby)
	void SetGameName(const std::string& SetName); // change the game's name
	void AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	void RemovePlayer(uint8_t Slot); // remove a player from the current game
	void SetPlayerName(uint8_t Slot, const std::string& Name); // change a player's name
	void SelectArena(uint16_t Index); // set the arena
	void SetNumRounds(int Value); // set the number of rounds to be played

	// during the match
	void UpdatePlayerMovement(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	void DropBomb(uint8_t Slot);
};