#include "sessionstate.h"

#include "states.h"
#include "../base/nel_objecttypes.h"



TSessionState::TSessionState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Lobby),
	SessionState()
{
}

TSessionState::~TSessionState()
{
}

void TSessionState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);

	SessionState.reset((nel::IStateMachine*)Application->GetFactory().CreateObject(nel::OT_StateMachine));
}

void TSessionState::Finalize()
{
	SessionState.reset();

	TGameState::Finalize();
}

bool TSessionState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
	{
		Owner->SetNextState(SID_Menu);
		handled = true;
	};

	if (!handled)
		handled = TGameState::ProcessEvent(event);

	return handled;
}

void TSessionState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}
