#pragma once

#include "../base/nel_interfaces.h"
#include "../base/tgui/nel_scene_tgui.h"



class TLobbyScene :	public nel::TTGUIScene
{
private:
	void OnBackBtnClick();

protected:
	void CreateWidgets() override;

public:
	TLobbyScene(std::shared_ptr<tgui::Gui> setGUI);
	~TLobbyScene() override;
};