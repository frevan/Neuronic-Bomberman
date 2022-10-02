#include "client.h"

#include "comms.h"

const int CLIENT_IDLE = 0;
const int CLIENT_CONNECTING = 1;
const int CLIENT_CONNECTED = 2;

TClient::TClient(TGame* SetGame)
:	Game(SetGame),
	Listener(nullptr),
	Commands(),
	Socket(),
	ConnectionStatus(CLIENT_IDLE)
{
}

TClient::~TClient()
{
	DisconnectInternal();
}

void TClient::CreateGame(const std::string& LobbyName)
{
	TClientCommand cmd;
	cmd.Command = CMD_OpenLobby;
	cmd.StrParam = LobbyName;
	Commands.push(cmd);
}

void TClient::JoinGame()
{
	TClientCommand cmd;
	cmd.Command = CMD_JoinLobby;
	Commands.push(cmd);
}

void TClient::CloseGame()
{
	Disconnect();

	Game->GameData.Reset();

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

	if (ConnectionStatus == CLIENT_CONNECTING)
	{
		if (Socket.getRemoteAddress() != sf::IpAddress::None)
		{
			ConnectionStatus = CLIENT_CONNECTED;
			ConnectedToServer();
		}
	}

	if (ConnectionStatus == CLIENT_CONNECTED && Socket.getRemoteAddress() == sf::IpAddress::None)
	{
		ConnectionStatus = CLIENT_CONNECTED;
		DisconnectedFromServer();
	}

	if (ConnectionStatus != CLIENT_CONNECTED)
		return;

	while (!Commands.empty())
	{
		cmd = Commands.front();
		Commands.pop();

		sf::Packet packet;
		switch (cmd.Command)
		{
			case CMD_OpenLobby:	
				packet << SRV_CreateLobby << cmd.StrParam;
				Socket.send(packet);
				break;
			case CMD_JoinLobby:
				packet << SRV_JoinLobby;
				Socket.send(packet);
				break;
			case CMD_SetGameName: 
				packet << SRV_SetGameName << cmd.StrParam;
				Socket.send(packet);
				break;
			case CMD_AddPlayer: 
				packet << SRV_AddPlayer << (uint8_t)cmd.Index << cmd.StrParam;
				Socket.send(packet);
				break;
			case CMD_RemovePlayer:
				packet << SRV_RemovePlayer << (uint8_t)cmd.Index;
				Socket.send(packet);
				break;
			case CMD_SetPlayerName: 
				packet << SRV_SetPlayerName << (uint8_t)cmd.Index << cmd.StrParam;
				Socket.send(packet);
				break;
			case CMD_SetNumRounds:
				packet << SRV_SetNumRounds << (uint8_t)cmd.Value;
				Socket.send(packet);
				break;
			case CMD_StartMatch:
				packet << SRV_StartMatch;
				Socket.send(packet);
				break;
			case CMD_StartNextRound: 
				packet << SRV_StartNextRound;
				Socket.send(packet);
				break;
			case CMD_SelectArena:
				TArena* map = Game->Maps.MapFromIndex((int)cmd.Index);
				if (map)
				{
					packet << SRV_SetArena << map->Caption;
					Socket.send(packet);
				}
				break;
		};
	}

	sf::Packet packet;
	while (Socket.getRemoteAddress() != sf::IpAddress::None)
	{
		sf::Socket::Status r = Socket.receive(packet);
		if (r != sf::Socket::Done && r != sf::Socket::Partial)
			break;

		ProcessReceivedPacket(&Socket, packet);
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

	uint8_t direction = 0;
	if (Left)
		direction |= DIRECTION_LEFT;
	if (Right)
		direction |= DIRECTION_RIGHT;
	if (Up)
		direction |= DIRECTION_UP;
	if (Down)
		direction |= DIRECTION_DOWN;

	if (direction != Game->GameData.Players[Slot].Direction)
	{
		Game->GameData.Players[Slot].Direction = direction;

		sf::Packet packet;
		packet << SRV_UpdatePlayerMovement << Slot << direction;
		Socket.send(packet);
	}
}

void TClient::DropBomb(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	sf::Packet packet;
	packet << SRV_DropBomb << Slot;
	Socket.send(packet);
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

void TClient::ServerConnected()
{
	if (Listener)
		Listener->ClientConnected();
}

void TClient::ServerLobbyCreated()
{
	/*
	if (Listener)
		Listener->ClientConnected();
	*/
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

void TClient::ServerPlayerInfo(uint8_t Slot, uint8_t state, std::string Name)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->GameData.Players[Slot].State = state;
	Game->GameData.Players[Slot].Name = Name;

	if (Listener)
		Listener->ClientPlayerInfoChanged(Slot);
}

void TClient::ServerPlayerNameChanged(uint8_t Slot, const std::string& PlayerName)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	Game->GameData.SetPlayerName(Slot, PlayerName);
	if (Listener)
		Listener->ClientPlayerInfoChanged(Slot);
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

void TClient::ServerPlayerDroppedBomb(uint8_t Slot, const TFieldPosition& Position, uint16_t TimeUntilExplosion)
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
		field->Bomb.TimeUntilNextState = TimeUntilExplosion;
		field->Bomb.Range = p->BombRange;
	}
}

void TClient::ServerFullUpdate(TGameData* Data)
{
	Game->GameData.UpdateGameFrom(Data);
}

void TClient::ServerBombExploding(const TFieldPosition& Position, TGameTime TimeUntilExploded)
{
	TField* field;
	Game->GameData.Arena.At(Position, field);
	if (field)
	{
		uint8_t slot = field->Bomb.DroppedByPlayer;
		if (slot < MAX_NUM_SLOTS)
			Game->GameData.Players[slot].ActiveBombs--;

		field->Bomb.State = BOMB_EXPLODING;
		field->Bomb.TimeUntilNextState = TimeUntilExploded;
	}
}

void TClient::ServerBombExploded(const TFieldPosition& Position)
{
	TField* field;
	Game->GameData.Arena.At(Position, field);
	if (field)
		field->Bomb.State = BOMB_NONE;
}

void TClient::ServerPlayerDying(uint8_t Slot, uint16_t TimeUntilDeath)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
	p->State = PLAYER_DYING;
	p->TimeUntilNextState = TimeUntilDeath;
}

void TClient::ServerPlayerDied(uint8_t Slot)
{
	if (Slot >= MAX_NUM_SLOTS || Slot == INVALID_SLOT)
		return;

	TPlayer* p = &(Game->GameData.Players[Slot]);
	p->State = PLAYER_DEAD;
}

void TClient::Connect(const std::string& ServerAddress, unsigned int ServerPort)
{
	DisconnectInternal(); // just in case

	ConnectionStatus = CLIENT_CONNECTING;

	Socket.setBlocking(false);
	Socket.connect(ServerAddress, ServerPort);
}

void TClient::Disconnect()
{
	DisconnectInternal();
	DisconnectedFromServer();
}

void TClient::DisconnectInternal() 
{
	Socket.disconnect();
	ConnectionStatus = CLIENT_IDLE;
}

void TClient::ProcessReceivedPacket(sf::Socket* Source, sf::Packet& Packet)
{
	uint16_t cmd;

	if (!(Packet >> cmd))
		return;

	uint8_t u8_1, u8_2, u8_3;
	uint16_t u16_1;
	float f_1, f_2;
	std::string s_1;

	if (cmd == CLN_CommandFailed)
	{
		TCommand failedCmd;
		if (Packet >> failedCmd)
		{
			switch (failedCmd)
			{
				case SRV_Connect:
					Disconnect();
					break;
			};
		}
		return;
	}
	
	switch (cmd)
	{
		case CLN_Connected:
			ServerConnected();
			break;
		case CLN_Disconnected:
			DisconnectInternal();
			DisconnectedFromServer();
			break;

		case CLN_LobbyCreated:
			ServerLobbyCreated();
			break;
		case CLN_LobbyClosed:
			ServerLobbyClosed();
			break;
		case CLN_EnteredLobby:
			if (Packet >> s_1)
				ServerEnteredLobby(s_1);
			break;

		case CLN_PlayerAdded:
			if (Packet >> u8_1 >> s_1)
				ServerPlayerAdded(u8_1, s_1);
			break;
		case CLN_PlayerRemoved: 
			if (Packet >> u8_1)
				ServerPlayerRemoved(u8_1);
			break;
		case CLN_PlayerIsReady: 
			break;
		case CLN_PlayerNameChanged: 
			if (Packet >> u8_1 >> s_1)
				ServerPlayerNameChanged(u8_1, s_1);
			break;
		
		case CLN_GameNameChanged:
			if (Packet >> s_1)
				ServerGameNameChanged(s_1);
			break;
		case CLN_NumRoundsChanged: 
			if (Packet >> u8_1)
				ServerNumRoundsChanged(u8_1);
			break;
		case CLN_ArenaChanged: 
			if (Packet >> s_1)
				ServerArenaChanged(s_1);
			break;
		case CLN_ArenaList: 
			break;
		
		case CLN_MatchStarted:
			if (Packet >> u8_1)
			{
				ServerNumRoundsChanged(u8_1);
				ServerMatchStarted();
			}
			break;
		case CLN_RoundStarting:
			ServerRoundStarting();
			break;
		case CLN_RoundStarted:
			ServerRoundStarted();
			break;
		case CLN_RoundEnded: 
			ServerRoundEnded();
			break;

		case CLN_PlayerMovement:
			if (Packet >> u8_1 >> u8_2 >> f_1 >> f_2)
			{
				ServerPlayerDirectionChanged(u8_1, u8_2 & DIRECTION_LEFT, u8_2 & DIRECTION_RIGHT, u8_2 & DIRECTION_UP, u8_2 & DIRECTION_DOWN);
				ServerPlayerPositionChanged(u8_1, f_1, f_2);
			}
			break;

		case CLN_BombDropped: 
			if (Packet >> u8_1 >> u8_2 >> u8_3 >> u16_1)
			{
				TFieldPosition p{u8_2, u8_3};
				ServerPlayerDroppedBomb(u8_1, p, u16_1);
			}
			break;
		case CLN_BombExploding:
			if (Packet >> u8_1 >> u8_2 >> u16_1)
			{
				TFieldPosition position{u8_1, u8_2};
				ServerBombExploding(position, u16_1);
			}
			break;
		case CLN_BombExploded:
			if (Packet >> u8_1 >> u8_2)
			{
				TFieldPosition position{u8_1, u8_2};
				ServerBombExploded(position);
			}
			break;
		case CLN_PlayerDying:
			if (Packet >> u8_1 >> u16_1)
				ServerPlayerDying(u8_1, u16_1);
			break;
		case CLN_PlayerDied: 
			if (Packet >> u8_1)
				ServerPlayerDied(u8_1);
			break;
		case CLN_PlayerScore:
			if (Packet >> u8_1 >> u8_2)
				ServerPlayerScore(u8_1, u8_2);
			break;

		case CLN_ArenaInfo:
			if (Packet >> u8_1 >> u8_2)
				ServerArenaInfo(u8_1, u8_2, Packet);
			break;
		case CLN_PlayerInfo:
			if (Packet >> u8_1 >> u8_2 >> s_1)
				ServerPlayerInfo(u8_1, u8_2, s_1);
			break;
	};
}

void TClient::ConnectedToServer()
{
	sf::Packet packet;
	packet << SRV_Connect << SERVER_VERSION;
	Socket.send(packet);
}

void TClient::DisconnectedFromServer()
{
	if (Listener)
		Listener->ClientDisconnected();
}

void TClient::ServerPlayerPositionChanged(uint8_t Slot, float X, float Y)
{
	if (Slot >= 0 && Slot < MAX_NUM_SLOTS)
	{
		Game->GameData.Players[Slot].Position.X = X;
		Game->GameData.Players[Slot].Position.Y = Y;
	}
}

void TClient::ServerArenaInfo(uint8_t Width, uint8_t Height, sf::Packet& Packet)
{
	if (Width < 0 || Height < 0)
		return;

	Game->GameData.Arena.SetSize(Width, Height);

	uint8_t type;
	TField* field = nullptr;
	for (uint8_t y = 0; y < Height; y++)
		for (uint8_t x = 0; x < Width; x++)
		{
			if (!(Packet >> type))
				break;

			Game->GameData.Arena.At(x, y, field);
			field->Type = type;
		}
}

void TClient::ServerPlayerScore(uint8_t Slot, uint8_t Score)
{
	if (Slot >= MAX_NUM_SLOTS)
		return;

	Game->GameData.Players[Slot].RoundsWon = Score;

	if (Listener)
		Listener->ClientPlayerInfoChanged(Slot);
}