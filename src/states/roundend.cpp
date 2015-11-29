#include "roundend.h"

#include "states.h"



TRoundEndState::TRoundEndState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_RoundEnd),
	GUI(setGUI)
{	
}

void TRoundEndState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);
}

void TRoundEndState::Finalize()
{
	TGameState::Finalize();
}

bool TRoundEndState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TRoundEndState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}