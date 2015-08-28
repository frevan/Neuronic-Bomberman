#include "nel_state.h"



namespace nel {

TGameState::TGameState()
:	IGameState(),
	Application(nullptr)
{
}

TGameState::~TGameState()
{
}

void TGameState::initialize(IApplication* setApplication)
{
	Application = setApplication;
}

void TGameState::finalize()
{
	Application = nullptr;
}

bool TGameState::processEvent(const sf::Event& event)
{
	bool handled = false;

	// window closed or escape key pressed: exit
	if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
	{
		Application->requestQuit();
		handled = true;
	}
	else
	{
		/*
		std::lock_guard<std::mutex> g(inputHandlersMutex);
		for (auto it = inputHandlers.begin(); it != inputHandlers.end(); it++)
		{
		if ((*it)->processEvent(event))
		break;
		}
		*/
	}

	return handled;
}

void TGameState::update(TGameTime deltaTime)
{
}

};	// namespace nel
