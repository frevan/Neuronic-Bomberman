#pragma once



#include <list>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"



#define TClientSocket sf::TcpSocket



class IServerCommsCallback
{
};



class TServerComms
{
private:
	std::thread* ListenerThread;
	sf::TcpListener Listener;
	sf::SocketSelector SocketSelector;
	std::list<TClientSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;

	bool ThreadsShouldStop;

	void ListenFunc();
	void InitializeNewConnection(TClientSocket* socket);
	void FinalizeConnection(TClientSocket* socket);
	void ProcessPacketFromClient(sf::Packet& packet);

public:
	TServerComms();
	~TServerComms();

	bool Start(unsigned int port);
	void Stop();

	void Update(nel::TGameTime deltaTime);
};