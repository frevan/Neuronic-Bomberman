#pragma once

#include <cstdint>
#include <array>
#include <SFML/Main.hpp>

#include "../nel_interfaces.h"



namespace nel {

class TFpsCalculator :	public IFpsCalculator
{
private:
	enum { CACHESIZECONST = 256 };

	sf::Clock Clock;
	std::array<int64_t, CACHESIZECONST> FrameTimeCache;
	int CacheIdx, CacheCount;

	void AddValueToCache(int64_t value);
	void CalculateValue();

public:
	TFpsCalculator();

	void NewFrame() override;
};

};	// namespace nel