#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TOverlayView :	public nel::TTGUIView
{
private:
	void CreateWidgets() override;

public:
	TOverlayView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine);
	~TOverlayView() override;

	// from IScene
	void Draw(sf::RenderTarget* target) override;
};
