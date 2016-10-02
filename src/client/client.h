#pragma once

#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"

#include "../gameinterfaces.h"



class TClient :	public IClient,
				public nel::ILogic
{
private:
	sf::TcpSocket Socket;
	bool Connected;

	void ProcessPacketFromServer(sf::Packet& packet);
	void HandleServerDisconnect();

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
};
