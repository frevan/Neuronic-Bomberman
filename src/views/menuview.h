#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_view_tgui.h"



class TMenuView :	public nel::TTGUIView
{
private:	
	void CreateWidgets() override;

	void OnNewGameBtnClick();
	void OnJoinGameBtnClick();
	void OnOptionsBtnClick();
	void OnQuitBtnClick();

public:
	TMenuView(std::shared_ptr<tgui::Gui> setGUI, nel::IStateMachine* setStateMachine);
	~TMenuView() override;
};