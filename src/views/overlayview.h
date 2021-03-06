#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TOverlayView :	public nel::TView
{
private:
	std::shared_ptr<nel::IEventHandler> EventHandler;
	sf::Font CommonFont;
	bool ShowOverlay, ShowFPS, ShowHelp;
	sf::Text HelpLine1, HelpLine2;
	sf::Text FPSText;
	sf::RectangleShape BackgroundShape;

public:
	TOverlayView(nel::IStateMachine* setStateMachine);
	~TOverlayView() override;

	void ProcessInput(nel::TGameID inputID, float value);

	// from IScene
	void OnAttach(nel::IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
};
