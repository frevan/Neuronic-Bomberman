#pragma once

#include <SFML/Network.hpp>

#include "base/nel_interfaces.h"
#include "base/utility/nel_macros.h"



DEFINE_GAMEID(IID_IServer, "game::IServer");
DEFINE_GAMEID(IID_IClient, "game::IClient");



struct TPlayer
{
	std::string Name;
	nel::TGameID Tag;
	nel::TGameID ClientTag;
	int Slot;
	int Score;
	int Kills;
};



class IServer : public nel::Interface
{
public:
	IServer(): nel::Interface() {};
	virtual ~IServer() {};

	virtual bool Start(unsigned int port) = 0;
	virtual void Stop() = 0;
};



class IServerSideGame : public nel::Interface
{
public:
	IServerSideGame() : nel::Interface() {};
	virtual ~IServerSideGame() {};

	virtual void SetNodeName(const std::string& SetName) = 0;
	virtual std::string GetNodeName() = 0;
	virtual bool GetPlayer(uint64_t Tag, TPlayer& Info) = 0;
	virtual uint64_t ConnectPlayer(const std::string& SetName, const std::string& SetVersion, uint64_t ClientTag) = 0;	// returns server tag
	virtual void DisconnectPlayer(uint64_t Tag) = 0;
};



class IClient :	public nel::Interface
{
public:
	IClient() : nel::Interface() {};
	virtual ~IClient() {};

	virtual void Process() = 0;

	virtual void Connect(const std::string& address, unsigned int port) = 0;
	virtual void Disconnect(const std::string& reason) = 0;
	virtual bool IsConnected() = 0;

	virtual void LeaveLobby() = 0;
	virtual bool IsInLobby() = 0;
};