#include "lobbystate.h"

#include "states.h"



TLobbyState::TLobbyState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Lobby)
{
	scene = std::make_shared<TLobbyScene>(setGUI);
}

TLobbyState::~TLobbyState()
{
	scene.reset();
}

void TLobbyState::Initialize(nel::IApplication* setApplication)
{
	TGameState::Initialize(setApplication);
	Application->AttachScene(scene);
}

void TLobbyState::Finalize()
{
	Application->DetachScene(scene);
	TGameState::Finalize();
}

bool TLobbyState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
	{
		Application->SetNextState(SID_Menu);
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