#include "client.h"

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	Commands()
{
}

void TClient::CreateGame(const std::string& LobbyName)
{
	Game->Server->Start();

	TClientCommand cmd;
	cmd.Command = CMD_OpenLobby;
	cmd.StrParam = LobbyName;
	Commands.push(cmd);
}

void TClient::CloseGame()
{
	Game->GameData.Reset();

	Game->Server->CloseLobby();
	if (Listener)
		Listener->ClientDisconnected();

	Game->Server->Stop();
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
				if (Game->Server->OpenLobby())
				{
					if (Listener)
						Listener->ClientConnected();
					Game->GameData.Reset();
					Game->GameData.GameName = cmd.StrParam;
					Game->GameData.Status = GAME_INLOBBY;
					if (Listener)
						Listener->ClientEnteredLobby();
				}
				break;

			case CMD_SetGameName:
				Game->Server->SetGameName(cmd.StrParam);
				Game->GameData.SetName(cmd.StrParam);
				if (Listener)
					Listener->ClientGameOptionsChanged();
				break;

			case CMD_AddPlayer:
				slot = (int)cmd.Index;
				slot = Game->Server->AddPlayer(cmd.StrParam, slot);
				success = (slot < MAX_NUM_SLOTS);
				if (success)
					Game->GameData.AddPlayer(cmd.StrParam, slot);
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
				if (Game->Server->RemovePlayer(slot))
				{
					Game->GameData.RemovePlayer(slot);
					if (Listener)
						Listener->ClientPlayerRemoved(slot);
				}
				break;

			case CMD_SetPlayerName:
				slot = (int)cmd.Index;
				if (Game->Server->SetPlayerName(slot, cmd.StrParam))
				{
					Game->GameData.SetPlayerName(slot, cmd.StrParam);
					if (Listener)
						Listener->ClientPlayerNameChanged(slot);
				}
				break;

			case CMD_SetNumRounds:
				cmd.Value = Game->Server->SetNumRounds((int)cmd.Value);
				Game->GameData.MaxRounds = (int)cmd.Value;
				if (Listener)
					Listener->ClientGameOptionsChanged();
				break;

			case CMD_StartMatch:
				if (Game->Server->StartMatch())
				{
					Game->GameData.Status = GAME_STARTING;
					if (Listener)
						Listener->ClientMatchStarting();
					Game->GameData.InitNewGame();
					Game->GameData.Status = GAME_RUNNING;
					if (Listener)
						Listener->ClientMatchStarted();
				}
				break;

			case CMD_StartNextRound:
				if (Game->Server->StartNextRound())
				{
					Game->GameData.Status = GAME_STARTING;
					if (Listener)
						Listener->ClientMatchStarting();
					Game->GameData.InitNewRound();
					Game->GameData.Status = GAME_RUNNING;
					if (Listener)
						Listener->ClientRoundStarted();
				}
				break;

			case CMD_EndRound:
				if (Game->Server->EndRound())
				{
					Game->GameData.Status = GAME_ENDED;
					if (Listener)
						Listener->ClientRoundEnded();
				}
				break;

			case CMD_SelectArena:
				TArena* map = Game->Maps.MapFromIndex(cmd.Index);
				if (map)
				{
					if (Game->Server->SelectArena(map->Caption))
						Game->GameData.Arena.LoadFromFile(map->OriginalFileName);
				}
				break;
		};
	}
}

void TClient::SelectArena(int Index)
{
	TClientCommand cmd;
	cmd.Command = CMD_SelectArena;
	cmd.Index = Index;
	Commands.push(cmd);
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