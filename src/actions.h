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
const uint32_t actionMatchPlayer1Left = 100;
const uint32_t actionMatchPlayer1Right = 101;
const uint32_t actionMatchPlayer1Up = 102;
const uint32_t actionMatchPlayer1Down = 103;
const uint32_t actionMatchPlayer1DropBomb = 104;
const uint32_t actionMatchPlayer1Kick = 105;
// -- player 2
const uint32_t actionMatchPlayer2Left = 106;
const uint32_t actionMatchPlayer2Right = 107;
const uint32_t actionMatchPlayer2Up = 108;
const uint32_t actionMatchPlayer2Down = 109;
const uint32_t actionMatchPlayer2DropBomb = 110;
const uint32_t actionMatchPlayer2Kick = 111;