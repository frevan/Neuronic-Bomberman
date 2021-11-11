#pragma once

#include <list>
#include <random>
#include "../gameinterfaces.h"

class TServerSideGame: public IServerSideGame
{
private:
	std::list<TPlayer> Players;
	std::mt19937_64 randomTagGenerator;
	int FindAvailableSlot();
public:
	TServerSideGame();
	~TServerSideGame();

	// from IServerSideGame
	virtual bool GetPlayer(uint64_t Tag, TPlayer& info);
	virtual uint64_t ConnectPlayer(const std::string& ClientName, const std::string& ClientVersion, uint64_t ClientTag);
	virtual void DisconnectPlayer(uint64_t Tag);
};