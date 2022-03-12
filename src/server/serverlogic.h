#pragma once

#include <list>
#include <random>
#include "../gameinterfaces.h"

class TServerSideGame: public IServerSideGame
{
private:
	std::string NodeName;
	std::list<TPlayer> Players;
	std::mt19937_64 randomTagGenerator;
	int FindAvailableSlot();
public:
	TServerSideGame();
	~TServerSideGame();

	// from IServerSideGame
	void SetNodeName(const std::string& SetName) override;
	std::string GetNodeName() override;
	bool GetPlayer(uint64_t Tag, TPlayer& info) override;
	uint64_t ConnectPlayer(const std::string& ClientName, const std::string& ClientVersion, uint64_t ClientTag) override;
	void DisconnectPlayer(uint64_t Tag) override;
};