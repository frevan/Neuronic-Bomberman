#include "server.h"

#include <functional>
#include "comms.h"

TServer::TServer(TServerListener* SetListener)
:	State(INACTIVE),
	Data(),
	Maps(),
	Listener(SetListener),
	NetworkListenerThread(nullptr),
	NetworkListener(),
	SocketSelector(),
	ClientSockets(),
	ClientSocketsMutex(),
	ThreadsShouldStop(false)
{
	Logic = new TGameLogic(&Data, this);
}

TServer::~TServer()
{
	Stop();

	delete Logic;
}

void TServer::LoadMaps(const std::string& Path)
{
	Maps.LoadAllMaps(Path);
}

void TServer::Start()
{
	if (State != INACTIVE)
		Stop();

	Data.Reset();
	Data.Status = GAME_NONE;

	NetworkListener.listen(SERVER_PORT);
	SocketSelector.add(NetworkListener);
	ThreadsShouldStop = false;
	NetworkListenerThread = new std::thread(std::bind(&TServer::ListenToNetwork, this));

	State = RUNNING;
}

void TServer::Stop()
{
	ThreadsShouldStop = true;
	if (NetworkListenerThread)
	{
		NetworkListenerThread->join();
		delete NetworkListenerThread;
		NetworkListenerThread = nullptr;
	}
	SocketSelector.clear();
	NetworkListener.close();

	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		TClientSocket* client = *it;
		delete client;
	}
	ClientSockets.clear();

	State = INACTIVE;
}

bool TServer::OpenLobby(intptr_t ConnectionID)
{
	bool result = false;

	if (State == RUNNING)
	{
		State = INLOBBY;

		Data.Reset();
		Data.Status = GAME_INLOBBY;

		DoLobbyCreated(ConnectionID);
		DoEnteredLobby(ConnectionID, Data.GameName);

		result = true;
	}

	return result;
}

void TServer::CloseLobby()
{
	State = RUNNING;

	Data.Status = GAME_NONE;

	DoLobbyClosed();
}

bool TServer::StartMatch()
{
	State = PLAYING;

	Data.Status = GAME_MATCHSTARTED;
	Data.InitNewGame();

	DoMatchStarted();

	StartRound();

	return true;
}

bool TServer::StartRound()
{
	Data.Status = GAME_ROUNDSTARTING;
	DoRoundStarting();

	Data.InitNewRound();

	DoFullUpdate(0);

	Data.Status = GAME_PLAYING;
	DoRoundStarted();

	return true;
}

bool TServer::EndRound()
{
	Data.Status = GAME_ROUNDENDED;
	DoRoundEnded();

	return true;
}

bool TServer::SetGameName(const std::string& SetName)
{
	Data.GameName = SetName;
	DoGameNameChanged(Data.GameName);

	return true;
}

uint8_t TServer::AddPlayer(const std::string& PlayerName, uint8_t Slot)
{
	uint8_t result = MAX_NUM_SLOTS;

	// find a free slot if necessary
	if (Slot >= MAX_NUM_SLOTS)
	{
		for (int i = 0; i < MAX_NUM_SLOTS; i++)
			if (Data.Players[i].State == PLAYER_NOTPLAYING)
			{
				Slot = i;
				break;
			}
	}

	// check if the slot is free
	if (Slot < MAX_NUM_SLOTS)
		if (Data.Players[Slot].State == PLAYER_NOTPLAYING)
		{
			Data.Players[Slot].State = PLAYER_ALIVE;
			Data.Players[Slot].Name = PlayerName;

			result = Slot;
		}

	if (result < MAX_NUM_SLOTS)
		DoPlayerAdded(result, Data.Players[result].Name);

	return result;
}

bool TServer::RemovePlayer(uint8_t Slot)
{
	bool result = false;

	if (Slot < MAX_NUM_SLOTS)
		if (Data.Players[Slot].State != PLAYER_NOTPLAYING)
		{
			Data.Players[Slot].State = PLAYER_NOTPLAYING;

			result = true;
		}

	if (result)
		DoPlayerRemoved(Slot);

	return result;
}

bool TServer::SetPlayerName(int Slot, const std::string& Name)
{
	bool result = false;

	if (Slot < MAX_NUM_SLOTS)
		if (Data.Players[Slot].State != PLAYER_NOTPLAYING)
		{
			Data.Players[Slot].Name = Name;

			result = true;
		}

	if (result)
		DoPlayerNameChanged(Slot, Data.Players[Slot].Name);

	return result;
}

bool TServer::SelectArena(const std::string& ArenaName)
{
	bool result = false;

	TArena* map =  Maps.MapFromName(ArenaName);

	if (map)
	{
		Data.Arena.LoadFromFile(map->OriginalFileName);
		result = true;
	}

	if (result)
		DoArenaChanged(ArenaName);

	return result;
}

int TServer::SetNumRounds(int Value)
{
	if (Value < 1)
		Value = 1;
	if (Value > MAX_NUM_ROUNDS)
		Value = MAX_NUM_ROUNDS;

	Data.MaxRounds = Value;

	DoNumRoundsChanged(Data.MaxRounds);		

	return Data.MaxRounds;
}

void TServer::Process(TGameTime Delta)
{
	if (State == INACTIVE)
		return;
	
	CheckForDisconnectedSockets();

	if (State == PLAYING)
	{
		Logic->Process(Delta);

		DoFullUpdate(Delta);
	}
}

void TServer::LogicBombExploding(const TFieldPosition& FieldPosition)
{
	TField* field = nullptr;
	Data.Arena.At(FieldPosition, field);
	if (field)
	{
		uint8_t x = FieldPosition.X;
		uint8_t y = FieldPosition.Y;
		uint16_t time = field->Bomb.TimeUntilNextState;

		sf::Packet packet;
		packet << CLN_BombExploding << x << y << time;
		SendPacketToAllClients(packet);
	}
}

void TServer::LogicBombExploded(const TFieldPosition& FieldPosition)
{
	uint8_t x = FieldPosition.X;
	uint8_t y = FieldPosition.Y;

	sf::Packet packet;
	packet << CLN_BombExploded << x << y;
	SendPacketToAllClients(packet);
}

void TServer::LogicPlayerDying(uint8_t Slot)
{
	uint16_t time = Data.Players[Slot].TimeUntilNextState;

	sf::Packet packet;
	packet << CLN_PlayerDying << Slot << time;
	SendPacketToAllClients(packet);
}

void TServer::LogicPlayerDied(uint8_t Slot)
{
	sf::Packet packet;
	packet << CLN_PlayerDied << Slot;
	SendPacketToAllClients(packet);
}

void TServer::LogicRoundEnded()
{
	EndRound();

	if (Data.CurrentRound == Data.MaxRounds)
	{
		Data.Status = GAME_MATCHENDED;
		DoMatchEnded();

		State = RUNNING;
	}
}

void TServer::SetPlayerDirections(uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Data.Players[Slot]);
	if (p->State == PLAYER_NOTPLAYING)
		return;

	if (Left)
		p->Direction |= DIRECTION_LEFT;
	else
		p->Direction &= ~DIRECTION_LEFT;
	if (Right)
		p->Direction |= DIRECTION_RIGHT;
	else
		p->Direction &= ~DIRECTION_RIGHT;
	if (Up)
		p->Direction |= DIRECTION_UP;
	else
		p->Direction &= ~DIRECTION_UP;
	if (Down)
		p->Direction |= DIRECTION_DOWN;
	else
		p->Direction &= ~DIRECTION_DOWN;

	DoPlayerDirectionChanged(Slot, Left, Right, Up, Down);
}

bool TServer::DropBomb(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return false;

	TPlayer* p = &(Data.Players[Slot]);
	if (p->State == PLAYER_NOTPLAYING)
		return false;
	if (p->ActiveBombs == p->MaxActiveBombs)
		return false;

	// determine the exact position where to drop it (top left pos of the field)
	TFieldPosition pos;
	pos.X = static_cast<int>(trunc(p->Position.X));
	pos.Y = static_cast<int>(trunc(p->Position.Y));

	// check if there's already a bomb at this position
	if (Data.BombInField(pos.X, pos.Y, false))
		return false;

	// add the new bomb
	TField* field{};
	Data.Arena.At(pos, field);
	if (!field)
		return false;

	field->Bomb.State = BOMB_TICKING;
	field->Bomb.DroppedByPlayer = Slot;
	field->Bomb.TimeUntilNextState = 2000; // 2 seconds
	field->Bomb.Range = p->BombRange;

	// update the player
	p->ActiveBombs++;

	DoPlayerDroppedBomb(Slot, pos, field->Bomb.TimeUntilNextState);

	return true;
}

typedef struct
{
	intptr_t ConnectionID;
	sf::Packet* Packet;
} TPacketInfoRec;

void TServer::ListenToNetwork()
{
	while (!ThreadsShouldStop)
	{
		// wait to receive something
		if (!SocketSelector.wait(sf::milliseconds(50)))
			continue;

		if (ThreadsShouldStop)
			break;

		// check for a new connection
		if (SocketSelector.isReady(NetworkListener))
		{
			TClientSocket* newClient = new TClientSocket;
			newClient->ID = (intptr_t)newClient;

			if (NetworkListener.accept(*newClient) == sf::Socket::Done)
			{
				std::lock_guard<std::mutex> g(ClientSocketsMutex);
				SocketSelector.add(*newClient);
				ClientSockets.push_back(newClient);
			}
			else
				delete newClient;
		};

		// receive packets from the connected clients
		std::list<TPacketInfoRec> packetList;
		{
			std::lock_guard<std::mutex> g(ClientSocketsMutex);
			for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
			{
				TClientSocket* client = *it;
				if (SocketSelector.isReady(*client))
				{
					TPacketInfoRec info;
					info.Packet = new sf::Packet();
					if (client->receive(*info.Packet) == sf::Socket::Done)
					{
						info.ConnectionID = client->ID;
						packetList.push_back(info);
					}
					else
						delete info.Packet;			
				}
			}
		}
		// process packets outside of the mutex
		for (auto it = packetList.begin(); it != packetList.end(); it++)
		{
			ProcessReceivedPacket((*it).ConnectionID, *(*it).Packet);	// should try to do this outside of the mutex?
			delete (*it).Packet;
		}
	}
}

void TServer::CheckForDisconnectedSockets()
{
	// look for sockets to remove
	std::list<TClientSocket*> removedSockets;
	{
		std::lock_guard<std::mutex> g(ClientSocketsMutex);
		for (auto it = ClientSockets.begin(); it != ClientSockets.end(); )
		{
			TClientSocket* client = *it++;	// increment here because otherwise there will be a runtime error when it's incremented in the for loop
			if (client->getRemoteAddress() == sf::IpAddress::None)
			{
				SocketSelector.remove(*client);
				ClientSockets.remove(client);
				removedSockets.push_back(client);
			}
		}
	}

	for (auto it = removedSockets.begin(); it != removedSockets.end(); it++)
	{
		TClientSocket* client = *it;
		delete client;
	}
}

void TServer::DoLobbyCreated(intptr_t ConnectionID)
{
	if (Listener)
		Listener->ServerLobbyCreated();

	TClientSocket* socket = FindSocketForConnection(ConnectionID);
	if (socket)
	{
		sf::Packet packet;
		packet << CLN_LobbyCreated;
		SendPacketToSocket(socket, packet);
	}
}

void TServer::DoLobbyClosed()
{
	if (Listener)
		Listener->ServerLobbyClosed();
}

void TServer::DoEnteredLobby(intptr_t ConnectionID, const std::string& GameName)
{
	if (Listener)
		Listener->ServerEnteredLobby(Data.GameName);

	TClientSocket* socket = FindSocketForConnection(ConnectionID);
	if (socket)
	{
		sf::Packet packet;
		packet << CLN_EnteredLobby << GameName;
		SendPacketToSocket(socket, packet);
	}
}

void TServer::DoLeftLobby(intptr_t ConnectionID)
{
}

void TServer::DoGameNameChanged(const std::string& GameName)
{
	if (Listener)
		Listener->ServerGameNameChanged(GameName);

	sf::Packet packet;
	packet << CLN_GameNameChanged << GameName;
	SendPacketToAllClients(packet);
}

void TServer::DoArenaChanged(const std::string& ArenaName)
{
	if (Listener)
		Listener->ServerArenaChanged(ArenaName);

	sf::Packet packet;
	packet << CLN_ArenaChanged << ArenaName;
	SendPacketToAllClients(packet);
}

void TServer::DoNumRoundsChanged(int NumRounds)
{
	if (Listener)
		Listener->ServerNumRoundsChanged(Data.MaxRounds);

	sf::Packet packet;
	packet << CLN_NumRoundsChanged << Data.MaxRounds;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerAdded(uint8_t Slot, const std::string& PlayerName)
{
	if (Listener)
		Listener->ServerPlayerAdded(Slot, PlayerName);

	sf::Packet packet;
	packet << CLN_PlayerAdded << Slot << PlayerName;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerRemoved(uint8_t Slot)
{
	if (Listener)
		Listener->ServerPlayerRemoved(Slot);

	sf::Packet packet;
	packet << CLN_PlayerRemoved << Slot;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerNameChanged(uint8_t Slot, const std::string& PlayerName)
{
	if (Listener)
		Listener->ServerPlayerNameChanged(Slot, PlayerName);

	sf::Packet packet;
	packet << CLN_PlayerNameChanged << Slot << PlayerName;
	SendPacketToAllClients(packet);
}

void TServer::DoMatchStarted()
{
	if (Listener)
		Listener->ServerMatchStarted();

	uint8_t rounds = Data.MaxRounds;

	sf::Packet packet;
	packet << CLN_MatchStarted << rounds;
	SendPacketToAllClients(packet);
}

void TServer::DoMatchEnded()
{
	if (Listener)
		Listener->ServerMatchEnded();
}

void TServer::DoRoundStarting()
{
	if (Listener)
		Listener->ServerRoundStarting();

	sf::Packet packet;
	packet << CLN_RoundStarting;
	SendPacketToAllClients(packet);
}

void TServer::DoRoundStarted()
{
	if (Listener)
		Listener->ServerRoundStarted();

	sf::Packet packet;
	packet << CLN_RoundStarted;
	SendPacketToAllClients(packet);
}

void TServer::DoRoundEnded()
{
	if (Listener)
		Listener->ServerRoundEnded();

	sf::Packet packet;
	packet << CLN_RoundEnded;
	SendPacketToAllClients(packet);

	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Data.Players[slot];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		uint8_t score = p->RoundsWon;
		
		packet.clear();
		packet << CLN_PlayerScore << slot << score;
		SendPacketToAllClients(packet);
	}
}

void TServer::DoPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Listener)
		Listener->ServerPlayerDirectionChanged(Slot, Left, Right, Up, Down);

	TPlayer* p = &Data.Players[Slot];

	uint8_t direction = p->Direction;
	float fieldX = p->Position.X;
	float fieldY = p->Position.Y;

	sf::Packet packet;
	packet << CLN_PlayerMovement << Slot << direction << fieldX << fieldY;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion)
{
	if (Listener)
		Listener->ServerPlayerDroppedBomb(Slot, Position, TimeUntilExplosion);

	uint8_t x = Position.X;
	uint8_t y = Position.Y;

	sf::Packet packet;
	packet << CLN_BombDropped << Slot << x << y << TimeUntilExplosion;
	SendPacketToAllClients(packet);
}

void TServer::DoFullUpdate(TGameTime Delta)
{
	if (Listener)
		Listener->ServerFullUpdate(&Data);

	DoArenaUpdate(Delta);
	SendPlayerPositionsToAllClients();
}

void TServer::DoArenaUpdate(TGameTime Delta)
{
	sf::Packet packet;

	packet << CLN_ArenaInfo;

	uint8_t width = (uint8_t)Data.Arena.Width;
	uint8_t height = (uint8_t)Data.Arena.Height;
	packet << width << height;

	TField field{};
	uint8_t fieldType;
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
		{
			fieldType = Data.Arena.At(x, y).Type;
			packet << fieldType;
		}

	SendPacketToAllClients(packet);
}

void TServer::ProcessReceivedPacket(intptr_t ConnectionID, sf::Packet& Packet)
{
	uint16_t cmd;
	if (!(Packet >> cmd))
		return;

	bool success = false;
	uint8_t u8_1, u8_2;
	std::string s_1;

	switch (cmd)
	{
		case SRV_Connect:
			TProtocolVersion version;
			if (Packet >> version)
				success = ProcessConnectionRequest(ConnectionID, version);
			break;

		case SRV_CreateLobby: 
			if (Packet >> s_1)
				success = ProcessCreateLobby(ConnectionID, s_1);
			break;

		case SRV_CloseLobby: 
			break;

		case SRV_AddPlayer: 
			if (Packet >> u8_1 >> s_1)
				success = ProcessAddPlayer(ConnectionID, u8_1, s_1);
			break;
		case SRV_RemovePlayer:
			if (Packet >> u8_1)
				success = ProcessRemovePlayer(ConnectionID, u8_1);
			break;
		case SRV_SetPlayerReady: break;
		case SRV_SetPlayerName: 
			if (Packet >> u8_1 >> s_1)
				success = ProcessSetPlayerName(ConnectionID, u8_1, s_1);
			break;

		case SRV_SetGameName: 
			if (Packet >> s_1)
				success = ProcessSetGameName(ConnectionID, s_1);
			break;
		case SRV_SetNumRounds: 
			if (Packet >> u8_1)
				success = ProcessSetNumRounds(ConnectionID, u8_1);
			break;
		case SRV_SetArena:
			if (Packet >> s_1)
				success = ProcessSetArena(ConnectionID, s_1);
			break;

		case SRV_StartMatch:
			success = StartMatch();
			break;
		case SRV_StartNextRound:
			success = StartRound();
			break;

		case SRV_UpdatePlayerMovement:
			if (Packet >> u8_1 >> u8_2)
				success = ProcessUpdatePlayerMovement(ConnectionID, u8_1, u8_2);
			break;
		case SRV_DropBomb:
			if (Packet >> u8_1)
				success = DropBomb(u8_1);
			break;
	};

	if (!success)
	{
		TClientSocket* socket = FindSocketForConnection(ConnectionID);
		if (socket)
			SendErrorResponse(socket, cmd);
	}
}

void TServer::SendErrorResponse(TClientSocket* Dest, uint16_t FailedCommand)
{
	sf::Packet packet;
	packet << CLN_CommandFailed << FailedCommand;
	SendPacketToSocket(Dest, packet);
}

bool TServer::SendPacketToSocket(TClientSocket* Socket, sf::Packet& Packet)
{
	sf::TcpSocket* sck = static_cast<sf::TcpSocket*>(Socket);
	sf::Socket::Status status = sck->send(Packet);

	return (status == sf::Socket::Status::Done);
}

void TServer::SendPacketToAllClients(sf::Packet& Packet)
{
	std::lock_guard<std::mutex> g(ClientSocketsMutex);
	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
		SendPacketToSocket((*it), Packet);
}

bool TServer::ProcessConnectionRequest(intptr_t ConnectionID, uint32_t ClientVersion)
{
	bool result = false;

	if (ClientVersion == SERVER_VERSION)
	{
		result = true;

		TClientSocket* socket = FindSocketForConnection(ConnectionID);
		if (socket)
		{
			sf::Packet packet;
			packet << CLN_Connected;
			SendPacketToSocket(socket, packet);
		}
	}

	return result;
}

bool TServer::ProcessCreateLobby(intptr_t ConnectionID, const std::string& GameName)
{
	bool result = false;

	// TODO: don't create more than one lobby!

	if (OpenLobby(ConnectionID))
	{
		result = true;
		SetGameName(GameName);
	}

	return result;
}

bool TServer::ProcessAddPlayer(intptr_t ConnectionID, uint8_t Slot, const std::string& PlayerName)
{
	bool result = false;

	if (AddPlayer(PlayerName, Slot))
		result = true;

	return result;
}

bool TServer::ProcessRemovePlayer(intptr_t ConnectionID, uint8_t Slot)
{
	bool result = false;

	if (RemovePlayer(Slot))
		result = true;

	return result;
}

bool TServer::ProcessSetPlayerName(intptr_t ConnectionID, uint8_t Slot, const std::string& PlayerName)
{
	bool result = false;

	if (SetPlayerName(Slot, PlayerName))
		result = true;

	return result;
}

bool TServer::ProcessSetGameName(intptr_t ConnectionID, const std::string& GameName)
{
	return SetGameName(GameName);
}

bool TServer::ProcessSetNumRounds(intptr_t ConnectionID, uint8_t NumRounds)
{
	bool result = false;

	if (NumRounds >= 0 && NumRounds <= MAX_NUM_ROUNDS)
	{
		SetNumRounds(NumRounds);
		result = true;
	}

	return result;
}

bool TServer::ProcessSetArena(intptr_t ConnectionID, const std::string& ArenaName)
{
	return SelectArena(ArenaName);
}

bool TServer::ProcessUpdatePlayerMovement(intptr_t ConnectionID, uint8_t Slot, uint8_t Direction)
{
	bool result = false;

	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
	{
		bool left = Direction & DIRECTION_LEFT;
		bool right = Direction & DIRECTION_RIGHT;
		bool up = Direction & DIRECTION_UP;
		bool down = Direction & DIRECTION_DOWN;

		SetPlayerDirections(Slot, left, right, up, down);

		result = true;
	}

	return result;
}


TClientSocket* TServer::FindSocketForConnection(intptr_t ConnectionID)
{
	TClientSocket* result = nullptr;

	{
		std::lock_guard<std::mutex> g(ClientSocketsMutex);
		
		for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
			if ((*it)->ID == ConnectionID)
			{
				result = (*it);
				break;
			}
	}

	return result;
}

void TServer::SendPlayerPositionsToAllClients()
{
	sf::Packet packet;

	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Data.Players[slot];
		if (p->State == PLAYER_NOTPLAYING)
			continue;

		uint8_t direction = p->Direction;
		float fieldX = p->Position.X;
		float fieldY = p->Position.Y;

		packet.clear();
		packet << CLN_PlayerMovement << slot << direction << fieldX << fieldY;
		SendPacketToAllClients(packet);
	}
}