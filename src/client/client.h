#pragma once

#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"

#include "../gameinterfaces.h"
#include "../util/networkprotocol.h"



class TClient :	public IClient,
				public nel::ILogic,
				public IBombermanProtocolReceiver
{
private:
	sf::TcpSocket Socket;
	bool Connected, SocketConnected, InLobby;
	TBombermanProtocol Protocol;

	uint8_t ServerProtocol;
	unsigned int ServerFlags;
	std::string ServerName;
	nel::TGameID ServerTag;
	nel::TGameID ClientTag;

	void HandleSocketConnect();
	void HandleSocketDisconnect();

public:
	nel::IApplication* Application;

	TClient();
	~TClient() override;

	// from IClient
	void Process() override;
	void Connect(const std::string& address, unsigned int port) override;
	void Disconnect(const std::string& reason) override;
	bool IsConnected() override;
	void LeaveLobby() override;
	bool IsInLobby() override;

	// from ILogic
	void Update(nel::TGameTime deltaTime) override;

	// from IBombermanProtocolReceiver
	bool OnInfoRequest(sf::Socket* Source) override { return false; };
	bool OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion, uint64_t ClientTag) override { return false; };
	bool OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command) override { return false; };
	bool OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName) override;
	bool OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, uint64_t ServerTag) override;
	bool OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text) override;
	bool OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason) override;
	bool OnDisconnect(sf::Socket* Source, const std::string& Reason) override;
	bool OnCreateGame(sf::Socket* Source, uint64_t ServerTag, const std::string& LobbyName);
};
