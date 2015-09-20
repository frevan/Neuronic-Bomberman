#pragma once

#include <string>

#include "../base/nel_interfaces.h"
#include "../base/utility/nel_macros.h"



DEFINE_GAMEID(SID_Splash,	"state:SplashScreen");
DEFINE_GAMEID(SID_Menu,		"state:MenuScreen");
DEFINE_GAMEID(SID_Options,	"state:OptionsScreen");
DEFINE_GAMEID(SID_Session,	"state:GameSession");
// ...

// substates of SID_Session
DEFINE_GAMEID(SID_Lobby,		"session:GameLobby");
DEFINE_GAMEID(SID_ServerSelect,	"session:ServerSelect");
DEFINE_GAMEID(SID_LoadMatch,	"session:LoadMatch");
DEFINE_GAMEID(SID_Play,			"session:Play");
DEFINE_GAMEID(SID_RoundEnd,		"session:EndOfRound");
DEFINE_GAMEID(SID_MatchEnd,		"session:EndOfMatch");
