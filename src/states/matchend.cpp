#include "matchend.h"

#include "states.h"



TMatchEndState::TMatchEndState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_MatchEnd),
	GUI(setGUI)
{	
}

void TMatchEndState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);
}

void TMatchEndState::Finalize()
{
	TGameState::Finalize();
}

bool TMatchEndState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TMatchEndState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}