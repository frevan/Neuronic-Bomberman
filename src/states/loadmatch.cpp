#include "loadmatch.h"

#include "states.h"



TLoadMatchState::TLoadMatchState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_LoadMatch),
	GUI(setGUI)
{	
}

TLoadMatchState::~TLoadMatchState()
{
}

void TLoadMatchState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);
}

void TLoadMatchState::Finalize()
{
	TGameState::Finalize();
}

bool TLoadMatchState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TLoadMatchState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}