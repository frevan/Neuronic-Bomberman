#include "serverselect.h"

#include "../states.h"



TServerSelectState::TServerSelectState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_ServerSelect),
	GUI(setGUI)
{	
}

void TServerSelectState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);
}

void TServerSelectState::Finalize()
{
	TGameState::Finalize();
}

bool TServerSelectState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TServerSelectState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}