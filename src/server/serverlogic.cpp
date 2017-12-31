#include "serverlogic.h"

// TServerSideGame

TServerSideGame::TServerSideGame()
:	IServerSideGame(),
	Players()
{
}

TServerSideGame::~TServerSideGame()
{
}

unsigned int TServerSideGame::GetPlayerCount()
{
	return 0;
}

bool TServerSideGame::GetPlayer(TPlayer& info)
{
	return false;
}

int TServerSideGame::ConnectPlayer(const std::string& name, nel::TGameID ID)
{
	return -1;
}

void TServerSideGame::DisconnectPlayer(nel::TGameID ID)
{
}
