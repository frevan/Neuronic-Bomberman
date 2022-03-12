#include "client.h"



// --- TClient ---

TClient::TClient()
:	IClient(),
	ILogic(),
	Socket(),
	Connected(false),
	SocketConnected(false),
	InLobby(false),
	Protocol(),
	ServerProtocol(0),
	ServerFlags(0),
	ServerName(""),
	ServerTag(nel::INVALID_GAME_ID),
	ClientTag(1),
	Application(nullptr)
{
}

TClient::~TClient()
{
	Disconnect("");
}

void TClient::Connect(const std::string& address, unsigned int port)
{
	Disconnect("");	// just in case

	Socket.setBlocking(false);

	Socket.connect(address, port);
}

void TClient::Disconnect(const std::string& reason)
{
	if (Connected)
	{
		sf::Packet packet;
		Protocol.Disconnect(packet, reason);
		Socket.send(packet);

		Connected = false;
	}

	if (SocketConnected)
	{
		Socket.disconnect();
		SocketConnected = false;
	}

	InLobby = false;
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
	InLobby = false;
}

void TClient::LeaveLobby()
{
	InLobby = false;
}

bool TClient::IsInLobby()
{
	return InLobby;
}

bool TClient::OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName)
{
	ServerProtocol = ProtocolVersion;
	ServerFlags = Flags;
	ServerName = HostName;

	sf::Packet packet;
	Protocol.Connect(packet, "LocalUser", "TestVersion", ClientTag);
	Socket.send(packet);

	return true;
}

bool TClient::OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, uint64_t ServerTag)
{
	ServerProtocol = ProtocolVersion;
	this->ServerTag = ServerTag;

	Connected = true;

	sf::Packet packet;
	Protocol.SetGameName(packet, this->ServerTag, "LocalGame");
	Socket.send(packet);

	InLobby = true;

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
	InLobby = false;
	Disconnect("");

	return true;
}

bool TClient::OnCreateGame(sf::Socket* Source, uint64_t ServerTag, const std::string& LobbyName)
{
	return false;
}
