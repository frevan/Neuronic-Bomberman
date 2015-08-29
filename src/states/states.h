#pragma once

#include <string>

#include "../base/nel_interfaces.h"



const nel::TGameID SID_Splash		= std::hash<std::string>()(std::string("state:SplashScreen"));
const nel::TGameID SID_Menu			= std::hash<std::string>()(std::string("state:MenuScreen"));
const nel::TGameID SID_Options		= std::hash<std::string>()(std::string("state:OptionsScreen"));
const nel::TGameID SID_Lobby		= std::hash<std::string>()(std::string("state:GameLobby"));
const nel::TGameID SID_ServerSelect = std::hash<std::string>()(std::string("state:ServerSelect"));
const nel::TGameID SID_Game			= std::hash<std::string>()(std::string("state:GameRunning"));
// ...
