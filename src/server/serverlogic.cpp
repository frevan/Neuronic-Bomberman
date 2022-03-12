#include "serverlogic.h"

const int SLOT_COUNT = 4;

// TServerSideGame

TServerSideGame::TServerSideGame()
:	IServerSideGame(),
	NodeName(),
	Players(),
	randomTagGenerator()
{
}

TServerSideGame::~TServerSideGame()
{
}

bool TServerSideGame::GetPlayer(uint64_t Tag, TPlayer& info)
{
	for (auto it = Players.begin(); it != Players.end(); it++)
	{
		if (it->Tag == Tag)
		{
			info = *it;
			return true;
		}
	}

	return false;
}

uint64_t TServerSideGame::ConnectPlayer(const std::string& ClientName, const std::string& SetVersion, uint64_t ClientTag)
{
	uint64_t tag = nel::INVALID_GAME_ID;

	int slot = FindAvailableSlot();
	if (slot >= 0)
	{
		tag = randomTagGenerator();
		TPlayer p;
		p.ClientTag = ClientTag;		
		p.Tag = tag;
		p.Name = ClientName;
		p.Kills = 0;
		p.Score = 0;
		p.Slot = slot;

		Players.push_back(p);
	}

	return tag;
}

void TServerSideGame::DisconnectPlayer(uint64_t Tag)
{
	for (auto it = Players.begin(); it != Players.end(); it++)
	{
		if (it->Tag == Tag)
		{
			it = Players.erase(it);
			return;
		}
	}
}

int TServerSideGame::FindAvailableSlot()
{
	bool slotIsUsed = true;
	int slot = -1;
	while (slotIsUsed && slot < SLOT_COUNT)
	{
		slot++;
		slotIsUsed = false;
		for (auto it = Players.begin(); it != Players.end(); it++)
		{
			if (it->Slot == slot)
			{
				slotIsUsed = true;
				break;
			}
		}
	}

	if (!slotIsUsed)
		return slot;
	else
		return -1;
}

void TServerSideGame::SetNodeName(const std::string& SetName)
{
	NodeName = SetName;
}

std::string TServerSideGame::GetNodeName()
{
	return NodeName;
}
