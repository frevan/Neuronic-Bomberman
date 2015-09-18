#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
//#include "../scenes/lobbyscene.h"


class TSessionState :	public nel::TGameState
{
private:
	//std::shared_ptr<TSessionScene> scene;
public:
	TSessionState(std::shared_ptr<tgui::Gui> setGUI);
	~TSessionState() override;

	// from TGameState
	void Initialize(nel::IStateMachine* setStateMachine, nel::IApplication* setApplication) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(nel::TGameTime deltaTime) override;
};