#pragma once

#include "inputmap.h"

// action definitions (used with the input map)
const uint32_t actionToPreviousScreen = 1;	// go to the previous screen
const uint32_t actionDoDefaultAction = 2; // do whatever is the default action

// lobby actions
const uint32_t actionLobbyPrevMap = 200;
const uint32_t actionLobbyNextMap = 201;

// action definitions during a match
const uint32_t PlayerActionCount = 6;
// -- player 1
const uint32_t actionMatchPlayer1Left = 100 + PlayerActionCount * 0;
const uint32_t actionMatchPlayer1Right = 101 + PlayerActionCount * 0;
const uint32_t actionMatchPlayer1Up = 102 + PlayerActionCount * 0;
const uint32_t actionMatchPlayer1Down = 103 + PlayerActionCount * 0;
const uint32_t actionMatchPlayer1DropBomb = 104 + PlayerActionCount * 0;
const uint32_t actionMatchPlayer1Kick = 105 + PlayerActionCount * 0;
// -- player 2
const uint32_t actionMatchPlayer2Left = 100 + PlayerActionCount * 1;
const uint32_t actionMatchPlayer2Right = 101 + PlayerActionCount * 1;
const uint32_t actionMatchPlayer2Up = 102 + PlayerActionCount * 1;
const uint32_t actionMatchPlayer2Down = 103 + PlayerActionCount * 1;
const uint32_t actionMatchPlayer2DropBomb = 104 + PlayerActionCount * 1;
const uint32_t actionMatchPlayer2Kick = 105 + PlayerActionCount * 1;
// -- player 3
const uint32_t actionMatchPlayer3Left = 100 + PlayerActionCount * 2;
const uint32_t actionMatchPlayer3Right = 101 + PlayerActionCount * 2;
const uint32_t actionMatchPlayer3Up = 102 + PlayerActionCount * 2;
const uint32_t actionMatchPlayer3Down = 103 + PlayerActionCount * 2;
const uint32_t actionMatchPlayer3DropBomb = 104 + PlayerActionCount * 2;
const uint32_t actionMatchPlayer3Kick = 105 + PlayerActionCount * 2;
// -- player 4
const uint32_t actionMatchPlayer4Left = 100 + PlayerActionCount * 3;
const uint32_t actionMatchPlayer4Right = 101 + PlayerActionCount * 3;
const uint32_t actionMatchPlayer4Up = 102 + PlayerActionCount * 3;
const uint32_t actionMatchPlayer4Down = 103 + PlayerActionCount * 3;
const uint32_t actionMatchPlayer4DropBomb = 104 + PlayerActionCount * 3;
const uint32_t actionMatchPlayer4Kick = 105 + PlayerActionCount * 3;
// -- player 5
const uint32_t actionMatchPlayer5Left = 100 + PlayerActionCount * 4;
const uint32_t actionMatchPlayer5Right = 101 + PlayerActionCount * 4;
const uint32_t actionMatchPlayer5Up = 102 + PlayerActionCount * 4;
const uint32_t actionMatchPlayer5Down = 103 + PlayerActionCount * 4;
const uint32_t actionMatchPlayer5DropBomb = 104 + PlayerActionCount * 4;
const uint32_t actionMatchPlayer5Kick = 105 + PlayerActionCount * 4;
// -- player 6
const uint32_t actionMatchPlayer6Left = 100 + PlayerActionCount * 5;
const uint32_t actionMatchPlayer6Right = 101 + PlayerActionCount * 5;
const uint32_t actionMatchPlayer6Up = 102 + PlayerActionCount * 5;
const uint32_t actionMatchPlayer6Down = 103 + PlayerActionCount * 5;
const uint32_t actionMatchPlayer6DropBomb = 104 + PlayerActionCount * 5;
const uint32_t actionMatchPlayer6Kick = 105 + PlayerActionCount * 5;
// -- player 7
const uint32_t actionMatchPlayer7Left = 100 + PlayerActionCount * 6;
const uint32_t actionMatchPlayer7Right = 101 + PlayerActionCount * 6;
const uint32_t actionMatchPlayer7Up = 102 + PlayerActionCount * 6;
const uint32_t actionMatchPlayer7Down = 103 + PlayerActionCount * 6;
const uint32_t actionMatchPlayer7DropBomb = 104 + PlayerActionCount * 6;
const uint32_t actionMatchPlayer7Kick = 105 + PlayerActionCount * 6;
// -- player 8
const uint32_t actionMatchPlayer8Left = 100 + PlayerActionCount * 7;
const uint32_t actionMatchPlayer8Right = 101 + PlayerActionCount * 7;
const uint32_t actionMatchPlayer8Up = 102 + PlayerActionCount * 7;
const uint32_t actionMatchPlayer8Down = 103 + PlayerActionCount * 7;
const uint32_t actionMatchPlayer8DropBomb = 104 + PlayerActionCount * 7;
const uint32_t actionMatchPlayer8Kick = 105 + PlayerActionCount * 7;
// -- player 9
const uint32_t actionMatchPlayer9Left = 100 + PlayerActionCount * 8;
const uint32_t actionMatchPlayer9Right = 101 + PlayerActionCount * 8;
const uint32_t actionMatchPlayer9Up = 102 + PlayerActionCount * 8;
const uint32_t actionMatchPlayer9Down = 103 + PlayerActionCount * 8;
const uint32_t actionMatchPlayer9DropBomb = 104 + PlayerActionCount * 8;
const uint32_t actionMatchPlayer9Kick = 105 + PlayerActionCount * 8;
// -- player 10
const uint32_t actionMatchPlayer10Left = 100 + PlayerActionCount * 9;
const uint32_t actionMatchPlayer10Right = 101 + PlayerActionCount * 9;
const uint32_t actionMatchPlayer10Up = 102 + PlayerActionCount * 9;
const uint32_t actionMatchPlayer10Down = 103 + PlayerActionCount * 9;
const uint32_t actionMatchPlayer10DropBomb = 104 + PlayerActionCount * 9;
const uint32_t actionMatchPlayer10Kick = 105 + PlayerActionCount * 9;