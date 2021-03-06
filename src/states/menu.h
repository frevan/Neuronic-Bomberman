#pragma once

#include "../base/nel_state.h"
#include "../views/menuview.h"



class TMenuState :	public nel::TGameState
{
private:
	std::shared_ptr<TMenuView> View;
	std::shared_ptr<tgui::Gui> GUI;
public:
	TMenuState(std::shared_ptr<tgui::Gui> setGUI);
	~TMenuState() override;

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params) override;
	void Finalize() override;
	void Update(nel::TGameTime deltaTime) override;
	void ProcessInput(nel::TGameID inputID, float value) override;
};