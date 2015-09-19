#include "menustate.h"

#include "states.h"



TMenuState::TMenuState(std::shared_ptr<tgui::Gui> setGUI)
:	TGameState(SID_Menu),
	View(),
	GUI(setGUI)
{	
}

TMenuState::~TMenuState()
{
}

void TMenuState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication)
{
	TGameState::Initialize(setOwner, setApplication);

	View = std::make_shared<TMenuView>(GUI, Owner);
	Application->AttachScene(View);
}

void TMenuState::Finalize()
{
	Application->DetachScene(View);
	View.reset();

	TGameState::Finalize();
}

void TMenuState::Update(nel::TGameTime deltaTime)
{
	TGameState::Update(deltaTime);
}