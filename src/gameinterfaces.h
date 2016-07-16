#pragma once

#include <SFML/Network.hpp>

#include "base/nel_interfaces.h"
#include "base/utility/nel_macros.h"



DEFINE_GAMEID(IID_IServer, "game::IServer");
DEFINE_GAMEID(IID_IClient, "game::IClient");



class IServer : public nel::Interface
{
public:
	IServer(): nel::Interface() {};
	virtual ~IServer() {};

	virtual bool Start(unsigned int port) = 0;
	virtual void Stop() = 0;
};



class IClient :	public nel::Interface
{
public:
	IClient() : nel::Interface() {};
	virtual ~IClient() {};

	virtual void Connect(const std::string& address, unsigned int port) = 0;
	virtual void Disconnect() = 0;
	virtual void Process() = 0;
	virtual bool IsConnected() = 0;
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

