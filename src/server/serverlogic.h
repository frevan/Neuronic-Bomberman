#pragma once

#include <list>
#include "../gameinterfaces.h"

class TServerSideGame: public IServerSideGame
{
private:
	std::list<TPlayer> Players;
public:
	TServerSideGame();
	~TServerSideGame();

	// from IServerSideGame
	virtual unsigned int GetPlayerCount();
	virtual bool GetPlayer(TPlayer& info);
	virtual int ConnectPlayer(const std::string& name, nel::TGameID ID);
	virtual void DisconnectPlayer(nel::TGameID ID);
};