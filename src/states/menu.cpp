#include "menu.h"

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

void TMenuState::Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params)
{
	TGameState::Initialize(setOwner, setApplication, params);

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

void TMenuState::ProcessInput(nel::TGameID inputID, float value)
{
	if (inputID == nel::actionToPreviousScreen && value != 0)
		Application->RequestQuit();
}