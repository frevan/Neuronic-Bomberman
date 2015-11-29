#pragma once

#include <string>

#include "../base/nel_interfaces.h"
#include "../base/utility/nel_macros.h"



DEFINE_GAMEID(SID_Splash,			"state:SplashScreen");
DEFINE_GAMEID(SID_Menu,				"state:MenuScreen");
DEFINE_GAMEID(SID_Options,			"state:OptionsScreen");
DEFINE_GAMEID(SID_ServerSelect,		"state:ServerSelect");
DEFINE_GAMEID(SID_ServerConnect,	"state::ServerConnect");
DEFINE_GAMEID(SID_Lobby,			"state:GameLobby");
DEFINE_GAMEID(SID_LoadMatch,		"state:LoadMatch");
DEFINE_GAMEID(SID_Play,				"state:Play");
DEFINE_GAMEID(SID_RoundEnd,			"state:EndOfRound");
DEFINE_GAMEID(SID_MatchEnd,			"state:EndOfMatch");
