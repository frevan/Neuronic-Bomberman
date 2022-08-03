/*
#include "menustate.h"

#include <TGUI/TGUI.hpp>

#include "../actions.h"
#include "../views/menuview.h"

TMenuState::TMenuState(TGameInterface* SetOwner)
:	TState(SetOwner),
	MenuView(nullptr)
{
}

void TMenuState::OnActivate()
{
	MenuView = new TMenuView(Game, Game->GetGUI());
	Game->AttachView(MenuView);
}

void TMenuState::OnDeactivate()
{
	Game->DetachView(MenuView);
	MenuView = nullptr;
}

bool TMenuState::ProcessInput(TInputID InputID, float Value)
{
	bool handled = false;

	if (Value != 1.0f)
		return false; // only handle key presses

	switch (InputID)
	{
		case actionToPreviousScreen:
			Game->RequestQuit();
			handled = true;
			break;
	};

	return handled;
}
*/