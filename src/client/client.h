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
	void Connect(const std::string& address, unsigned int port) override;
	void Disconnect() override;
	void Process() override;
	bool IsConnected() override;

	// from ILogic
	void Update(nel::TGameTime deltaTime) override;
};
