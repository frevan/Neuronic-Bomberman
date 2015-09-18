#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TLobbyView :	public nel::TTGUIView
{
private:
	void OnBackBtnClick();

protected:
	void CreateWidgets() override;

public:
	TLobbyView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine);
	~TLobbyView() override;
};