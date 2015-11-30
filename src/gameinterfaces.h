#pragma once

#include <SFML/Network.hpp>

#include "base/nel_interfaces.h"
#include "base/utility/nel_macros.h"



DEFINE_GAMEID(IID_IServer, "game::IServer");

class IServer : public nel::Interface
{
public:
	IServer(): nel::Interface() {};
	virtual ~IServer() {};

	virtual bool Start(unsigned int port) = 0;
	virtual void Stop() = 0;
};



struct TPlayer
{
	std::string Name;
	sf::IpAddress ClientAddress;
	nel::TGameID ClientID;
	int Slot;
	int Score;
	int Kills;
};

