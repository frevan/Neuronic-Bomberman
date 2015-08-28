#include "game.h"

#include "base/nel_state.h"
#include "states/menustate.h"



// --- TGame ---

TGame::TGame()
:	nel::TSFGUIApplication()
{
}

sf::RenderWindow* TGame::createWindow()
{
	return new sf::RenderWindow{sf::VideoMode{800, 600, 32}, "Bomberman"};
}

nel::IGameState* TGame::createInitialGameState()
{
	return new TMenuState();
}

void TGame::afterInitialization()
{
	nel::TApplication::afterInitialization();
}

