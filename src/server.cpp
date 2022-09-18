#include "server.h"

TServer::TServer(TServerListener* SetListener)
:	State(INACTIVE),
	Data(),
	Maps(),
	Listener(SetListener)
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

	State = RUNNING;
}

void TServer::Stop()
{
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

		if (Listener)
		{
			Listener->ServerLobbyCreated();
			Listener->ServerEnteredLobby(Data.GameName);
		}

		result = true;
	}

	return result;
}

void TServer::CloseLobby()
{
	State = RUNNING;

	Data.Status = GAME_NONE;
}

bool TServer::StartMatch()
{
	Data.Status = GAME_MATCHSTARTED;
	if (Listener)
		Listener->ServerMatchStarted();

	StartRound();

	return true;
}

bool TServer::StartRound()
{
	Data.Status = GAME_ROUNDSTARTING;
	if (Listener)
		Listener->ServerRoundStarting();

	Data.InitNewRound();

	Data.Status = GAME_PLAYING;
	if (Listener)
		Listener->ServerRoundStarted();

	return true;
}

bool TServer::EndRound()
{
	Data.Status = GAME_ENDED;
	if (Listener)
		Listener->ServerRoundEnded();

	return true;
}

void TServer::SetGameName(const std::string& SetName)
{
	Data.GameName = SetName;

	if (Listener)
		Listener->ServerGameNameChanged(Data.GameName);
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
		if (Listener)
			Listener->ServerPlayerAdded(result, Data.Players[result].Name);

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
		if (Listener)
			Listener->ServerPlayerRemoved(Slot);

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
		if (Listener)
			Listener->ServerPlayerNameChanged(Slot, Data.Players[Slot].Name);

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
		if (Listener)
			Listener->ServerArenaChanged(ArenaName);

	return result;
}

int TServer::SetNumRounds(int Value)
{
	if (Value < 1)
		Value = 1;
	if (Value > MAX_NUM_ROUNDS)
		Value = MAX_NUM_ROUNDS;

	Data.MaxRounds = Value;

	if (Listener)
		Listener->ServerNumRoundsChanged(Data.MaxRounds);

	return Data.MaxRounds;
}

void TServer::Process(TGameTime Delta)
{
	//Logic->Process(Delta);
}

void TServer::BroadCastChange(int Command, intptr_t Index, intptr_t Value, std::string StrParam)
{
	// TODO
}

void TServer::LogicRoundEnded()
{
	// TODO
}