#pragma once

#include "../view.h"
#include "../gamedata.h"

class TConnectToServerView :	public TTGUIView
{
private:
	tgui::EditBox::Ptr AddressEdit;
	tgui::Button::Ptr ConnectBtn;
	tgui::Label::Ptr PreviousAddressLabels[5];

	// event handlers
	void OnConnectBtnClick();

	// do stuff
	void Connect(const std::string Address);
	void UpdatePreviousAddressList();

protected:
	void CreateWidgets() override;

public:
	TConnectToServerView(TGame* SetGame, tgui::Gui* SetGUI);
	~TConnectToServerView();

	bool ProcessInput(TInputID InputID, float Value) override;
	void StateChanged() override;
};