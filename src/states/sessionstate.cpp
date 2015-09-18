#include "sessionstate.h"

#include "states.h"



TSessionState::TSessionState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Lobby)
{
	//scene = std::make_shared<TLobbyScene>(setGUI);
}

TSessionState::~TSessionState()
{
	//scene.reset();
}

void TSessionState::Initialize(nel::IStateMachine* setStateMachine, nel::IApplication* setApplication)
{
	TGameState::Initialize(setStateMachine, setApplication);
	//Application->AttachScene(scene);
}

void TSessionState::Finalize()
{
	//Application->DetachScene(scene);
	TGameState::Finalize();
}

bool TSessionState::ProcessEvent(const sf::Event& event)
{
	bool handled = false;

	if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
	{
		StateMachine->SetNextState(SID_Menu);
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