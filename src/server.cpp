#include "server.h"

#include <functional>

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
	delete Logic;
}

void TServer::LoadMaps(const std::string& Path)
{
	Maps.LoadAllMaps(Path);
}

void TServer::Start()
{
	Data.Reset();
	Data.Status = GAME_NONE;

	NetworkListener.listen(45091);
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

	State = INACTIVE;
}

bool TServer::OpenLobby()
{
	bool result = false;

	if (State == RUNNING)
	{
		State = INLOBBY;

		Data.Reset();
		Data.Status = GAME_INLOBBY;

		DoLobbyCreated(0);
		DoEnteredLobby(0, Data.GameName);

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
	Data.Status = GAME_MATCHSTARTED;
	DoMatchStarted();

	StartRound();

	return true;
}

bool TServer::StartRound()
{
	Data.Status = GAME_ROUNDSTARTING;
	DoRoundStarting();

	Data.InitNewRound();

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

void TServer::SetGameName(const std::string& SetName)
{
	Data.GameName = SetName;
	DoGameNameChanged(Data.GameName);
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
	CheckForDisconnectedSockets();

	Logic->Process(Delta);

	DoFullUpdate(&Data);
}

void TServer::LogicRoundEnded()
{
	EndRound();

	if (Data.CurrentRound == Data.MaxRounds)
	{
		Data.Status = GAME_MATCHENDED;
		DoMatchEnded();
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

void TServer::DropBomb(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Data.Players[Slot]);
	if (p->State == PLAYER_NOTPLAYING)
		return;
	if (p->ActiveBombs == p->MaxActiveBombs)
		return;

	// determine the exact position where to drop it (top left pos of the field)
	TFieldPosition pos;
	pos.X = static_cast<int>(trunc(p->Position.X));
	pos.Y = static_cast<int>(trunc(p->Position.Y));

	// check if there's already a bomb at this position
	if (Data.BombInField(pos.X, pos.Y, false))
		return;

	// add the new bomb
	TField* field{};
	Data.Arena.At(pos, field);
	field->Bomb.State = BOMB_TICKING;
	field->Bomb.DroppedByPlayer = Slot;
	field->Bomb.TimeUntilNextState = 2000; // 2 seconds
	field->Bomb.Range = p->BombRange;

	// update the player
	p->ActiveBombs++;

	DoPlayerDroppedBomb(Slot, pos);
}

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

		/*
		// check the connected clients
		{
			std::lock_guard<std::mutex> g(ClientSocketsMutex);
			for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
			{
				TClientSocket* client = *it;
				if (SocketSelector.isReady(*client))
				{
					sf::Packet packet;
					if (client->receive(packet) == sf::Socket::Done)
						Protocol.ProcessReceivedPacket(client, packet, this);	// should try to do this outside of the mutex?
				}
			}
		}
		*/
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
}

void TServer::DoLeftLobby(intptr_t ConnectionID)
{
}

void TServer::DoGameNameChanged(const std::string& GameName)
{
	if (Listener)
		Listener->ServerGameNameChanged(Data.GameName);
}

void TServer::DoArenaChanged(const std::string& ArenaName)
{
	if (Listener)
		Listener->ServerArenaChanged(ArenaName);
}

void TServer::DoNumRoundsChanged(int NumRounds)
{
	if (Listener)
		Listener->ServerNumRoundsChanged(Data.MaxRounds);
}

void TServer::DoPlayerAdded(uint8_t Slot, const std::string& PlayerName)
{
	if (Listener)
		Listener->ServerPlayerAdded(Slot, PlayerName);
}

void TServer::DoPlayerRemoved(uint8_t Slot)
{
	if (Listener)
		Listener->ServerPlayerRemoved(Slot);
}

void TServer::DoPlayerNameChanged(uint8_t Slot, const std::string& PlayerName)
{
	if (Listener)
		Listener->ServerPlayerNameChanged(Slot, PlayerName);
}

void TServer::DoMatchStarted()
{
	if (Listener)
		Listener->ServerMatchStarted();
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
}

void TServer::DoRoundStarted()
{
	if (Listener)
		Listener->ServerRoundStarted();
}

void TServer::DoRoundEnded()
{
	if (Listener)
		Listener->ServerRoundEnded();
}

void TServer::DoPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Listener)
		Listener->ServerPlayerDirectionChanged(Slot, Left, Right, Up, Down);
}

void TServer::DoPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position)
{
	if (Listener)
		Listener->ServerPlayerDroppedBomb(Slot, Position);
}

void TServer::DoFullUpdate(TGameData* Data)
{
	if (Listener)
		Listener->ServerFullUpdate(Data);
}