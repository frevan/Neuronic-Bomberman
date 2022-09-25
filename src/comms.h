#pragma once

#include <cstdint>

const uint32_t SERVER_VERSION = 1;

// server commands (sent by client)
// - connection
const uint16_t SRV_Connect = 0; // uint32_t Version
const uint16_t SRV_CreateLobby = 1; // string GameName
const uint16_t SRV_CloseLobby = 2;
// - players
const uint16_t SRV_AddPlayer = 100; // uint8_t Slot | string PlayerName
const uint16_t SRV_RemovePlayer = 101;// uint8_t Slot
const uint16_t SRV_SetPlayerReady = 102; // uint8_t Slot | uint8_t Ready (0=false, 1=true)
const uint16_t SRV_SetPlayerName = 103; // uint8_t Slot | string PlayerName
// - match properties
const uint16_t SRV_SetGameName = 200; // string GameName

const uint16_t SRV_SetNumRounds = 201; // uint8_t NumRounds
const uint16_t SRV_SetArena = 202; // string ArenaName
// - match
const uint16_t SRV_StartMatch = 300;
const uint16_t SRV_StartNextRound = 301;
const uint16_t SRV_UpdatePlayerMovement = 302; // uint8_t Slot | uint8_t Direction (bitfield, see DIR_ constants))
const uint16_t SRV_DropBomb = 303; // uint8_t Slot

// client commands (sent by server)
// - connection responses
const uint16_t CLN_CommandFailed = 1000; // uint16_t Command
const uint16_t CLN_Connected = 1001;
const uint16_t CLN_Disconnected = 1002;
const uint16_t CLN_LobbyCreated = 1003;
const uint16_t CLN_LobbyClosed = 1004;
const uint16_t CLN_EnteredLobby = 1005; // string GameName
// - player notifications
const uint16_t CLN_PlayerAdded = 1100; // uint8_t Slot | string PlayerName
const uint16_t CLN_PlayerRemoved = 1101; // uint8_t Slot
const uint16_t CLN_PlayerIsReady = 1102;
const uint16_t CLN_PlayerNameChanged = 1103; // uint8_t Slot | string PlayerName
// - match property notifications
const uint16_t CLN_GameNameChanged = 1200;
const uint16_t CLN_NumRoundsChanged = 1201;
const uint16_t CLN_ArenaChanged = 1202;
const uint16_t CLN_ArenaList = 1203;
// - match notifications
const uint16_t CLN_MatchStarted = 1300; // uint8_t NumRounds
const uint16_t CLN_RoundStarting = 1301;
const uint16_t CLN_RoundStarted = 1302;
const uint16_t CLN_RoundEnded = 1303;
const uint16_t CLN_PlayerMovement = 1304; // uint8_t Slot | uint8_t Direction | float X | float Y
const uint16_t CLN_BombDropped = 1305; // uint8_t Slot | uint8_t FieldX | uint8_t FieldY | uint16_t TimeUntilExplosion (milliseconds)
const uint16_t CLN_BombExploding = 1306; // uint8_t FieldX | uint8_t FieldY | uint16_t TimeUntilExploded (milliseconds)
const uint16_t CLN_BombExploded = 1307; // uint8_t FieldX | uint8_t FieldY
const uint16_t CLN_PlayerDying = 1308; // uint8_t Slot | uint16_t TimeUntilDeath (milliseconds)
const uint16_t CLN_PlayerDied = 1309; // uint8_t Slot
const uint16_t CLN_ArenaInfo = 1310; // uint8_t Widdth | uint8_t Height | Field Type (x=0,y=0), Field Type (x=1,y=0) .. Fieldt Type (x=width-1,y=height-1)
