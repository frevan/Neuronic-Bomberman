#include "client.h"



// --- TClient ---

TClient::TClient()
:	IClient(),
	ILogic(),
	Socket(),
	Connected(false),
	SocketConnected(false),
	Protocol(),
	ServerProtocol(0),
	ServerFlags(0),
	ServerName(""),
	ServerID(0),
	ClientID(0),
	Application(nullptr)
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
	if (Connected)
	{
		sf::Packet packet;
		Protocol.Disconnect(packet, "no reason");
		Socket.send(packet);

		Connected = false;
	}

	if (SocketConnected)
	{
		Socket.disconnect();
		SocketConnected = false;
	}
}

void TClient::Process()
{
	sf::Packet packet;
	while (SocketConnected)
	{
		sf::Socket::Status r = Socket.receive(packet);
		if (r != sf::Socket::Done && r != sf::Socket::Partial)
			break;

		Protocol.ProcessReceivedPacket(&Socket, packet, this);
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

	if (SocketConnected)
	{
		// check for disconnection
		if (addr == sf::IpAddress::None || port == 0)
			HandleSocketDisconnect();
	}
	else
	{
		// check for connection
		if (addr != sf::IpAddress::None && port != 0)
			HandleSocketConnect();
	}
}

void TClient::HandleSocketConnect()
{
	SocketConnected = true;

	sf::Packet packet;
	Protocol.InfoRequest(packet);
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

bool TClient::OnInfoRequest(sf::Socket* Source)
{
	return false;
}

bool TClient::OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion)
{
	return false;
}

bool TClient::OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command)
{
	return false;
}

bool TClient::OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName)
{
	ServerProtocol = ProtocolVersion;
	ServerFlags = Flags;
	ServerName = HostName;

	sf::Packet packet;
	Protocol.Connect(packet, "LocalUser", "TestVersion");
	Socket.send(packet);

	return true;
}

bool TClient::OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int ServerID, unsigned int ClientID)
{
	ServerProtocol = ProtocolVersion;
	this->ServerID = ServerID;
	this->ClientID = ClientID;

	Connected = true;

	return true;
}

bool TClient::OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text)
{
	return false;
}

bool TClient::OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason)
{
	switch (Code)
	{
		case Err_Unknown:		break;
		case Err_ServerFull:	break;
	};

	return true;
}

bool TClient::OnDisconnect(sf::Socket* Source, const std::string& Reason)
{
	Connected = false;
	Disconnect();

	return true;
}
