#pragma once

#include <SFML/Network.hpp>



const unsigned int Err_Unknown		= 0;
const unsigned int Err_ServerFull	= 1;

const unsigned int SF_PasswordProtected	= 1 << 0;



class IBombermanProtocolReceiver
{
public:
	virtual bool OnInfoRequest(sf::Socket* Source) = 0;
	virtual bool OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion, uint64_t ClientTag) = 0;
	virtual bool OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command) = 0;
	virtual bool OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName) = 0;
	virtual bool OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, uint64_t ServerTag) = 0;
	virtual bool OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text) = 0;
	virtual bool OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason) = 0;
	virtual bool OnDisconnect(sf::Socket* Source, const std::string& Reason) = 0;
};



class TBombermanProtocol
{
private:
	void InitPacket(sf::Packet& Packet, uint16_t ID);

public:
	bool ProcessReceivedPacket(sf::Socket* Source, sf::Packet& Packet, IBombermanProtocolReceiver* Receiver);
	bool SendToSocket(sf::Socket* Socket, sf::Packet& Packet);

	// create packet functions
	// - connectionless - client
	void InfoRequest(sf::Packet& Packet);
	void Connect(sf::Packet& Packet, const std::string& NodeName, const std::string& ClientVersion, uint64_t ClientTag);
	void RCon(sf::Packet& Packet, const std::string& Password, const std::string& Command);
	void CreateLobby(sf::Packet& Packet, uint64_t ServerTag);
	// - connectionless - server
	void InfoResponse(sf::Packet& Packet, unsigned int Flags, const std::string& HostName);
	void ConnectResponse(sf::Packet& Packet, unsigned int Protocol, uint64_t ServerTag);
	void Print(sf::Packet& Packet, unsigned int Type, const std::string& Text);
	void Error(sf::Packet& Packet, unsigned int Code, const std::string& Reason);
	void Disconnect(sf::Packet& Packet, const std::string& Reason);
};

