#include <iostream>
#include <string>

#include "game.h"



int main(int argc, char *argv[])
{
	TGame game{};

	// initialize game
	if (!game.Initialize(argv[0]))
	{
		std::cout << "There was an initialization error" << std::endl;
		return 1;
	}

	// game loop
	game.Execute();

	// clean up before exit
	game.Finalize();

	return 0;
}