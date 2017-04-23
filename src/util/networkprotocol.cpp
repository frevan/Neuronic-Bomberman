#include "networkprotocol.h"



// packet id's
const uint16_t ID_Connectionless	= 0xFFFF;
const uint16_t CL_KanNiet		= 0;
const uint16_t CL_NOP			= 1;
const uint16_t CL_Error			= 2;
const uint16_t CL_Ping			= 3;
const uint16_t CL_Ping_Response	= 4;
const uint16_t CL_Game			= 5;
const uint16_t CL_Move			= 6;
const uint16_t CL_Cmd			= 7;
const uint16_t SRV_KanNiet			= 0;
const uint16_t SRV_NOP				= 1;
const uint16_t SRV_Error			= 2;
const uint16_t SRV_Ping				= 3;
const uint16_t SRV_Ping_Response	= 4;
const uint16_t SRV_Info				= 0;	// duplicate value?
const uint16_t SRV_Game				= 5;
const uint16_t SRV_Cmd				= 7;
const uint16_t SRV_Map				= 8;
const uint16_t SRV_Time				= 0;	// duplicate value?
const uint16_t SRV_Position			= 0;	// duplicate value?
const uint16_t SRV_Position2		= 1;	// duplicate value?



const std::string CMD_InfoRequest		= "infoRequest";
const std::string CMD_Connect			= "connect";
const std::string CMD_RCon				= "rcon";
const std::string CMD_ConnectResponse	= "connectResponse";
const std::string CMD_InfoResponse		= "infoResponse";
const std::string CMD_Print				= "print";
const std::string CMD_Error				= "error";
const std::string CMD_Disconnect		= "disconnect";



// TBombermanProtocol

bool TBombermanProtocol::ProcessReceivedPacket(sf::Socket* Source, sf::Packet& Packet, IBombermanProtocolReceiver* Receiver)
{
	uint16_t id;
	if (!(Packet >> id))
		return false;

	// to be used below
	uint8_t u8_1, u8_2;
	uint16_t u16_1, u16_2;
	std::string s_1, s_2;

	if (id == ID_Connectionless)
	{
		std::string command;
		if (Packet >> command)
		{
			if (command == CMD_InfoRequest)
				return Receiver->OnInfoRequest(Source);
			else if (command == CMD_Connect)
			{
				if (Packet >> u8_1 >> s_1 >> s_2)
					return Receiver->OnConnect(Source, u8_1, s_1, s_2);
			}
			else if (command == CMD_RCon)
			{
				if (Packet >> s_1 >> s_2)
					return Receiver->OnRCon(Source, s_1, s_2);
			}
			else if (command == CMD_ConnectResponse)
			{
				if (Packet >> u8_1 >> u16_1 >> u16_2)
					return Receiver->OnConnectResponse(Source, u8_1, u16_1, u16_2);
			}
			else if (command == CMD_InfoResponse)
			{
				if (Packet >> u8_1 >> u8_2 >> s_1)
					return Receiver->OnInfoResponse(Source, u8_1, u8_2, s_1);
			}
			else if (command == CMD_Print)
			{
				if (Packet >> u8_1 >> s_1)
					return Receiver->OnPrint(Source, u8_1, s_1);
			}
			else if (command == CMD_Error)
			{
				if (Packet >> u8_1 >> s_1)
					return Receiver->OnError(Source, u8_1, s_1);
			}
			else if (command == CMD_Disconnect)
			{
				if (Packet >> s_1)
					return Receiver->OnDisconnect(Source, s_1);
			}
		}
	}

	return false;
}

bool TBombermanProtocol::SendToSocket(sf::Socket* Socket, sf::Packet& Packet)
{
	// change if we ever switch to UDP!!!
	sf::TcpSocket* sck = static_cast<sf::TcpSocket*>(Socket);
	sf::Socket::Status status = sck->send(Packet);

	return (status == sf::Socket::Status::Done);
}

void TBombermanProtocol::InitPacket(sf::Packet& Packet, uint16_t ID)
{
	Packet.clear();
	Packet << ID;
}

void TBombermanProtocol::InfoRequest(sf::Packet& Packet)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_InfoRequest;
}

void TBombermanProtocol::Connect(sf::Packet& Packet, const std::string& NodeName, const std::string& ClientVersion)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_Connect;
	uint8_t version = 0;
	Packet << version;
	Packet << NodeName;
	Packet << ClientVersion;
}

void TBombermanProtocol::RCon(sf::Packet& Packet, const std::string& Password, const std::string& Command)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_RCon;
	Packet << Password;
	Packet << Command;
}

void TBombermanProtocol::InfoResponse(sf::Packet& Packet, unsigned int Flags, const std::string& HostName)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_InfoResponse;
	uint8_t protocolVersion = 0;
	Packet << protocolVersion;
	Packet << static_cast<uint8_t>(Flags);
	Packet << HostName;
}

void TBombermanProtocol::ConnectResponse(sf::Packet& Packet, unsigned int ProtocolVersion, unsigned int ServerID, unsigned int ClientID)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_ConnectResponse;
	Packet << static_cast<uint8_t>(ProtocolVersion);
	Packet << static_cast<uint16_t>(ServerID);
	Packet << static_cast<uint16_t>(ClientID);
}

void TBombermanProtocol::Print(sf::Packet& Packet, unsigned int Type, const std::string& Text)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_Print;
	Packet << static_cast<uint8_t>(Type);
	Packet << Text;
}

void TBombermanProtocol::Error(sf::Packet& Packet, unsigned int Code, const std::string& Reason)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_Error;
	Packet << static_cast<uint8_t>(Code);
	Packet << Reason;
}

void TBombermanProtocol::Disconnect(sf::Packet& Packet, const std::string& Reason)
{
	InitPacket(Packet, ID_Connectionless);

	Packet << CMD_Disconnect;
	Packet << Reason;
}
