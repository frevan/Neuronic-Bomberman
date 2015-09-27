#include "session.h"

#include "states.h"
#include "../base/nel_objecttypes.h"

#include "session/lobby.h"
#include "session/serverselect.h"
#include "session/loadmatch.h"
#include "session/play.h"
#include "session/roundend.h"
#include "session/matchend.h"



TSessionState::TSessionState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Lobby),
	IGameSession(),
	States(),
	GUI(setGUI)
{
}

TSessionState::~TSessionState()
{
	States.reset();
}

nel::Interface* TSessionState::RetrieveInterface(nel::TGameID id)
{
	if (id == IID_GameSession)
		return (IGameSession*)this;
	else
		return TGameState::RetrieveInterface(id);
}

void TSessionState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);

	// register factory functions
	Application->GetFactory().RegisterObjectType(SID_Lobby, std::bind(&TSessionState::CreateState_Lobby, this));
	Application->GetFactory().RegisterObjectType(SID_ServerSelect, std::bind(&TSessionState::CreateState_ServerSelect, this));
	Application->GetFactory().RegisterObjectType(SID_LoadMatch, std::bind(&TSessionState::CreateState_LoadMatch, this));
	Application->GetFactory().RegisterObjectType(SID_Play, std::bind(&TSessionState::CreateState_Play, this));
	Application->GetFactory().RegisterObjectType(SID_RoundEnd, std::bind(&TSessionState::CreateState_RoundEnd, this));
	Application->GetFactory().RegisterObjectType(SID_MatchEnd, std::bind(&TSessionState::CreateState_MatchEnd, this));

	// create state machine
	States.reset((nel::IStateMachine*)Application->GetFactory().CreateObject(nel::OT_StateMachine));
	States->Initialize((IGameState*)this);

	// set initial state
	//States->SetNextState(SID_Lobby);
	States->SetNextState(SID_Play);
}

void TSessionState::Finalize()
{
	States->Finalize();

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

	States->SwitchToNextState();
}

void* TSessionState::CreateState_Lobby()
{
	return new TLobbyState(GUI);
}

void* TSessionState::CreateState_ServerSelect()
{
	return new TServerSelectState(GUI);
}

void* TSessionState::CreateState_LoadMatch()
{
	return new TLoadMatchState(GUI);
}

void* TSessionState::CreateState_Play()
{
	return new TPlayState(GUI);
}

void* TSessionState::CreateState_RoundEnd()
{
	return new TRoundEndState(GUI);
}

void* TSessionState::CreateState_MatchEnd()
{
	return new TMatchEndState(GUI);
}

void TSessionState::SessionGotoState(nel::TGameID stateID)
{
	Owner->SetNextState(stateID);
}
