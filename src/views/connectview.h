#pragma once

#include "../view.h"
#include "../gamedata.h"

class TConnectToServerView :	public TTGUIView
{
private:
	tgui::EditBox::Ptr AddressEdit;
	tgui::Button::Ptr ConnectBtn;

	// event handlers
	void OnConnectBtnClick();

	// do stuff
	void Connect();

protected:
	void CreateWidgets() override;

public:
	TConnectToServerView(TGame* SetGame, tgui::Gui* SetGUI);
	~TConnectToServerView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
};