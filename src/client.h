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
	virtual void ClientPlayerNameChanged(int Slot) = 0;

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
const int CMD_AddPlayer = 2;
const int CMD_RemovePlayer = 3;
const int CMD_StartMatch = 4;
const int CMD_StartNextRound = 5;
const int CMD_SetNumRounds = 6;
const int CMD_SetPlayerName = 7;
const int CMD_SetGameName = 8;
const int CMD_SelectArena = 9;

typedef struct
{
	int Command;
	intptr_t Index, Value;
	std::string StrParam;
} TClientCommand;

class TClient: public TServerListener
{
private:
	int ConnectionStatus;
	TGame* Game;
	sf::TcpSocket Socket;
	std::queue<TClientCommand> Commands;
	void ProcessReceivedPacket(sf::Socket* Source, sf::Packet& Packet);
	void ConnectedToServer();
	void DisconnectedFromServer();
	void DisconnectInternal();
	void ServerPlayerPositionChanged(uint8_t Slot, float X, float Y);
	void ServerBombExploding(const TFieldPosition& Position, TGameTime TimeUntilExploded);
	void ServerBombExploded(const TFieldPosition& Position);
	void ServerPlayerDying(uint8_t Slot, uint16_t TimeUntilDeath);
	void ServerPlayerDied(uint8_t Slot);
	void ServerArenaInfo(uint8_t Width, uint8_t Height, sf::Packet& Packet);
public:
	TClientListener* Listener;

	TClient(TGame* SetGame);
	~TClient();

	void Process(TGameTime Delta);

	// connect to and disconnect from the server
	void Connect(const std::string& ServerAddress, unsigned int ServerPort);
	void Disconnect();

	// start/stop the game, rounds, ...
	void CreateGame(const std::string& SetName); // create a new game / lobby
	void CloseGame(); // leave the current game (removes all players that were added by this client)
	void StartMatch(); // start the first round of the match	
	void StartNextRound(); // start a new round when the previous one has ended

	// set game properties (when in the lobby)
	void SetGameName(const std::string& SetName); // change the game's name
	void AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	void RemovePlayer(uint8_t Slot); // remove a player from the current game
	void SetPlayerName(uint8_t Slot, const std::string& Name); // change a player's name
	void SelectArena(int Index); // set the arena
	void SetNumRounds(int Value); // set the number of rounds to be played

	// during the match
	void UpdatePlayerMovement(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	void DropBomb(uint8_t Slot);

	// from TServerListener
	void ServerLobbyCreated() override;
	void ServerLobbyClosed() override;
	void ServerEnteredLobby(const std::string& GameName) override;
	void ServerLeftLobby() override;
	void ServerGameNameChanged(const std::string& GameName) override;
	void ServerArenaChanged(const std::string& ArenaName) override;
	void ServerNumRoundsChanged(int NumRounds) override;
	void ServerPlayerAdded(uint8_t Slot, const std::string& PlayerName) override;
	void ServerPlayerRemoved(uint8_t Slot) override;
	void ServerPlayerNameChanged(uint8_t Slot, const std::string& PlayerName) override;
	void ServerMatchStarted() override;
	void ServerMatchEnded() override;
	void ServerRoundStarting() override;
	void ServerRoundStarted() override;
	void ServerRoundEnded() override;
	void ServerPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down) override;
	void ServerPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion) override;
	void ServerFullUpdate(TGameData* Data) override;
};