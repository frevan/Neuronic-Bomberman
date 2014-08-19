#include "game.h"



// --- TGame ---

TGame::TGame()
:	TGameApplication()
{
}

sf::Window* TGame::createWindow()
{
	return new sf::RenderWindow{sf::VideoMode{800, 600, 32}, "Bomberman"};
}
