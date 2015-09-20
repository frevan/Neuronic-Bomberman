#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TOverlayView :	public nel::TTGUIView
{
private:
	std::shared_ptr<nel::IEventHandler> EventHandler;

protected:
	void CreateWidgets() override;

public:
	TOverlayView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine);
	~TOverlayView() override;

	void ProcessInput(nel::TGameID inputID, float value);

	// from IScene
	void OnAttach(nel::IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
};
