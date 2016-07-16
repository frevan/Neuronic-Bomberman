#pragma once

#include <list>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"

#include "../gameinterfaces.h"



class TServer :	public IServer,
				public nel::ILogic
{
private:
	std::thread* ListenerThread;
	sf::TcpListener Listener;
	sf::SocketSelector SocketSelector;
	std::list<sf::TcpSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;

	bool ThreadsShouldStop;

	void ListenFunc();
	void InitializeNewConnection(sf::TcpSocket* socket);
	void FinalizeConnection(sf::TcpSocket* socket);
	void ProcessPacketFromClient(sf::Packet& packet);

public:
	TServer();
	~TServer() override;

	// from IServer
	bool Start(unsigned int port) override;
	void Stop() override;

	// from ILogic
	void Update(nel::TGameTime deltaTime) override;
};