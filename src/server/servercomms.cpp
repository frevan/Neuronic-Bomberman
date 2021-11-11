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

bool TServerComms::Start(unsigned int port, IServerSideGame* game)
{
	Stop();	// just in case

	Game = game;

	Listener.listen(port);
	SocketSelector.add(Listener);

	ThreadsShouldStop = false;

	ListenerThread = new std::thread(std::bind(&TServerComms::ListenFunc, this));

	return true;
}

void TServerComms::Stop()
{
	Game = nullptr;

	ThreadsShouldStop = true;
	if (ListenerThread)
	{
		ListenerThread->join();
		delete ListenerThread;
		ListenerThread = nullptr;
	}

	SocketSelector.clear();
	Listener.close();


	std::list<TClientSocket*> removedSockets;
	{
		std::lock_guard<std::mutex> g(ClientSocketsMutex);
		for (auto it = ClientSockets.begin(); it != ClientSockets.end(); )
		{
			TClientSocket* client = *it++;	// increment here because otherwise there will be a runtime error when it's incremented in the for loop
			RemoveSocketFromList(client, removedSockets);
		}
		ClientSockets.clear();
	}
	DeleteRemovedSockets(removedSockets);
}

void TServerComms::ListenFunc()
{
	while (!ThreadsShouldStop)
	{
		// wait to receive something
		if (!SocketSelector.wait(sf::milliseconds(50)))
			continue;

		if (ThreadsShouldStop)
			break;

		// check for a new connection
		if (SocketSelector.isReady(Listener))
		{
			TClientSocket* newclient = new TClientSocket;
			if (Listener.accept(*newclient) == sf::Socket::Done)
			{
				newclient->ID = reinterpret_cast<std::uintptr_t>(newclient);
				{					
					std::lock_guard<std::mutex> g(ClientSocketsMutex);
					SocketSelector.add(*newclient);
					ClientSockets.push_back(newclient);
				}
				InitializeNewConnection(newclient);				
			}
			else
				delete newclient;
		};

		// check the connected clients
		{
			std::lock_guard<std::mutex> g(ClientSocketsMutex);
			for (auto it = ClientSockets.begin(); it != ClientSockets.end(); it++)
			{
				TClientSocket* client = *it;
				if (SocketSelector.isReady(*client))
				{
					sf::Packet packet;
					if (client->receive(packet) == sf::Socket::Done)
						Protocol.ProcessReceivedPacket(client, packet, this);	// should try to do this outside of the mutex?
				}
			}
		}
	}
}

void TServerComms::InitializeNewConnection(TClientSocket* socket)
{
	// TODO: send "hello" packet?
}

void TServerComms::FinalizeConnection(TClientSocket* socket)
{
	// TODO: send "goodbye" packet

	socket->disconnect();
}

void TServerComms::Update(nel::TGameTime deltaTime)
{
	// look for sockets to remove
	std::list<TClientSocket*> removedSockets;
	{
		std::lock_guard<std::mutex> g(ClientSocketsMutex);
		for (auto it = ClientSockets.begin(); it != ClientSockets.end(); )
		{
			TClientSocket* client = *it++;	// increment here because otherwise there will be a runtime error when it's incremented in the for loop
			if (client->getRemoteAddress() == sf::IpAddress::None)
				RemoveSocketFromList(client, removedSockets);
		}
	}
	DeleteRemovedSockets(removedSockets);
}

bool TServerComms::OnInfoRequest(sf::Socket* Source)
{
	sf::Packet packet;
	Protocol.InfoResponse(packet, 0, "TestServer");
	Protocol.SendToSocket(Source, packet);

	return false;
}

bool TServerComms::OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion, uint64_t ClientTag)
{
	nel::TGameID tag = nel::INVALID_GAME_ID;

	if (Game)
		tag = Game->ConnectPlayer(NodeName, ClientVersion, ClientTag);

	sf::Packet packet;
	if (tag != nel::INVALID_GAME_ID)
		Protocol.ConnectResponse(packet, 0, tag);
	else
		Protocol.Error(packet, Err_ServerFull, "The server appears to be full.");
	Protocol.SendToSocket(Source, packet);

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

bool TServerComms::OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, uint64_t ServerID)
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

void TServerComms::RemoveSocketFromList(TClientSocket* socket, std::list<TClientSocket*>& removedSockets)
{
	SocketSelector.remove(*socket);
	ClientSockets.remove(socket);
	removedSockets.push_back(socket);
}

void TServerComms::DeleteRemovedSockets(std::list<TClientSocket*>& removedSockets)
{
	for (auto it = removedSockets.begin(); it != removedSockets.end(); it++)
	{
		TClientSocket* client = *it;
		FinalizeConnection(client);
		if (Game)
			Game->DisconnectPlayer(client->ID);
		delete client;
	}
}