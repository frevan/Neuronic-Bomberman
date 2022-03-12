#include "lobby.h"

#include "states.h"



TLobbyState::TLobbyState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Lobby),
	View(),
	GUI(setGUI)
{	
}

TLobbyState::~TLobbyState()
{
}

void TLobbyState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);

	View = std::make_shared<TLobbyView>(GUI, Owner);
	Application->AttachView(View);
}

void TLobbyState::Finalize()
{
	Application->DetachView(View);
	View.reset();

	TGameState::Finalize();
}

void TLobbyState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		View->LeaveLobby();
}

void TLobbyState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}