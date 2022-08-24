#pragma once

#include "inputmap.h"

// action definitions (used with the input map)
const uint32_t actionToPreviousScreen = 1;	// go to the previous screen
const uint32_t actionDoDefaultAction = 2; // do whatever is the default action

// action definitions during a match
const uint32_t actionMatchLeft = 100;
const uint32_t actionMatchRight = 101;
const uint32_t actionMatchUp = 102;
const uint32_t actionMatchDown = 103;