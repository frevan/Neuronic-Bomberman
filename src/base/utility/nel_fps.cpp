#include "nel_fps.h"



namespace nel {

TFpsCalculator::TFpsCalculator()
:	clock()
{ 
	fps = 0.0;
}

void TFpsCalculator::newFrame() 
{ 
	sf::Time elapsed = clock.restart();
	fps = 1000.f / elapsed.asMilliseconds();
};

};	// namespace nel