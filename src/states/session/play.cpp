#include "play.h"

#include "../states.h"



TPlayState::TPlayState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI),
	Session(nullptr)
{	
}

void TPlayState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);

	Session = (IGameSession*)Owner->RetrieveInterface(IID_GameSession);
}

void TPlayState::Finalize()
{
	Session = nullptr;

	TGameState::Finalize();
}

void TPlayState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}

void TPlayState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Session->SessionGotoState(SID_Menu);
}
