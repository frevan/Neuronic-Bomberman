#pragma once

#include <SFML/Network.hpp>

#include "base/nel_interfaces.h"
#include "base/utility/nel_macros.h"



struct TPlayer
{
	std::string Name;
	sf::IpAddress ClientAddress;
	nel::TGameID ClientID;
	int Slot;
	int Score;
	int Kills;
};

