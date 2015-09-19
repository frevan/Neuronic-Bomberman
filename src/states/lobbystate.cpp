#include "lobbystate.h"

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

void TLobbyState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);

	View = std::make_shared<TLobbyView>(GUI, Owner);
	Application->AttachScene(View);
}

void TLobbyState::Finalize()
{
	Application->DetachScene(View);
	View.reset();

	TGameState::Finalize();
}

bool TLobbyState::ProcessEvent(const sf::Event& event)
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

void TLobbyState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}