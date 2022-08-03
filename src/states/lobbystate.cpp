
#include "lobbystate.h"

#include "../actions.h"
#include "../states/menustate.h"
#include "../views/lobbyview.h"

TLobbyState::TLobbyState(TGameInterface* SetOwner)
:	TState(SetOwner),
	LobbyView(nullptr)
{
}

void TLobbyState::OnActivate()
{
	LobbyView = new TLobbyView(Game, Game->GetGUI());
	Game->AttachView(LobbyView);
}

void TLobbyState::OnDeactivate()
{
	Game->DetachView(LobbyView);
	LobbyView = nullptr;
}

bool TLobbyState::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	if (Value != 1.0f)
		return false; // only handle key presses

	switch (InputID)
	{
		case actionToPreviousScreen:
			TMenuState* state = new TMenuState(Game);
			Game->SwitchToState(state);
			handled = true;
			break;
	};

	return handled;
}
*/