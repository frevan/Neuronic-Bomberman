#include "client.h"

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	Commands()
{
}

void TClient::CreateGame(const std::string& LobbyName)
{
	TClientCommand cmd;
	cmd.Command = CMD_OpenLobby;
	cmd.StrParam = LobbyName;
	Commands.push(cmd);
}

void TClient::CloseGame()
{
	Game->GameData.Reset();
	if (Listener)
		Listener->ClientDisconnected();
}

void TClient::StartNextRound()
{
	TClientCommand cmd;
	cmd.Command = CMD_StartNextRound;
	Commands.push(cmd);
}

void TClient::EndRound()
{
	TClientCommand cmd;
	cmd.Command = CMD_EndRound;
	Commands.push(cmd);
}

void TClient::AddPlayer(const std::string& PlayerName, uint8_t Slot)
{
	TClientCommand cmd;
	cmd.Command = CMD_AddPlayer;
	cmd.StrParam = PlayerName;
	cmd.Index = Slot;
	Commands.push(cmd);
}

void TClient::RemovePlayer(uint8_t Slot)
{
	TClientCommand cmd;
	cmd.Command = CMD_RemovePlayer;
	cmd.Index = Slot;
	Commands.push(cmd);
}

void TClient::StartMatch()
{
	TClientCommand cmd;
	cmd.Command = CMD_StartMatch;
	Commands.push(cmd);
}

void TClient::Process(TGameTime Delta)
{
	TClientCommand cmd;
	bool success;
	int slot;

	while (!Commands.empty())
	{
		cmd = Commands.front();
		Commands.pop();

		switch (cmd.Command)
		{
			case CMD_OpenLobby:
				if (Listener)
					Listener->ClientConnected();
				Game->GameData.Reset();
				Game->GameData.GameName = cmd.StrParam;
				Game->GameData.Status = GAME_INLOBBY;
				if (Listener)
					Listener->ClientEnteredLobby();
				break;

			case CMD_SetGameName:
				Game->GameData.SetName(cmd.StrParam);
				if (Listener)
					Listener->ClientGameOptionsChanged();
				break;

			case CMD_AddPlayer:
				slot = (int)cmd.Index;
				success = Game->GameData.AddPlayer(cmd.StrParam, slot);
				if (Listener)
				{
					if (success)
						Listener->ClientPlayerAdded(slot);
					else
						Listener->ClientPlayerNotAdded(slot);
				}
				break;

			case CMD_RemovePlayer:
				slot = (int)cmd.Index;
				Game->GameData.RemovePlayer(slot);
				if (Listener)
					Listener->ClientPlayerRemoved(slot);
				break;

			case CMD_SetPlayerName:
				slot = (int)cmd.Index;
				success = Game->GameData.SetPlayerName(slot, cmd.StrParam);
				if (Listener && success)
					Listener->ClientPlayerNameChanged(slot);
				break;

			case CMD_SetNumRounds:
				Game->GameData.MaxRounds = (int)cmd.Value;
				if (Listener)
					Listener->ClientGameOptionsChanged();
				break;

			case CMD_StartMatch:
				Game->GameData.Status = GAME_STARTING;
				if (Listener)
					Listener->ClientMatchStarting();
				Game->GameData.InitNewGame();
				Game->GameData.Status = GAME_RUNNING;
				if (Listener)
					Listener->ClientMatchStarted();
				break;

			case CMD_StartNextRound:
				Game->GameData.Status = GAME_STARTING;
				if (Listener)
					Listener->ClientMatchStarting();
				Game->GameData.InitNewRound();
				Game->GameData.Status = GAME_RUNNING;
				if (Listener)
					Listener->ClientRoundStarted();
				break;

			case CMD_EndRound:
				Game->GameData.Status = GAME_ENDED;
				if (Listener)
					Listener->ClientRoundEnded();
				break;
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

void TClient::UpdatePlayerMovement(int Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Slot < 0 || Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
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
}

void TClient::DropBomb(int Slot)
{
	if (Slot < 0 || Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
	if (p->State == PLAYER_NOTPLAYING)
		return;
	if (p->ActiveBombs == p->MaxActiveBombs)
		return;

	// determine the exact position where to drop it (top left pos of the field)
	TFieldPosition pos;
	pos.X = static_cast<int>(trunc(p->Position.X));
	pos.Y = static_cast<int>(trunc(p->Position.Y));

	// check if there's already a bomb at this position
	if (Game->GameData.BombInField(pos.X, pos.Y, false))
		return;

	// add the new bomb
	TField* field = Game->GameData.Arena.At(pos);
	field->Bomb.State = BOMB_TICKING;
	field->Bomb.DroppedByPlayer = Slot;
	field->Bomb.TimeUntilNextState = 2000; // 2 seconds
	field->Bomb.Range = p->BombRange;

	// update the player
	p->ActiveBombs++;
}

void TClient::SetNumRounds(int Value)
{
	if (Value < 1)
		Value = 1;
	else if (Value > MAX_NUM_ROUNDS)
		Value = MAX_NUM_ROUNDS;

	TClientCommand cmd;
	cmd.Command = CMD_SetNumRounds;
	cmd.Value = Value;
	Commands.push(cmd);
}

void TClient::SetPlayerName(int Slot, const std::string& Name)
{
	TClientCommand cmd;
	cmd.Command = CMD_SetPlayerName;
	cmd.Index = Slot;
	cmd.StrParam = Name;
	Commands.push(cmd);
}

void TClient::SetGameName(const std::string& SetName)
{
	TClientCommand cmd;
	cmd.Command = CMD_SetGameName;
	cmd.StrParam = SetName;
	Commands.push(cmd);
}