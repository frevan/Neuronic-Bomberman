#pragma once

#include "gamedata.h"
#include "gamelogic.h"
#include "resourcemgr.h"

#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

typedef intptr_t TConnectionID;

class TClientSocket : public sf::TcpSocket
{
public:
	TConnectionID ID;
};

typedef struct 
{
	TConnectionID ConnectionID;
} TSlotInfo;

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
	uint16_t CurrentMapIndex;
	TConnectionID OwnerID;
	TSlotInfo Slots[MAX_NUM_SLOTS];

	bool ThreadsShouldStop;

	std::thread* NetworkListenerThread;
	sf::TcpListener NetworkListener;
	sf::SocketSelector SocketSelector;
	std::list<TClientSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;

	// utility
	TClientSocket* FindSocketForConnection(TConnectionID ConnectionID);
	void SendErrorResponse(TClientSocket* Socket, uint16_t FailedCommand);
	bool SendPacketToSocket(TClientSocket* Socket, sf::Packet& Packet);
	void SendPacketToAllClients(sf::Packet& Packet);

	// receive input from network
	void ListenToNetwork();
	void CheckForDisconnectedSockets();
	void ProcessReceivedPacket(TConnectionID ConnectionID, sf::Packet& Packet);
	// process received network communication
	bool ProcessConnectionRequest(TConnectionID ConnectionID, uint32_t ClientVersion);
	bool ProcessUpdatePlayerMovement(TConnectionID ConnectionID, uint8_t Slot, uint8_t Direction);

	// notify connected clients
	void DoGameCreated(TConnectionID ConnectionID);
	void DoGameEnded();
	void DoEnteredLobby(TConnectionID ConnectionID, const std::string& GameName);
	void DoLeftGame(TConnectionID ConnectionID);
	void DoGameNameChanged(const std::string& GameName);
	void DoArenaChanged(TConnectionID ConnectionID);
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
	void DoFullUpdate(TConnectionID ConnectionID = 0);
	void DoArenaUpdate(TConnectionID ConnectionID = 0);
	void DoPlayerPositionUpdate(TConnectionID ConnectionID = 0);
	void DoPlayerInfoUpdate(TConnectionID ConnectionID = 0);
	void DoArenaNameList(TConnectionID ConnectionID);

	// do things
	bool OpenLobby(TConnectionID ConnectionID); // create a lobby for a new game, owned by ConnectionID
	bool JoinLobby(TConnectionID ConnectionID);  // join an existing lobby
	void CloseLobby(TConnectionID ConnectionID); // close the lobby
	bool StartMatch(TConnectionID ConnectionID); // start the first round of the match	
	bool StartRound(TConnectionID ConnectionID); // start a new round when the previous one has ended
	bool EndRound(); // end the current round
	bool SetGameName(TConnectionID ConnectionID, const std::string& SetName); // change the game's name
	bool AddPlayer(TConnectionID ConnectionID, const std::string& PlayerName, uint8_t Slot = INVALID_SLOT); // add a player to the current game
	bool RemovePlayer(TConnectionID ConnectionID, uint8_t Slot); // remove a player from the current game
	bool SetPlayerName(TConnectionID ConnectionID, int Slot, const std::string& Name); // change a player's name
	bool SelectArena(TConnectionID ConnectionID, uint16_t Index); // set the arena
	bool SetNumRounds(TConnectionID ConnectionID, int Value); // set the number of rounds to be played
	void SetPlayerDirections(TConnectionID ConnectionID, uint8_t Slot, bool Left, bool Right, bool Up, bool Down);
	bool DropBomb(TConnectionID ConnectionID, uint8_t Slot);
	void ClientDisconnected(TConnectionID ConnectionID);
	
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