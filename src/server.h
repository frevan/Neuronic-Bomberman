#pragma once

#include "gamedata.h"
#include "gamelogic.h"
#include "resourcemgr.h"

#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

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

	bool ThreadsShouldStop;

	std::thread* NetworkListenerThread;
	sf::TcpListener NetworkListener;
	sf::SocketSelector SocketSelector;
	std::list<TClientSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;

	// utility
	TClientSocket* FindSocketForConnection(intptr_t ConnectionID);
	void SendErrorResponse(TClientSocket* Socket, uint16_t FailedCommand);
	bool SendPacketToSocket(TClientSocket* Socket, sf::Packet& Packet);
	void SendPacketToAllClients(sf::Packet& Packet);

	// receive input from network
	void ListenToNetwork();
	void CheckForDisconnectedSockets();
	void ProcessReceivedPacket(intptr_t ConnectionID, sf::Packet& Packet);
	// process received network communication
	bool ProcessConnectionRequest(intptr_t ConnectionID, uint32_t ClientVersion);
	bool ProcessUpdatePlayerMovement(intptr_t ConnectionID, uint8_t Slot, uint8_t Direction);

	// notify connected clients
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
	void DoArenaUpdate();
	void DoPlayerPositionUpdate();

	// do things
	bool OpenLobby(intptr_t ConnectionID); // create a lobby for a new game, owned by ConnectionID
	void CloseLobby(); // close the lobby
	bool StartMatch(); // start the first round of the match	
	bool StartRound(); // start a new round when the previous one has ended
	bool EndRound(); // end the current round
	bool SetGameName(const std::string& SetName); // change the game's name
	bool AddPlayer(const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	bool RemovePlayer(uint8_t Slot); // remove a player from the current game
	bool SetPlayerName(int Slot, const std::string& Name); // change a player's name
	bool SelectArena(const std::string& ArenaName); // set the arena
	bool SetNumRounds(int Value); // set the number of rounds to be played
	void SetPlayerDirections(uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	bool DropBomb(uint8_t Slot);
	
public:
	TServer();
	~TServer();

	// load resources
	void LoadMaps(const std::string& Path);

	// activate and deactivate the server
	void Start();
	void Stop();

	// do things
	void Process(TGameTime Delta);

	// inherited from TLogicListener
	void LogicBombExploding(const TFieldPosition& FieldPosition) override;
	void LogicBombExploded(const TFieldPosition& FieldPosition) override;
	void LogicPlayerDying(uint8_t Slot) override;
	void LogicPlayerDied(uint8_t Slot) override;
	void LogicRoundEnded() override; 
};