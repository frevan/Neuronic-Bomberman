#include "state.h"

#include "actions.h"
#include "comms.h"

// ----------

void TState::Finish()
{
	if (View)
	{
		Game->DetachView(View->ID);
		View = nullptr;
	}
}

void TState::GameStateChanged()
{
	if (View)
		View->StateChanged();
}

// ----------

void TMenuState::Init()
{
	View = Game->AttachView(VIEW_MENU);

	Game->InputMap.DefineInput(actionMenuJoinGame, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Space, 0));
	Game->InputMap.DefineInput(actionMenuOptions, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::O, 0));
}

void TMenuState::Finish()
{
	TState::Finish();
}

// ----------

void TConnectingState::Init()
{
}

void TConnectingState::Finish()
{
	TState::Finish();
}

// ----------

void TLobbyState::Init()
{
	View = Game->AttachView(VIEW_LOBBY);

	if (Game->IsServer)
	{
		Game->Server->Start();

		Game->Client->Connect(sf::IpAddress::getLocalAddress().toString(), SERVER_PORT);
		Game->Client->CreateGame("Don't Explode");
	}

	Game->InputMap.DefineInput(actionLobbyPrevMap, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Left, 0));
	Game->InputMap.DefineInput(actionLobbyNextMap, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Right, 0));
	Game->InputMap.DefineInput(actionLobbyPrevSlot, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Up, 0));
	Game->InputMap.DefineInput(actionLobbyNextSlot, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Down, 0));
	Game->InputMap.DefineInput(actionLobbyAddPlayer, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Equal, 0));
	Game->InputMap.DefineInput(actionLobbyRemovePlayer, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::Hyphen, 0));
	Game->InputMap.DefineInput(actionLobbyRemapPlayerControls, TInputControl::Pack(TInputControl::KEYBOARD, 0, sf::Keyboard::R, 0));
}

void TLobbyState::Finish()
{
	Game->InputMap.RemoveInput(actionLobbyPrevMap);
	Game->InputMap.RemoveInput(actionLobbyNextMap);

	TState::Finish();
}

// ----------

void TMatchState::Init()
{
	View = Game->AttachView(VIEW_MATCH);

	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		int offset = i * PlayerActionCount;
		Game->InputMap.SetInputActive(actionMatchPlayer1Left + offset, true);
		Game->InputMap.SetInputActive(actionMatchPlayer1Right + offset, true);
		Game->InputMap.SetInputActive(actionMatchPlayer1Up + offset, true);
		Game->InputMap.SetInputActive(actionMatchPlayer1Down + offset, true);
		Game->InputMap.SetInputActive(actionMatchPlayer1DropBomb + offset, true);
	}
}

void TMatchState::Finish()
{
	for (int i = 0; i < MAX_NUM_SLOTS; i++)
	{
		int offset = i * PlayerActionCount;
		Game->InputMap.SetInputActive(actionMatchPlayer1Left + offset, false);
		Game->InputMap.SetInputActive(actionMatchPlayer1Right + offset, false);
		Game->InputMap.SetInputActive(actionMatchPlayer1Up + offset, false);
		Game->InputMap.SetInputActive(actionMatchPlayer1Down + offset, false);
		Game->InputMap.SetInputActive(actionMatchPlayer1DropBomb + offset, false);
	}

	TState::Finish();
}

// ----------

void TEndOfRoundState::Init()
{
	View = Game->AttachView(VIEW_ENDOFROUND);

}

void TEndOfRoundState::Finish()
{
	TState::Finish();
}

// ----------

void TEndOfMatchState::Init()
{
}

void TEndOfMatchState::Finish()
{
	TState::Finish();
}

// ----------

void TConnectToServerState::Init()
{
	View = Game->AttachView(VIEW_CONNECTTOSERVER);
}

void TConnectToServerState::Finish()
{
	TState::Finish();
}

// ----------

void TOptionsState::Init()
{
	View = Game->AttachView(VIEW_OPTIONS);
}

void TOptionsState::Finish()
{
	TState::Finish();
}