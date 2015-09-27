#include "play.h"

#include "../states.h"
#include "../../gameinterfaces.h"



TPlayState::TPlayState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Play),
	GUI(setGUI)
{	
}

void TPlayState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);
}

void TPlayState::Finalize()
{
	TGameState::Finalize();
}

bool TPlayState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TPlayState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}

void TPlayState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
	{
		IGameSession* session = (IGameSession*)Owner->RetrieveInterface(IID_GameSession);
		if (session)
			session->SessionGotoState(SID_Menu);
	}
}