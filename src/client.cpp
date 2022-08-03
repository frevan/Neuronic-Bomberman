#include "client.h"

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	NextCommand(CMD_None)
{
}

void TClient::CreateGame(const std::string& LobbyName)
{
	Game->GameData.GameName = LobbyName;
	NextCommand = CMD_OpenLobby;
}

void TClient::CloseGame()
{
	Game->GameData.Reset();
	if (Listener)
		Listener->OnDisconnected();
}

bool TClient::AddPlayer(const std::string& PlayerName, uint8_t Slot)
{
	bool success = Game->GameData.AddPlayer(PlayerName, Slot);

	if (success && Listener)
		Listener->OnPlayerAdded();

	return success;
}

void TClient::RemovePlayer(uint8_t Slot)
{
}

void TClient::StartMatch()
{
}

void TClient::Process(TGameTime Delta)
{
	switch (NextCommand)
	{
		case CMD_OpenLobby:
			Listener->OnConnected();
			Game->GameData.Status = GAME_INLOBBY;
			Listener->OnEnteredLobby();
			break;
	};

	NextCommand = CMD_None;
}