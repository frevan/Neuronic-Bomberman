#pragma once

#include <TGUI/TGUI.hpp>

#include "../nel_scene.h"



namespace nel {

class TTGUIScene :	public TScene					
{
protected:
	std::shared_ptr<tgui::Gui> GUI;

public:
	TTGUIScene(std::shared_ptr<tgui::Gui> setGUI, TViewType setType = VT_HUMANVIEW);
	~TTGUIScene() override;

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
};

};	// namespace nel