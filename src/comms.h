#pragma once

#include <cstdint>

typedef uint16_t TCommand;
typedef uint32_t TProtocolVersion;

const int SERVER_PORT = 45091;
const TProtocolVersion SERVER_VERSION = 1;

// server commands (sent by client)
// - connection
const TCommand SRV_Connect = 0; // TProtocolVersion Version
const TCommand SRV_CreateGame = 1; // string GameName
const TCommand SRV_JoinGame = 2;
const TCommand SRV_EndGame = 3;
const TCommand SRV_LeaveGame = 4;
// - players
const TCommand SRV_AddPlayer = 100; // uint8_t Slot | string PlayerName
const TCommand SRV_RemovePlayer = 101;// uint8_t Slot
const TCommand SRV_SetPlayerReady = 102; // uint8_t Slot | uint8_t Ready (0=false, 1=true)
const TCommand SRV_SetPlayerName = 103; // uint8_t Slot | string PlayerName
// - match properties
const TCommand SRV_SetGameName = 200; // string GameName

const TCommand SRV_SetNumRounds = 201; // uint8_t NumRounds
const TCommand SRV_SetArena = 202; // string ArenaName
// - match
const TCommand SRV_StartMatch = 300;
const TCommand SRV_StartNextRound = 301;
const TCommand SRV_UpdatePlayerMovement = 302; // uint8_t Slot | uint8_t Direction (bitfield, see DIR_ constants))
const TCommand SRV_DropBomb = 303; // uint8_t Slot

// client commands (sent by server)
// - connection responses
const TCommand CLN_CommandFailed = 1000; // TCommand Command
const TCommand CLN_Connected = 1001;
const TCommand CLN_Disconnected = 1002;
const TCommand CLN_LobbyCreated = 1003;
const TCommand CLN_LobbyClosed = 1004;
const TCommand CLN_EnteredLobby = 1005; // string GameName
// - player notifications
const TCommand CLN_PlayerAdded = 1100; // uint8_t Slot | string PlayerName
const TCommand CLN_PlayerRemoved = 1101; // uint8_t Slot
const TCommand CLN_PlayerIsReady = 1102;
const TCommand CLN_PlayerNameChanged = 1103; // uint8_t Slot | string PlayerName
// - match property notifications
const TCommand CLN_GameNameChanged = 1200;
const TCommand CLN_NumRoundsChanged = 1201;
const TCommand CLN_ArenaChanged = 1202; // uint16_t Index | string Name
const TCommand CLN_ArenaList = 1203; // uint16_t Count | uint16_t Index | string ArenaName
// - match notifications
const TCommand CLN_MatchStarted = 1300; // uint8_t NumRounds
const TCommand CLN_RoundStarting = 1301;
const TCommand CLN_RoundStarted = 1302;
const TCommand CLN_RoundEnded = 1303;
const TCommand CLN_PlayerMovement = 1304; // uint8_t Slot | uint8_t Direction | float X | float Y
const TCommand CLN_BombDropped = 1305; // uint8_t Slot | uint8_t FieldX | uint8_t FieldY | uint16_t TimeUntilExplosion (milliseconds)
const TCommand CLN_BombExploding = 1306; // uint8_t FieldX | uint8_t FieldY | uint16_t TimeUntilExploded (milliseconds)
const TCommand CLN_BombExploded = 1307; // uint8_t FieldX | uint8_t FieldY
const TCommand CLN_PlayerDying = 1308; // uint8_t Slot | uint16_t TimeUntilDeath (milliseconds)
const TCommand CLN_PlayerDied = 1309; // uint8_t Slot
const TCommand CLN_ArenaInfo = 1310; // uint8_t Widdth | uint8_t Height | Field Type (x=0,y=0), Field Type (x=1,y=0) .. Fieldt Type (x=width-1,y=height-1)
const TCommand CLN_PlayerScore = 1311; // uint8_t Slot | uint8_t Score
const TCommand CLN_PlayerInfo = 1312; // uint8_t Slot | uint8_t State | string Name