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
	int slot;

	while (!Commands.empty())
	{
		cmd = Commands.front();
		Commands.pop();

		switch (cmd.Command)
		{
			case CMD_OpenLobby:	Game->Server->OpenLobby(); break;
			case CMD_SetGameName: Game->Server->SetGameName(cmd.StrParam); break;
			case CMD_AddPlayer: slot = Game->Server->AddPlayer(cmd.StrParam, (int)cmd.Index); break;
			case CMD_RemovePlayer: Game->Server->RemovePlayer((int)cmd.Index); break;
			case CMD_SetPlayerName: Game->Server->SetPlayerName((int)cmd.Index, cmd.StrParam); break;
			case CMD_SetNumRounds: cmd.Value = Game->Server->SetNumRounds((int)cmd.Value); break;
			case CMD_StartMatch: Game->Server->StartMatch(); break;
			case CMD_StartNextRound: Game->Server->StartRound(); break;
			case CMD_EndRound: Game->Server->EndRound(); break;
			case CMD_SelectArena:
				TArena* map = Game->Maps.MapFromIndex((int)cmd.Index);
				if (map)
					(Game->Server->SelectArena(map->Caption));
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

void TClient::ServerLobbyCreated()
{
	if (Listener)
		Listener->ClientConnected();
}

void TClient::ServerLobbyClosed()
{
}

void TClient::ServerEnteredLobby(const std::string& GameName)
{
	Game->GameData.Reset();
	Game->GameData.GameName = GameName;
	Game->GameData.Status = GAME_INLOBBY;
	if (Listener)
		Listener->ClientEnteredLobby();
}

void TClient::ServerLeftLobby()
{
}

void TClient::ServerGameNameChanged(const std::string& GameName)
{
	Game->GameData.SetName(GameName);
	if (Listener)
		Listener->ClientGameOptionsChanged();
}

void TClient::ServerArenaChanged(const std::string& ArenaName)
{
	TArena* map = Game->Maps.MapFromName(ArenaName);
	if (map)
		Game->GameData.Arena.LoadFromFile(map->OriginalFileName);
}

void TClient::ServerNumRoundsChanged(int NumRounds)
{
	Game->GameData.MaxRounds = NumRounds;
	if (Listener)
		Listener->ClientGameOptionsChanged();
}

void TClient::ServerMatchStarted()
{
	Game->GameData.InitNewGame();
	Game->GameData.Status = GAME_MATCHSTARTED;

	if (Listener)
		Listener->ClientMatchStarted();
}

void TClient::MatchEnded()
{
	//Game->GameData.Status = GAME_ENDED;
}

void TClient::ServerRoundStarting()
{
	Game->GameData.Status = GAME_ROUNDSTARTING;
	if (Listener)
		Listener->ClientMatchStarting();

	Game->GameData.InitNewRound();
}

void TClient::ServerRoundStarted()
{
	Game->GameData.Status = GAME_PLAYING;
	if (Listener)
		Listener->ClientRoundStarted();
}

void TClient::ServerRoundEnded()
{
	Game->GameData.Status = GAME_ENDED;
	if (Listener)
		Listener->ClientRoundEnded();
}

void TClient::ServerPlayerAdded(int Slot, const std::string& PlayerName)
{
	Game->GameData.AddPlayer(PlayerName, Slot);
	if (Listener)
		Listener->ClientPlayerAdded(Slot);
}

void TClient::ServerPlayerRemoved(int Slot)
{
	Game->GameData.RemovePlayer(Slot);
	if (Listener)
		Listener->ClientPlayerRemoved(Slot);
}

void TClient::ServerPlayerNameChanged(int Slot, const std::string& PlayerName)
{
	Game->GameData.SetPlayerName(Slot, PlayerName);
	if (Listener)
		Listener->ClientPlayerNameChanged(Slot);
}
