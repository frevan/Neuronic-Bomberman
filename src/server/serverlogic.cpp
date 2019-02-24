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

bool TServerSideGame::GetPlayer(nel::TGameID ID, TPlayer& info)
{
	for (auto it = Players.begin(); it != Players.end(); it++)
	{
		if (it->ID == ID)
		{
			info = *it;
			return true;
		}
	}

	return false;
}

int TServerSideGame::ConnectPlayer(const std::string& name, nel::TGameID ID)
{
	int slot = FindAvailableSlot();
	if (slot >= 0)
	{
		TPlayer p;
		p.ClientID = 0;
		p.ID = ID;
		p.Name = name;
		p.Kills = 0;
		p.Score = 0;
		p.Slot = slot;

		Players.push_back(p);
	}
	return slot;
}

void TServerSideGame::DisconnectPlayer(nel::TGameID ID)
{
	for (auto it = Players.begin(); it != Players.end(); it++)
	{
		if (it->ID == ID)
		{
			it = Players.erase(it);
			return;
		}
	}
}
