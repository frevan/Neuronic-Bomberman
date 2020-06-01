#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TConnectingView :	public nel::TTGUIView
{
protected:
	void CreateWidgets() override;

public:
	TConnectingView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine);
	~TConnectingView() override;
};