#include "client.h"

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	Commands()
{
}

void TClient::CreateGame(const std::string& LobbyName)
{
	Game->GameData.GameName = LobbyName;
	Commands.push(CMD_OpenLobby);
}

void TClient::CloseGame()
{
	Game->GameData.Reset();
	if (Listener)
		Listener->ClientDisconnected();
}

bool TClient::AddPlayer(const std::string& PlayerName, uint8_t Slot)
{
	bool success = Game->GameData.AddPlayer(PlayerName, Slot);

	if (success && Listener)
		Listener->ClientPlayerAdded();

	return success;
}

void TClient::RemovePlayer(uint8_t Slot)
{
}

void TClient::StartMatch()
{
	Commands.push(CMD_StartMatch);
}

void TClient::Process(TGameTime Delta)
{
	int cmd;

	while (!Commands.empty())
	{
		cmd = Commands.front();
		Commands.pop();

		switch (cmd)
		{
			case CMD_OpenLobby:
				Listener->ClientConnected();
				Game->GameData.Status = GAME_INLOBBY;
				Listener->ClientEnteredLobby();
				break;

			case CMD_StartMatch:
				Game->GameData.Status = GAME_STARTING;
				Listener->ClientMatchStarting();

				Game->GameData.InitNewGame();

				Game->GameData.Status = GAME_RUNNING;
				Listener->ClientMatchStarted();
		};
	}
}

void TClient::SelectArena(int Index)
{
	// temp: load the standard map file
	Game->GameData.LoadMapFromFile(Game->MapPath + "4CORNERS.SCH");
}