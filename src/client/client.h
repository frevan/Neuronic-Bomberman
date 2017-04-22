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

	void HandleSocketConnect();
	void HandleSocketDisconnect();

public:
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
	bool OnInfoRequest() override;
	bool OnConnect(int Version, const std::string& NodeName, const std::string& ClientVersion) override;
	bool OnRCon(const std::string& Password, const std::string& Command) override;
	bool OnInfoResponse(unsigned int Protocol, unsigned int Flags, const std::string& HostName) override;
	bool OnConnectResponse(unsigned int Protocol, unsigned int ServerID, unsigned int ClientID) override;
	bool OnPrint(unsigned int Type, const std::string& Text) override;
	bool OnError(unsigned int Code, const std::string& Reason) override;
	bool OnDisconnect(const std::string& Reason) override;
};
