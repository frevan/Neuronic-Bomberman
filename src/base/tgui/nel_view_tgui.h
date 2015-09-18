#pragma once

#include <vector>
#include <TGUI/TGUI.hpp>

#include "../nel_view.h"



namespace nel {

class TTGUIView :	public TView
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
	TTGUIView(std::shared_ptr<tgui::Gui> setGUI, IStateMachine* setStateMachine, TViewType setType = VT_HUMANVIEW);
	~TTGUIView() override;

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
};

};	// namespace nel