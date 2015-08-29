#pragma once

#include <cstdint>
#include <SFML/Main.hpp>

#include "../nel_interfaces.h"



namespace nel {

class TFpsCalculator :	public IFpsCalculator
{
private:
	sf::Clock clock;

public:
	TFpsCalculator();

	void newFrame() override;
};

};	// namespace nel