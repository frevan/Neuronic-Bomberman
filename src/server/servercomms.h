#pragma once



#include <list>
#include <thread>
#include <mutex>
#include <SFML/Network.hpp>

#include "../base/nel_interfaces.h"
#include "../util/networkprotocol.h"



#define TClientSocket sf::TcpSocket



class IServerCommsCallback
{
};



class TServerComms : public IBombermanProtocolReceiver
{
private:
	std::thread* ListenerThread;
	sf::TcpListener Listener;
	sf::SocketSelector SocketSelector;
	std::list<TClientSocket*> ClientSockets;
	std::mutex ClientSocketsMutex;
	TBombermanProtocol Protocol;

	bool ThreadsShouldStop;

	void ListenFunc();
	void InitializeNewConnection(TClientSocket* socket);
	void FinalizeConnection(TClientSocket* socket);

public:
	TServerComms();
	~TServerComms();

	bool Start(unsigned int port);
	void Stop();

	void Update(nel::TGameTime deltaTime);

	// from IBombermanProtocolReceiver
	bool OnInfoRequest(sf::Socket* Source) override;
	bool OnConnect(sf::Socket* Source, int Version, const std::string& NodeName, const std::string& ClientVersion) override;
	bool OnRCon(sf::Socket* Source, const std::string& Password, const std::string& Command) override;
	bool OnInfoResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int Flags, const std::string& HostName) override;
	bool OnConnectResponse(sf::Socket* Source, unsigned int ProtocolVersion, unsigned int ServerID, unsigned int ClientID) override;
	bool OnPrint(sf::Socket* Source, unsigned int Type, const std::string& Text) override;
	bool OnError(sf::Socket* Source, unsigned int Code, const std::string& Reason) override;
	bool OnDisconnect(sf::Socket* Source, const std::string& Reason) override;
};