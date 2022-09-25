#pragma once

#include "gamedata.h"
#include "gamelogic.h"
#include "resourcemgr.h"

#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

const int SERVER_PORT = 45091;

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

	virtual void ServerPlayerAdded(uint8_t Slot, const std::string& PlayerName) = 0;
	virtual void ServerPlayerRemoved(uint8_t Slot) = 0;
	virtual void ServerPlayerNameChanged(uint8_t Slot, const std::string& PlayerName) = 0;

	virtual void ServerMatchStarted() = 0;
	virtual void ServerMatchEnded() = 0;
	virtual void ServerRoundStarting() = 0;
	virtual void ServerRoundStarted() = 0;
	virtual void ServerRoundEnded() = 0;

	virtual void ServerPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down) = 0;
	virtual void ServerPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion) = 0;

	virtual void ServerFullUpdate(TGameData* Data) = 0;
};

class TClientSocket : public sf::TcpSocket
{
public:
	intptr_t ID;
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

	bool ThreadsShouldStop;

	std::thread* NetworkListenerThread;
	sf::TcpListener NetworkListener;
	sf::SocketSelector SocketSelector;
	std::list<TClientSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;

	TClientSocket* FindSocketForConnection(intptr_t ConnectionID);

	// network
	void ListenToNetwork();
	void CheckForDisconnectedSockets();
	void ProcessReceivedPacket(intptr_t ConnectionID, sf::Packet& Packet);
	void SendErrorResponse(TClientSocket* Socket, uint16_t FailedCommand);
	bool SendPacketToSocket(TClientSocket* Socket, sf::Packet& Packet);
	void SendPacketToAllClients(sf::Packet& Packet);
	// process received network communication
	bool ProcessConnectionRequest(intptr_t ConnectionID, uint32_t ClientVersion);
	bool ProcessCreateLobby(intptr_t ConnectionID, const std::string& GameName);
	bool ProcessAddPlayer(intptr_t ConnectionID, uint8_t Slot, const std::string& PlayerName);
	bool ProcessRemovePlayer(intptr_t ConnectionID, uint8_t Slot);
	bool ProcessSetPlayerName(intptr_t ConnectionID, uint8_t Slot, const std::string& PlayerName);
	bool ProcessSetGameName(intptr_t ConnectionID, const std::string& GameName);
	bool ProcessSetNumRounds(intptr_t ConnectionID, uint8_t NumRounds);
	bool ProcessSetArena(intptr_t ConnectionID, const std::string& ArenaName);
	bool ProcessUpdatePlayerMovement(intptr_t ConnectionID, uint8_t Slot, uint8_t Direction);

	void DoLobbyCreated(intptr_t ConnectionID);
	void DoLobbyClosed();
	void DoEnteredLobby(intptr_t ConnectionID, const std::string& GameName);
	void DoLeftLobby(intptr_t ConnectionID);
	void DoGameNameChanged(const std::string& GameName);
	void DoArenaChanged(const std::string& ArenaName);
	void DoNumRoundsChanged(int NumRounds);
	void DoPlayerAdded(uint8_t Slot, const std::string& PlayerName);
	void DoPlayerRemoved(uint8_t Slot);
	void DoPlayerNameChanged(uint8_t Slot, const std::string& PlayerName);
	void DoMatchStarted();
	void DoMatchEnded();
	void DoRoundStarting();
	void DoRoundStarted();
	void DoRoundEnded();
	void DoPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	void DoPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion);
	void DoFullUpdate(TGameTime Delta);
	void DoArenaUpdate(TGameTime Delta);

	void SendPlayerPositionsToAllClients();
	
public:
	TServer(TServerListener* SetListener);
	~TServer();

	void LoadMaps(const std::string& Path);

	void Start(); // activate the server
	void Stop(); // deactivate the server

	bool OpenLobby(intptr_t ConnectionID); // create a lobby for a new game, owned by ConnectionID
	void CloseLobby(); // close the lobby

	bool StartMatch(); // start the first round of the match	
	bool StartRound(); // start a new round when the previous one has ended
	bool EndRound(); // end the current round

	bool SetGameName(const std::string& SetName); // change the game's name
	uint8_t AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	bool RemovePlayer(uint8_t Slot); // remove a player from the current game
	bool SetPlayerName(int Slot, const std::string& Name); // change a player's name
	bool SelectArena(const std::string& ArenaName); // set the arena
	int SetNumRounds(int Value); // set the number of rounds to be played

	void SetPlayerDirections(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	bool DropBomb(uint8_t Slot);

	void Process(TGameTime Delta);

	// inherited from TLogicListener
	void LogicBombExploding(const TFieldPosition& FieldPosition) override;
	void LogicBombExploded(const TFieldPosition& FieldPosition) override;
	void LogicPlayerDying(uint8_t Slot) override;
	void LogicPlayerDied(uint8_t Slot) override;
	void LogicRoundEnded() override; 
};