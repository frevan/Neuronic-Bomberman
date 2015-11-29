#include "serverconnect.h"

#include "states.h"



TServerConnectState::TServerConnectState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI)
{	
}

void TServerConnectState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);
}

void TServerConnectState::Finalize()
{
	TGameState::Finalize();
}

void TServerConnectState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}

void TServerConnectState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Owner->SetNextState(SID_Menu);
}
