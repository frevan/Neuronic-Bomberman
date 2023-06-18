#include "server.h"

#include <functional>

#include "comms.h"

const TGameTime FULL_UPDATE_INTERVAL = 100;
	
TServer::TServer()
:	TLogicListener(),
	State(INACTIVE),
	Data(),
	Maps(),
	NetworkListenerThread(nullptr),
	NetworkListener(),
	SocketSelector(),
	ClientSockets(),
	ClientSocketsMutex(),
	ThreadsShouldStop(false),
	OwnerID(0),
	CurrentMapIndex(0),
	CurrentTime(0),
	NextFullUpdate(0)
{
	Logic = new TGameLogic(&Data, this);

	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
		Slots[slot].ConnectionID = 0;
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
	if (State == INLOBBY || State == PLAYING)
		CloseLobby(0);

	ThreadsShouldStop = true;
	if (NetworkListenerThread)
	{
		NetworkListenerThread->join();
		delete NetworkListenerThread;
		NetworkListenerThread = nullptr;
	}
	SocketSelector.clear();
	NetworkListener.close();

	std::lock_guard<std::mutex> g(ClientSocketsMutex);
	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		TClientSocket* client = *it;
		delete client;
	}
	ClientSockets.clear();

	State = INACTIVE;
}

bool TServer::OpenLobby(TConnectionID ConnectionID)
{
	bool result = false;
	
	if (State == RUNNING)
	{
		State = INLOBBY;

		Data.Reset();
		Data.Status = GAME_INLOBBY;

		OwnerID = ConnectionID;

		if (Maps.Maps.size() > 0)
			SelectArena(ConnectionID, 0);

		DoGameCreated(ConnectionID);
		DoEnteredLobby(ConnectionID, Data.GameName);

		result = true;
	}

	return result;
}

bool TServer::JoinLobby(TConnectionID ConnectionID)
{
	bool result = false;

	if (State == INLOBBY)
	{
		DoEnteredLobby(ConnectionID, Data.GameName);
		result = true;
	}

	return result;
}

void TServer::CloseLobby(TConnectionID ConnectionID)
{
	if (ConnectionID == OwnerID || ConnectionID == 0)
	{
		State = RUNNING;

		Data.Status = GAME_NONE;

		OwnerID = 0;

		DoGameEnded();
	}
}

bool TServer::StartMatch(TConnectionID ConnectionID)
{
	if (ConnectionID != OwnerID)
		return false;
	if (State == PLAYING)
		return false;

	State = PLAYING;

	Data.Status = GAME_MATCHSTARTED;
	Data.InitNewGame();

	DoMatchStarted();

	StartRound(ConnectionID);

	return true;
}

bool TServer::StartRound(TConnectionID ConnectionID)
{
	if (ConnectionID != OwnerID)
		return false;

	Data.Status = GAME_ROUNDSTARTING;
	DoRoundStarting();

	Data.InitNewRound();

	ResetSocketReceivedTimes();
	DoFullUpdate();

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

bool TServer::SetGameName(TConnectionID ConnectionID, const std::string& SetName)
{
	if (ConnectionID != OwnerID)
		return false;

	Data.GameName = SetName;
	DoGameNameChanged(Data.GameName);

	return true;
}

bool TServer::AddPlayer(TConnectionID ConnectionID, const std::string& PlayerName, uint8_t Slot)
{
	bool result = false;

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

			Slots[Slot].ConnectionID = ConnectionID;

			result = true;
		}

	if (result)
		DoPlayerAdded(Slot, Data.Players[Slot].Name);

	return result;
}

bool TServer::RemovePlayer(TConnectionID ConnectionID, uint8_t Slot)
{
	bool result = false;

	if (Slot < MAX_NUM_SLOTS)
	{
		if (ConnectionID == Slots[Slot].ConnectionID || ConnectionID == OwnerID)
		{

			TPlayer* p = &Data.Players[Slot];
			if (p->State != PLAYER_NOTPLAYING)
			{
				p->State = PLAYER_NOTPLAYING;
				result = true;
			}

			Slots[Slot].ConnectionID = 0;

			if (result)
				DoPlayerRemoved(Slot);
		}
	}

	return result;
}

bool TServer::SetPlayerName(TConnectionID ConnectionID, int Slot, const std::string& Name)
{
	bool result = false;

	if (Slot < MAX_NUM_SLOTS)
	{
		if (ConnectionID == Slots[Slot].ConnectionID || ConnectionID == OwnerID)
		{
			TPlayer* p = &Data.Players[Slot];

			if (p->State != PLAYER_NOTPLAYING)
			{
				p->Name = Name;
				result = true;
			}

			if (result)
				DoPlayerNameChanged(Slot, p->Name);
		}
	}

	return result;
}

bool TServer::SelectArena(TConnectionID ConnectionID, uint16_t Index)
{
	bool result = false;

	if (ConnectionID == OwnerID)
	{
		TArena* map = Maps.MapFromIndex(Index);

		if (map)
		{
			CurrentMapIndex = Index;
			Data.Arena.LoadFromFile(map->OriginalFileName);
			result = true;

			DoArenaChanged(0);
			DoArenaUpdate(0);
		}
	}

	return result;
}

bool TServer::SetNumRounds(TConnectionID ConnectionID, int Value)
{
	if (ConnectionID != OwnerID)
		return false;

	if (Value < 1)
		Value = 1;
	if (Value > MAX_NUM_ROUNDS)
		Value = MAX_NUM_ROUNDS;

	Data.MaxRounds = Value;

	DoNumRoundsChanged(Data.MaxRounds);		

	return true;
}

void TServer::Process(TGameTime Delta)
{
	if (State == INACTIVE)
		return;
	
	CheckForDisconnectedSockets();

	if (State == PLAYING)
	{
		Logic->Process(Delta);

		bool sendFullUpdate = CurrentTime >= NextFullUpdate;
		if (sendFullUpdate)
			DoFullUpdate();

		CurrentTime += Delta;
		if (sendFullUpdate)
			NextFullUpdate = (CurrentTime / FULL_UPDATE_INTERVAL) * FULL_UPDATE_INTERVAL + FULL_UPDATE_INTERVAL;
	}
	else
		CurrentTime = 0;
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

void TServer::SetPlayerDirections(TConnectionID ConnectionID, uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;
	if (ConnectionID != Slots[Slot].ConnectionID)
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

bool TServer::DropBomb(TConnectionID ConnectionID, uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return false;
	if (ConnectionID != Slots[Slot].ConnectionID)
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
	TConnectionID ConnectionID;
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
		ClientDisconnected(client->ID);
		delete client;
	}
}

void TServer::DoGameCreated(TConnectionID ConnectionID)
{
	TClientSocket* socket = FindSocketForConnection(ConnectionID);
	if (socket)
	{
		sf::Packet packet;
		packet << CLN_LobbyCreated;
		SendPacketToSocket(socket, packet);
	}
}

void TServer::DoGameEnded()
{
	sf::Packet packet;
	packet << CLN_LobbyClosed;
	SendPacketToAllClients(packet);
}

void TServer::DoEnteredLobby(TConnectionID ConnectionID, const std::string& GameName)
{
	TClientSocket* socket = FindSocketForConnection(ConnectionID);
	if (socket)
	{
		sf::Packet packet;
		packet << CLN_EnteredLobby << GameName;
		SendPacketToSocket(socket, packet);

		DoArenaNameList(ConnectionID);
		DoPlayerInfoUpdate(ConnectionID);
		DoArenaChanged(ConnectionID);
		DoFullUpdate(ConnectionID);
	}
}

void TServer::DoLeftGame(TConnectionID ConnectionID)
{
	// TODO
}

void TServer::DoGameNameChanged(const std::string& GameName)
{
	sf::Packet packet;
	packet << CLN_GameNameChanged << GameName;
	SendPacketToAllClients(packet);
}

void TServer::DoArenaChanged(TConnectionID ConnectionID)
{
	std::string name = Data.Arena.Caption;
	uint16_t index = CurrentMapIndex;

	sf::Packet packet;
	packet << CLN_ArenaChanged << index << name;

	if (ConnectionID == 0)
		SendPacketToAllClients(packet);
	else
	{
		TClientSocket* socket = FindSocketForConnection(ConnectionID);
		if (socket)
			socket->send(packet);
	}
}

void TServer::DoNumRoundsChanged(int NumRounds)
{
	sf::Packet packet;
	uint16_t rounds = Data.MaxRounds;
	packet << CLN_NumRoundsChanged << rounds;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerAdded(uint8_t Slot, const std::string& PlayerName)
{
	TConnectionID id = Slots[Slot].ConnectionID;

	uint8_t flags = 0;

	sf::Packet packet;

	flags = 1;
	packet << CLN_PlayerAdded << Slot << PlayerName << flags;
	SendPacketToSocket(FindSocketForConnection(id), packet);

	packet.clear();
	flags = 0;
	packet << CLN_PlayerAdded << Slot << PlayerName << flags;
	SendPacketToAllClients(packet, id);
}

void TServer::DoPlayerRemoved(uint8_t Slot)
{
	sf::Packet packet;
	packet << CLN_PlayerRemoved << Slot;
	SendPacketToAllClients(packet);
}

void TServer::DoPlayerNameChanged(uint8_t Slot, const std::string& PlayerName)
{
	sf::Packet packet;
	packet << CLN_PlayerNameChanged << Slot << PlayerName;
	SendPacketToAllClients(packet);
}

void TServer::DoMatchStarted()
{
	uint8_t rounds = Data.MaxRounds;

	sf::Packet packet;
	packet << CLN_MatchStarted << rounds;
	SendPacketToAllClients(packet);
}

void TServer::DoMatchEnded()
{
	// TODO
}

void TServer::DoRoundStarting()
{
	sf::Packet packet;
	packet << CLN_RoundStarting;
	SendPacketToAllClients(packet);
}

void TServer::DoRoundStarted()
{
	sf::Packet packet;
	packet << CLN_RoundStarted;
	SendPacketToAllClients(packet);
}

void TServer::DoRoundEnded()
{
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
	uint8_t x = Position.X;
	uint8_t y = Position.Y;

	sf::Packet packet;
	packet << CLN_BombDropped << Slot << x << y << TimeUntilExplosion;
	SendPacketToAllClients(packet);
}

void TServer::DoFullUpdate(TConnectionID ConnectionID)
{
	// OLD OLD OLD
	//DoArenaUpdate(ConnectionID);
	//DoPlayerPositionUpdate(ConnectionID);

	// NEW NEW NEW

	sf::Packet packet;

	// can't use SendPacketToAllClients for this as the packet contains the individual "last received time" for each socket 
	std::lock_guard<std::mutex> g(ClientSocketsMutex);
	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		TClientSocket* client = *it;

		if (ConnectionID != 0 && client->ID != ConnectionID)
			continue;

		packet.clear();

		// write command + last processed frame index to packet
		packet << CLN_FullMatchUpdate << client->LastReceivedTime;

		// write player state to packet
		for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
		{
			TPlayer* p = &Data.Players[slot];

			uint8_t direction = p->Direction;
			float fieldX = p->Position.X;
			float fieldY = p->Position.Y;
			
			packet << direction << fieldX << fieldY;
		}

		// write arena info to packet
		uint8_t width = (uint8_t)Data.Arena.Width;
		uint8_t height = (uint8_t)Data.Arena.Height;
		TField field{};
		uint8_t fieldType;
		for (uint8_t y = 0; y < height; y++)
			for (uint8_t x = 0; x < width; x++)
			{
				fieldType = Data.Arena.At(x, y).Type;
				packet << fieldType;
			}

		// send
		SendPacketToSocket(client, packet);
	}
}

void TServer::DoArenaUpdate(TConnectionID ConnectionID)
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

	if (ConnectionID == 0)
		SendPacketToAllClients(packet);
	else
	{
		TClientSocket* socket = FindSocketForConnection(ConnectionID);
		if (socket)
			SendPacketToSocket(socket, packet);
	}
}

void TServer::ProcessReceivedPacket(TConnectionID ConnectionID, sf::Packet& Packet)
{
	uint16_t cmd;
	if (!(Packet >> cmd))
		return;

	bool success = false;
	uint8_t u8_1, u8_2;
	uint16_t u16_1;
	uint64_t u64_1;
	std::string s_1;

	switch (cmd)
	{
		case SRV_Connect:
			TProtocolVersion version;
			if (Packet >> version)
				success = ProcessConnectionRequest(ConnectionID, version);
			break;

		case SRV_CreateGame:
			if (Packet >> s_1)
			{
				success = OpenLobby(ConnectionID);
				if (success)
					SetGameName(ConnectionID, s_1);
			}
			break;

		case SRV_JoinGame:
			success = JoinLobby(ConnectionID);
			break;

		case SRV_EndGame: 
			CloseLobby(ConnectionID);
			success = true;
			break;
		case SRV_LeaveGame:
			ClientDisconnected(ConnectionID);
			break;

		case SRV_AddPlayer: 
			if (Packet >> u8_1 >> s_1)
				success = AddPlayer(ConnectionID, s_1, u8_1);
			break;
		case SRV_RemovePlayer:
			if (Packet >> u8_1)
				success = RemovePlayer(ConnectionID, u8_1);
			break;
		case SRV_SetPlayerReady: break;
		case SRV_SetPlayerName: 
			if (Packet >> u8_1 >> s_1)
				success = SetPlayerName(ConnectionID, u8_1, s_1);
			break;

		case SRV_SetGameName: 
			if (Packet >> s_1)
				success = SetGameName(ConnectionID, s_1);
			break;
		case SRV_SetNumRounds: 
			if (Packet >> u8_1)
				success = SetNumRounds(ConnectionID, u8_1);
			break;
		case SRV_SetArena:
			if (Packet >> u16_1)
				success = SelectArena(ConnectionID, u16_1);
			break;

		case SRV_StartMatch:
			success = StartMatch(ConnectionID);
			break;
		case SRV_StartNextRound:
			success = StartRound(ConnectionID);
			break;

		case SRV_UpdatePlayerMovement:
			if (Packet >> u64_1 >> u8_1 >> u8_2)
			{
				TClientSocket* socket = FindSocketForConnection(ConnectionID);
				if (socket)
					socket->LastReceivedTime = u64_1;
				success = ProcessUpdatePlayerMovement(ConnectionID, u8_1, u8_2);
			}
			break;
		case SRV_DropBomb:
			if (Packet >> u64_1 >> u8_1)
			{
				TClientSocket* socket = FindSocketForConnection(ConnectionID);
				if (socket)
					socket->LastReceivedTime = u64_1;
				success = DropBomb(ConnectionID, u8_1);
			}
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

void TServer::SendPacketToAllClients(sf::Packet& Packet, TConnectionID SkipConnectionID)
{
	std::lock_guard<std::mutex> g(ClientSocketsMutex);
	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		if ((*it)->ID != SkipConnectionID)
			SendPacketToSocket((*it), Packet);
	}
}

bool TServer::ProcessConnectionRequest(TConnectionID ConnectionID, uint32_t ClientVersion)
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

bool TServer::ProcessUpdatePlayerMovement(TConnectionID ConnectionID, uint8_t Slot, uint8_t Direction)
{
	bool result = false;

	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
	{
		bool left = Direction & DIRECTION_LEFT;
		bool right = Direction & DIRECTION_RIGHT;
		bool up = Direction & DIRECTION_UP;
		bool down = Direction & DIRECTION_DOWN;

		SetPlayerDirections(ConnectionID, Slot, left, right, up, down);

		result = true;
	}

	return result;
}

TClientSocket* TServer::FindSocketForConnection(TConnectionID ConnectionID)
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

void TServer::DoPlayerInfoUpdate(TConnectionID ConnectionID)
{
	sf::Packet packet;

	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		TPlayer* p = &Data.Players[slot];

		uint8_t state = p->State;
		std::string name = p->Name;

		packet.clear();
		packet << CLN_PlayerInfo << slot << state << name;

		if (ConnectionID == 0)
			SendPacketToAllClients(packet);
		else
		{
			TClientSocket* socket = FindSocketForConnection(ConnectionID);
			if (socket)
				SendPacketToSocket(socket, packet);
		}
	}
}

void TServer::DoPlayerPositionUpdate(TConnectionID ConnectionID)
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

		if (ConnectionID == 0)
			SendPacketToAllClients(packet);
		else
		{
			TClientSocket* socket = FindSocketForConnection(ConnectionID);
			if (socket)
				SendPacketToSocket(socket, packet);
		}
	}
}

void TServer::DoArenaNameList(TConnectionID ConnectionID)
{
	TClientSocket* socket = FindSocketForConnection(ConnectionID);
	if (!socket)
		return;

	sf::Packet packet;

	uint16_t count = (uint16_t)Maps.Maps.size();
	uint16_t index = 0;
	for (auto it = Maps.Maps.begin(); it != Maps.Maps.end(); it++)
	{
		std::string name = (*it)->Caption;

		packet.clear();
		packet << CLN_ArenaList << count << index << name;
		socket->send(packet);

		index++;
	}
}

void TServer::ClientDisconnected(TConnectionID ConnectionID)
{
	// remove players for this client
	for (uint8_t slot = 0; slot < MAX_NUM_SLOTS; slot++)
	{
		if (Slots[slot].ConnectionID == ConnectionID)
			RemovePlayer(ConnectionID, slot);
	}

	if (ConnectionID == OwnerID)
	{
		CloseLobby(ConnectionID);
	}
	else
	{

	}
}

void TServer::ResetSocketReceivedTimes()
{
	std::lock_guard<std::mutex> g(ClientSocketsMutex);
	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		TClientSocket* client = *it;
		client->LastReceivedTime = 0;
	}
}