#include "nel_state.h"

#include "utility/nel_eventhandler.h"



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
	Owner(nullptr),
	StateID(setStateID)
{
	Logic = std::make_shared<TGameStateLogic>(this);
	EventHandler = std::make_shared<TEventHandler>(IEventHandler::STATE, std::bind(&TGameState::ProcessEvent, this, std::placeholders::_1));
}

TGameState::~TGameState()
{
	Logic.reset();
	EventHandler.reset();
}

void TGameState::Initialize(IStateMachine* setOwner, IApplication* setApplication)
{
	Application = setApplication;
	Owner = setOwner;

	Application->AddEventHandler(EventHandler);
	Application->AddLogic(Logic);
}

void TGameState::Finalize()
{
	Application->RemoveEventHandler(EventHandler);
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

	return handled;
}

void TGameState::Update(TGameTime deltaTime)
{
}

};	// namespace nel
