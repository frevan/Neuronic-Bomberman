#pragma once

#include <TGUI/TGUI.hpp>

#include "../base/nel_state.h"
#include "../scenes/lobbyscene.h"


class TLobbyState :	public nel::TGameState
{
private:
	std::shared_ptr<TLobbyScene> scene;
public:
	TLobbyState(std::shared_ptr<tgui::Gui> setGUI);
	~TLobbyState() override;

	// from TGameState
	void Initialize(nel::IApplication* setApplication) override;
	void Finalize() override;
	bool ProcessEvent(const sf::Event& event) override;
	void Update(nel::TGameTime deltaTime) override;
};