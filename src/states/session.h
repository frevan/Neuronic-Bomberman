#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
#include "../gameinterfaces.h"


class TSessionState :	public nel::TGameState,
						public IGameSession
{
private:
	std::shared_ptr<tgui::Gui> GUI;
	std::shared_ptr<nel::IStateMachine> States;

	// factory functions
	void* CreateState_Lobby();
	void* CreateState_ServerSelect();
	void* CreateState_LoadMatch();
	void* CreateState_Play();
	void* CreateState_RoundEnd();
	void* CreateState_MatchEnd();

public:
	TSessionState(std::shared_ptr<tgui::Gui> setGUI);
	~TSessionState() override;

	// from Interface
	nel::Interface* RetrieveInterface(nel::TGameID id) override;

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(nel::TGameTime deltaTime) override;

	// from IGameSession
	void SessionGotoState(nel::TGameID stateID) override;
};
