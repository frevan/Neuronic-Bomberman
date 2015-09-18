#include "nel_state.h"



namespace nel {

class TGameStateLogic :	public ILogic
{
private:
	TGameState* State;
public:
	TGameStateLogic(TGameState* setState):	ILogic(), State(setState) {};

	// from ILogic
	void Update(TGameTime deltaTime) override { State->Update(deltaTime); };
};



TGameState::TGameState(TGameID setStateID)
:	IGameState(),
	Application(nullptr),
	StateMachine(nullptr),
	StateID(setStateID)
{
	Logic = std::make_shared<TGameStateLogic>(this);
}

TGameState::~TGameState()
{
	Logic.reset();
}

void TGameState::Initialize(IStateMachine* setStateMachine, IApplication* setApplication)
{
	Application = setApplication;
	StateMachine = setStateMachine;

	Application->AddEventHandler(this);
	Application->AddLogic(Logic);
}

void TGameState::Finalize()
{
	Application->RemoveEventHandler(this);
	Application->RemoveLogic(Logic);

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
