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
	TTGUIView(std::shared_ptr<tgui::Gui> setGUI, IStateMachine* setStateMachine, TViewType setType = TViewType::VT_HUMANVIEW);
	~TTGUIView() override;

	std::shared_ptr<tgui::Gui> GetGUIPtr() { return GUI; };

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
};



class TTGUISystemView : public TView
{
private:
	std::shared_ptr<tgui::Gui> GUI;

public:
	TTGUISystemView(std::shared_ptr<tgui::Gui> setGUI, IStateMachine* setStateMachine);

	// from IScene
	void Draw(sf::RenderTarget* target);
};

};	// namespace nel