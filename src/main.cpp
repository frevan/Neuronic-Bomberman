#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "game.h"



int main(int argc, char *argv[])
{
	TGame game{};

	// initialize game
	if (!game.initialize(argv[0]))
	{
		std::cout << "There was an initialization error" << std::endl;
		return 1;
	}

	// game loop
	game.execute();

	// clean up before exit
	game.finalize();

	return 0;
}