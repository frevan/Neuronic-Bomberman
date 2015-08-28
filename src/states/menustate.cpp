#include "menustate.h"



TMenuState::TMenuState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState()
{
	scene = std::make_shared<TMenuScene>(setGUI);
}

TMenuState::~TMenuState()
{
	scene.reset();
}

void TMenuState::Initialize(nel::IApplication* setApplication)
{
	TGameState::Initialize(setApplication);

	Application->AttachScene(scene);
}

void TMenuState::Finalize()
{
	Application->DetachScene(scene);
}

bool TMenuState::ProcessEvent(const sf::Event& event)
{
	return TGameState::ProcessEvent(event);
}

void TMenuState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}