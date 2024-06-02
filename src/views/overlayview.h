#pragma once

#include "../view.h"
#include "../gamedata.h"

class TOverlayView :	public TTGUIView
{
private:
	sf::RectangleShape BackgroundShape;

protected:
	void CreateWidgets() override;

public:
	TOverlayView(TGame* SetGame, tgui::Gui* SetGUI);
	~TOverlayView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
	void Draw(sf::RenderTarget* target) override;
};