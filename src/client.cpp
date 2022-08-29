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
				Game->GameData.Reset();
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
	TArena* map = Game->Maps.MapFromIndex(Index);
	if (!map)
		return;

	// temp: load the standard map file
	Game->GameData.Arena.LoadFromFile(map->OriginalFileName);
}

void TClient::UpdatePlayerMovement(int Slot, TPlayerDirection Direction, bool SetActive)
{
	if (Slot < 0 || Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;
	if (Game->GameData.Players[Slot].State == PLAYER_NOTPLAYING)
		return;

	if (SetActive)
		Game->GameData.Players[Slot].Direction |= Direction;
	else
		Game->GameData.Players[Slot].Direction &= ~Direction;
}

void TClient::DropBomb(int Slot)
{
	if (Slot < 0 || Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;
	if (Game->GameData.Players[Slot].State == PLAYER_NOTPLAYING)
		return;

	// determine the exact position where to drop it (top left pos of the field)
	TFieldPosition pos;
	pos.X = static_cast<int>(trunc(Game->GameData.Players[Slot].Position.X));
	pos.Y = static_cast<int>(trunc(Game->GameData.Players[Slot].Position.Y));

	// check if there's already a bomb at this position
	if (Game->GameData.BombInField(pos.X, pos.Y))
		return;

	// add the new bomb
	TField* field = Game->GameData.Arena.At(pos);
	field->Bomb.State = BOMB_TICKING;
	field->Bomb.TimeUntilNextState = 5000; // 5 seconds
	field->Bomb.Range = 1;
}