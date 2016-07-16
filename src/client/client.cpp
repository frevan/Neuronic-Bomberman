#include "client.h"



// --- TClient ---

TClient::TClient()
:	IClient(),
	ILogic(),
	Socket(),
	Connected(false)
{
}

TClient::~TClient()
{
	Disconnect();
}

void TClient::Connect(const std::string& address, unsigned int port)
{
	Disconnect();	// just in case

	Socket.setBlocking(false);

	Socket.connect(address, port);
}

void TClient::Disconnect()
{
	Socket.disconnect();
	Connected = false;
}

void TClient::Process()
{
	sf::Packet packet;
	while (true)
	{
		sf::Socket::Status r = Socket.receive(packet);
		if (r != sf::Socket::Done && r != sf::Socket::Partial)
			break;

		ProcessPacketFromServer(packet);
	}
}

bool TClient::IsConnected()
{
	return Connected;
}

void TClient::ProcessPacketFromServer(sf::Packet& packet)
{
	// TODO: process packets
}

void TClient::Update(nel::TGameTime deltaTime)
{
	auto addr = Socket.getRemoteAddress();
	auto port = Socket.getRemotePort();

	if (!Connected)
	{
		if (addr != sf::IpAddress::None && port != 0)
			Connected = true;
	}
	else if (Connected)
	{
		if (addr == sf::IpAddress::None || port == 0)
			HandleServerDisconnect();
	}
}

void TClient::HandleServerDisconnect()
{
	// TODO

	Connected = false;
}