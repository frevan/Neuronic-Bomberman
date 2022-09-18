#include "client.h"

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	Commands(),
	Socket()
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

	while (!Commands.empty())
	{
		cmd = Commands.front();
		Commands.pop();

		switch (cmd.Command)
		{
			case CMD_OpenLobby:	Game->Server->OpenLobby(); break;
			case CMD_SetGameName: Game->Server->SetGameName(cmd.StrParam); break;
			case CMD_AddPlayer: Game->Server->AddPlayer(cmd.StrParam, (int)cmd.Index); break;
			case CMD_RemovePlayer: Game->Server->RemovePlayer((int)cmd.Index); break;
			case CMD_SetPlayerName: Game->Server->SetPlayerName((int)cmd.Index, cmd.StrParam); break;
			case CMD_SetNumRounds: Game->Server->SetNumRounds((int)cmd.Value); break;
			case CMD_StartMatch: Game->Server->StartMatch(); break;
			case CMD_StartNextRound: Game->Server->StartRound(); break;
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

void TClient::UpdatePlayerMovement(uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Slot < 0 || Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->Server->SetPlayerDirections(Slot, Left, Right, Up, Down);
}

void TClient::DropBomb(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->Server->DropBomb(Slot);
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

void TClient::SetPlayerName(uint8_t Slot, const std::string& Name)
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

void TClient::ServerMatchEnded()
{
	Game->GameData.Status = GAME_MATCHENDED;
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
	Game->GameData.Status = GAME_ROUNDENDED;
	if (Listener)
		Listener->ClientRoundEnded();
}

void TClient::ServerPlayerAdded(uint8_t Slot, const std::string& PlayerName)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->GameData.AddPlayer(PlayerName, Slot);
	if (Listener)
		Listener->ClientPlayerAdded(Slot);
}

void TClient::ServerPlayerRemoved(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->GameData.RemovePlayer(Slot);
	if (Listener)
		Listener->ClientPlayerRemoved(Slot);
}

void TClient::ServerPlayerNameChanged(uint8_t Slot, const std::string& PlayerName)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->GameData.SetPlayerName(Slot, PlayerName);
	if (Listener)
		Listener->ClientPlayerNameChanged(Slot);
}

void TClient::ServerPlayerDirectionChanged(uint8_t Slot, bool Left, bool Right, bool Up, bool Down)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
	p->Direction = DIRECTION_NONE;
	if (Left)
		p->Direction |= DIRECTION_LEFT;
	if (Right)
		p->Direction |= DIRECTION_RIGHT;
	if (Up)
		p->Direction |= DIRECTION_UP;
	if (Down)
		p->Direction |= DIRECTION_DOWN;
}

void TClient::ServerPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
	p->ActiveBombs++;

	TField* field{};
	Game->GameData.Arena.At(Position, field);
	if (field)
	{
		field->Bomb.State = BOMB_TICKING;
		field->Bomb.DroppedByPlayer = Slot;
		field->Bomb.TimeUntilNextState = 2000; // 2 seconds
		field->Bomb.Range = p->BombRange;
	}
}

void TClient::ServerFullUpdate(TGameData* Data)
{
	Game->GameData.UpdateGameFrom(Data);
}

void TClient::Connect(const std::string& ServerAddress, unsigned int ServerPort)
{
	Disconnect(); // just in case

	Socket.setBlocking(false);
	Socket.connect(ServerAddress, ServerPort);
}

void TClient::Disconnect()
{
	Socket.disconnect();
}