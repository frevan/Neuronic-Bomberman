#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
#include "../views/lobbyview.h"


class TLobbyState :	public nel::TGameState
{
private:
	std::shared_ptr<TLobbyView> View;
	std::shared_ptr<tgui::Gui> GUI;
public:
	TLobbyState(std::shared_ptr<tgui::Gui> setGUI);
	~TLobbyState() override;

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication, nel::IGameStateParamsPtr params) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(nel::TGameTime deltaTime) override;
};