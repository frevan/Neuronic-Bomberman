#include "client.h"



// --- TClient ---

TClient::TClient()
:	IClient(),
	ILogic(),
	Socket(),
	Connected(false),
	SocketConnected(false),
	Protocol()
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
	sf::Packet packet;
	Protocol.Disconnect(packet, "no reason");
	Socket.send(packet);
	Connected = false;

	Socket.disconnect();
	SocketConnected = false;
}

void TClient::Process()
{
	sf::Packet packet;
	while (true)
	{
		sf::Socket::Status r = Socket.receive(packet);
		if (r != sf::Socket::Done && r != sf::Socket::Partial)
			break;

		Protocol.ProcessReceivedPacket(Socket, packet, this);
	}
}

bool TClient::IsConnected()
{
	return Connected;
}

void TClient::Update(nel::TGameTime deltaTime)
{
	auto addr = Socket.getRemoteAddress();
	auto port = Socket.getRemotePort();

	if (!Connected)
	{
		if (addr != sf::IpAddress::None && port != 0)
			HandleSocketConnect();
	}
	else if (Connected)
	{
		if (addr == sf::IpAddress::None || port == 0)
			HandleSocketDisconnect();
	}
}

void TClient::HandleSocketConnect()
{
	SocketConnected = true;

	sf::Packet packet;
	Protocol.Connect(packet, "TEST", "TESTV1");
	Socket.send(packet);
}

void TClient::HandleSocketDisconnect()
{
	// TODO

	Connected = false;
	SocketConnected = false;
}

void TClient::LeaveLobby()
{
}

bool TClient::IsInLobby()
{
	return false;
}

bool TClient::OnInfoRequest()
{
	return false;
}

bool TClient::OnConnect(int Version, const std::string& NodeName, const std::string& ClientVersion)
{
	return false;
}

bool TClient::OnRCon(const std::string& Password, const std::string& Command)
{
	return false;
}

bool TClient::OnInfoResponse(unsigned int Protocol, unsigned int Flags, const std::string& HostName)
{
	return false;
}

bool TClient::OnConnectResponse(unsigned int Protocol, unsigned int ServerID, unsigned int ClientID)
{
	return false;
}

bool TClient::OnPrint(unsigned int Type, const std::string& Text)
{
	return false;
}

bool TClient::OnError(unsigned int Code, const std::string& Reason)
{
	return false;
}

bool TClient::OnDisconnect(const std::string& Reason)
{
	Socket.disconnect();
	return true;
}
