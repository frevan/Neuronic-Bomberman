#pragma once

#include <string>

#include "../base/nel_interfaces.h"



const nel::TGameID SID_Splash		= std::hash<std::string>()(std::string("state:SplashScreen"));
const nel::TGameID SID_Menu			= std::hash<std::string>()(std::string("state:MenuScreen"));
const nel::TGameID SID_Options		= std::hash<std::string>()(std::string("state:OptionsScreen"));
const nel::TGameID SID_Session		= std::hash<std::string>()(std::string("state:GameSession"));
// ...

// substates of SID_Session
const nel::TGameID SID_Lobby		= std::hash<std::string>()(std::string("session:GameLobby"));
const nel::TGameID SID_ServerSelect	= std::hash<std::string>()(std::string("session:ServerSelect"));
const nel::TGameID SID_LoadMatch	= std::hash<std::string>()(std::string("session:LoadMatch"));
const nel::TGameID SID_Play			= std::hash<std::string>()(std::string("session:Play"));
const nel::TGameID SID_RoundEnd		= std::hash<std::string>()(std::string("session:EndOfRound"));
const nel::TGameID SID_MatchEnd		= std::hash<std::string>()(std::string("session:EndOfMatch"));
