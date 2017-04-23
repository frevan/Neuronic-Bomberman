#pragma once

#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"

#include "../gameinterfaces.h"
#include "../util/networkprotocol.h"



class TClient :	public IClient,
				public nel::ILogic,
				public IBombermanProtocolReceiver
{
private:
	sf::TcpSocket Socket;
	bool Connected, SocketConnected;
	TBombermanProtocol Protocol;

	uint8_t ServerProtocol;
	unsigned int ServerFlags;
	std::string ServerName;
	uint16_t ServerID;
	uint16_t ClientID;

	void HandleSocketConnect();
	void HandleSocketDisconnect();

public:
	nel::IApplication* Application;

	TClient();
	~TClient() override;

	// from IClient
	void Process() override;
	void Connect(const std::string& address, unsigned int port) override;
	void Disconnect() override;	
	bool IsConnected() override;
	void LeaveLobby() override;
	bool IsInLobby() override;

	// from ILogic
	void Update(nel::TGameTime deltaTime) override;

	// from IBombermanProtocolReceiver
	bool OnInfoRequest(sf::Socket* Source) override;
	bool OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion) override;
	bool OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command) override;
	bool OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName) override;
	bool OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int ServerID, unsigned int ClientID) override;
	bool OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text) override;
	bool OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason) override;
	bool OnDisconnect(sf::Socket* Source, const std::string& Reason) override;
};
