#pragma once

#include "../view.h"
#include "../gamedata.h"

class TOptionsView :	public TTGUIView
{
private:
	const std::string ControlToString(const TInputControl& Control);

protected:
	void CreateWidgets() override;

public:
	TOptionsView(TGame* SetGame, tgui::Gui* SetGUI);
	~TOptionsView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
	void Draw(sf::RenderTarget* target) override;
};