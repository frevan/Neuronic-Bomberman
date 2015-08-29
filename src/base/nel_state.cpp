#include "nel_state.h"



namespace nel {

TGameState::TGameState(TGameID setStateID)
:	IGameState(),
	Application(nullptr),
	StateID(setStateID)
{
}

TGameState::~TGameState()
{
}

void TGameState::Initialize(IApplication* setApplication)
{
	Application = setApplication;
}

void TGameState::Finalize()
{
	Application = nullptr;
}

bool TGameState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	// window closed or escape key pressed: exit
	if ((event.type == sf::Event::Closed) || ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
	{
		Application->RequestQuit();
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

void TGameState::Update(TGameTime deltaTime)
{
}

};	// namespace nel
