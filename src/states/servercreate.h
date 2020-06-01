#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
#include "../gameinterfaces.h"

#include "../views/connectingview.h"



class TServerCreateState :	public nel::TGameState
{
private:
	typedef enum {
		SHOULDCONNECT,
		CONNECTING,
		CONNECTED,
		INLOBBY
	} TConnectionProgress;

	std::shared_ptr<tgui::Gui> GUI;
	std::shared_ptr<TConnectingView> View;
	TConnectionProgress progress;

	void ConnectToServer();

public:
	TServerCreateState(std::shared_ptr<tgui::Gui> setGUI);

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params) override;
	void Finalize() override;
	void Update(nel::TGameTime deltaTime) override;
	void ProcessInput(nel::TGameID inputID, float value) override;
};

