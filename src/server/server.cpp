#include "server.h"

#include <assert.h>



// --- TServer ---

TServer::TServer()
:	IServer(),
	ListenerThread(nullptr),
	Listener(),
	SocketSelector(),
	ClientSockets(),
	ClientSocketsMutex(),
	ThreadsShouldStop(false)
{
}

TServer::~TServer()
{
	Stop();
}

bool TServer::Start(unsigned int port)
{
	Stop();	// just in case

	Listener.listen(port);
	SocketSelector.add(Listener);

	ThreadsShouldStop = false;

	ListenerThread = new std::thread(std::bind(&TServer::ListenFunc, this));
	//ListenerThread->detach();

	return true;
}

void TServer::Stop()
{
	ThreadsShouldStop = true;
	if (ListenerThread)
	{
		ListenerThread->join();
		delete ListenerThread;
		ListenerThread = nullptr;
	}

	SocketSelector.clear();
	Listener.close();

	{
		std::lock_guard<std::mutex> g(ClientSocketsMutex);
		for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
		{
			sf::TcpSocket* client = *it;
			FinalizeConnection(client);
			delete client;
		}
		ClientSockets.clear();
	}
}

void TServer::ListenFunc()
{
	while (!ThreadsShouldStop)
	{
		// wait for a socket to receive something
		if (!SocketSelector.wait(sf::milliseconds(250)))
			continue;

		// check the listener for a new connection
		if (SocketSelector.isReady(Listener))
		{
			// The listener is ready: there is a pending connection
			sf::TcpSocket* newclient = new sf::TcpSocket;
			if (Listener.accept(*newclient) == sf::Socket::Done)
			{
				std::lock_guard<std::mutex> g(ClientSocketsMutex);
				ClientSockets.push_back(newclient);
				InitializeNewConnection(newclient);
			}
			else
				delete newclient;
		}

		// otherwise the client sockets
		else
		{
			std::lock_guard<std::mutex> g(ClientSocketsMutex);

			for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
			{
				sf::TcpSocket* client = *it;
				if (SocketSelector.isReady(*client))
				{
					sf::Packet packet;
					if (client->receive(packet) == sf::Socket::Done)
						ProcessPacketFromClient(packet);
				}
			}
		}
	}
}

void TServer::InitializeNewConnection(sf::TcpSocket* socket)
{
}

void TServer::FinalizeConnection(sf::TcpSocket* socket)
{
	socket->disconnect();
}

void TServer::ProcessPacketFromClient(sf::Packet& packet)
{
}