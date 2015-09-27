#pragma once

#include "base/nel_interfaces.h"
#include "base/utility/nel_macros.h"



DEFINE_GAMEID(IID_GameSession,	"IGameSession");

class IGameSession :	public nel::Interface
{
public:	
	virtual void SessionGotoState(nel::TGameID stateID) = 0;
};
