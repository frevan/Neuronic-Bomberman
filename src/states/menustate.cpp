#include "menustate.h"

#include "states.h"



TMenuState::TMenuState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Menu)
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

	TGameState::Finalize();
}

void TMenuState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}