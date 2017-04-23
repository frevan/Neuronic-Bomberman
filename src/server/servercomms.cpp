#include "servercomms.h"

#include <assert.h>



// --- TServerComms ---

TServerComms::TServerComms()
: 	ListenerThread(nullptr),
	Listener(),
	SocketSelector(),
	ClientSockets(),
	ClientSocketsMutex(),
	ThreadsShouldStop(false),
	Protocol()
{
}

TServerComms::~TServerComms()
{
	Stop();
}

bool TServerComms::Start(unsigned int port)
{
	Stop();	// just in case

	Listener.listen(port);
	SocketSelector.add(Listener);

	ThreadsShouldStop = false;

	ListenerThread = new std::thread(std::bind(&TServerComms::ListenFunc, this));

	return true;
}

void TServerComms::Stop()
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
			TClientSocket* client = *it;
			FinalizeConnection(client);
			delete client;
		}
		ClientSockets.clear();
	}
}

void TServerComms::ListenFunc()
{
	while (!ThreadsShouldStop)
	{
		// wait to receive something
		if (!SocketSelector.wait(sf::milliseconds(100)))
			continue;

		// check for a new connection
		if (SocketSelector.isReady(Listener))
		{
			TClientSocket* newclient = new TClientSocket;
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
				TClientSocket* client = *it;
				if (SocketSelector.isReady(*client))
				{
					sf::Packet packet;
					if (client->receive(packet) == sf::Socket::Done)
						Protocol.ProcessReceivedPacket(client, packet, this);
				}
			}
		}
	}
}

void TServerComms::InitializeNewConnection(TClientSocket* socket)
{
	// TODO: send "hello" packet
}

void TServerComms::FinalizeConnection(TClientSocket* socket)
{
	// TODO: send "goodbye" packet

	socket->disconnect();
}

void TServerComms::Update(nel::TGameTime deltaTime)
{
	std::lock_guard<std::mutex> g(ClientSocketsMutex);

	for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
	{
		TClientSocket* client = *it;
		if (client->getRemoteAddress() == sf::IpAddress::None)
		{
			// UNTESTED!
			FinalizeConnection(client);
			ClientSockets.remove(client);
			delete client;
		}
	}
}

bool TServerComms::OnInfoRequest(sf::Socket* Source)
{
	sf::Packet packet;
	Protocol.InfoResponse(packet, 0, "TestServer");
	Protocol.SendToSocket(Source, packet);

	return false;
}

bool TServerComms::OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion)
{
	return false;
}

bool TServerComms::OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command)
{
	return false;
}

bool TServerComms::OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName)
{
	return false;
}

bool TServerComms::OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int ServerID, unsigned int ClientID)
{
	return false;
}

bool TServerComms::OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text)
{
	return false;
}

bool TServerComms::OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason)
{
	return false;
}

bool TServerComms::OnDisconnect(sf::Socket* Source, const std::string& Reason)
{
	return false;
}