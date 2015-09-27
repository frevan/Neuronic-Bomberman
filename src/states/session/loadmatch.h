#pragma once

#include <TGUI/TGUI.hpp>

#include "../../base/nel_state.h"


class TLoadMatchState :	public nel::TGameState
{
private:
	std::shared_ptr<tgui::Gui> GUI;
public:
	TLoadMatchState(std::shared_ptr<tgui::Gui> setGUI);
	~TLoadMatchState() override;

	// from TGameState
	void Initialize(nel::IStateMachine* setOwner, nel::IApplication* setApplication) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(nel::TGameTime deltaTime) override;
};
