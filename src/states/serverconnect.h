#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
#include "../gameinterfaces.h"



class TServerConnectState :	public nel::TGameState
{
private:
	std::shared_ptr<tgui::Gui> GUI;
	bool CanConnectToServer;
	void ConnectToServer();
public:
	TServerConnectState(std::shared_ptr<tgui::Gui> setGUI);

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params) override;
	void Finalize() override;
	void Update(nel::TGameTime deltaTime) override;
	void ProcessInput(nel::TGameID inputID, float value) override;
};
