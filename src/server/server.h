#pragma once

#include <list>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

#include "../gameinterfaces.h"



class TServer :	public IServer
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

	bool Start(unsigned int port) override;
	void Stop() override;
};