#include "play.h"

#include "states.h"



TPlayState::TPlayState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI)
{	
}

void TPlayState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);

	// TODO: 
	// - create server (temporary - should be in different state)
	// - open arena/match view
}

void TPlayState::Finalize()
{
	TGameState::Finalize();
}

void TPlayState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}

void TPlayState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Owner->SetNextState(SID_Menu);
}
