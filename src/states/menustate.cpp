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

void TMenuState::Initialize(nel::IStateMachine* setStateMachine, nel::IApplication* setApplication)
{
	TGameState::Initialize(setStateMachine, setApplication);

	View = std::make_shared<TMenuView>(GUI, StateMachine);
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