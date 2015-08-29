#pragma once

#include <vector>
#include <TGUI/TGUI.hpp>

#include "../nel_scene.h"



namespace nel {

class TTGUIScene :	public TScene					
{
private:
	std::vector<tgui::Widget::Ptr> Widgets;
protected:
	std::shared_ptr<tgui::Gui> GUI;

	void AddWidgetToGUI(tgui::Widget::Ptr widget, const std::string& widgetName = "");
	void RemoveWidgetFromGUI(tgui::Widget::Ptr widget);
	void RemoveAllWidgetsFromGUI();

	virtual void CreateWidgets() {};

public:
	TTGUIScene(std::shared_ptr<tgui::Gui> setGUI, TViewType setType = VT_HUMANVIEW);
	~TTGUIScene() override;

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
};

};	// namespace nel