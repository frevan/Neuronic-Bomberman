#pragma once

#include <SFML/Network.hpp>



class IBombermanProtocolReceiver
{
public:
	virtual bool OnInfoRequest() = 0;
	virtual bool OnConnect(int Version, const std::string& NodeName, const std::string& ClientVersion) = 0;
	virtual bool OnRCon(const std::string& Password, const std::string& Command) = 0;
	virtual bool OnInfoResponse(unsigned int Protocol, unsigned int Flags, const std::string& HostName) = 0;
	virtual bool OnConnectResponse(unsigned int Protocol, unsigned int ServerID, unsigned int ClientID) = 0;
	virtual bool OnPrint(unsigned int Type, const std::string& Text) = 0;
	virtual bool OnError(unsigned int Code, const std::string& Reason) = 0;
	virtual bool OnDisconnect(const std::string& Reason) = 0;
};



class TBombermanProtocol
{
private:
	void InitPacket(sf::Packet& Packet, uint16_t ID);

public:
	bool ProcessReceivedPacket(sf::Packet& Packet, IBombermanProtocolReceiver* Receiver);

	// connectionless - client
	void InfoRequest(sf::Packet& Packet);
	void Connect(sf::Packet& Packet, const std::string& NodeName, const std::string& ClientVersion);
	void RCon(sf::Packet& Packet, const std::string& Password, const std::string& Command);
	// connectionless - server
	void InfoResponse(sf::Packet& Packet, unsigned int Protocol, unsigned int Flags, const std::string& HostName);
	void ConnectResponse(sf::Packet& Packet, unsigned int Protocol, unsigned int ServerID, unsigned int ClientID);
	void Print(sf::Packet& Packet, unsigned int Type, const std::string& Text);
	void Error(sf::Packet& Packet, unsigned int Code, const std::string& Reason);
	void Disconnect(sf::Packet& Packet, const std::string& Reason);
};

