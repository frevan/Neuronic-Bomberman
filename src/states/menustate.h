#pragma once

#include "../base/nel_state.h"
#include "../scenes/menuscene.h"



class TMenuState :	public nel::TGameState
{
private:
	std::shared_ptr<TMenuScene> scene;
public:
	TMenuState(std::shared_ptr<tgui::Gui> setGUI);
	~TMenuState() override;

	// from TGameState
	void Initialize(nel::IApplication* setApplication) override;
	void Finalize() override;
	void Update(nel::TGameTime deltaTime) override;
};