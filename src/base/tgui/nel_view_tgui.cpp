#include "nel_view_tgui.h"



namespace nel {

TTGUISystemView::TTGUISystemView(std::shared_ptr<tgui::Gui> setGUI, IStateMachine* setStateMachine)
:	TView(setStateMachine, nel::IView::TViewType::VT_SYSTEMVIEW),
	GUI(setGUI)
{
}

void TTGUISystemView::Draw(sf::RenderTarget* target) 
{ 
	GUI->draw(); 
};



TTGUIView::TTGUIView(std::shared_ptr<tgui::Gui> setGUI, IStateMachine* setStateMachine, TViewType setType)
:	TView(setStateMachine, setType),	
	GUI(setGUI),
	Widgets()
{
}

TTGUIView::~TTGUIView()
{
	RemoveAllWidgetsFromGUI();
}

void TTGUIView::OnAttach(IApplication* setApplication)
{
	TView::OnAttach(setApplication);

	RemoveAllWidgetsFromGUI();
	CreateWidgets();
}

void TTGUIView::OnDetach()
{
	RemoveAllWidgetsFromGUI();
	TView::OnDetach();
}

void TTGUIView::AddWidgetToGUI(tgui::Widget::Ptr widget, const std::string& widgetName)
{
	Widgets.push_back(widget);
	GUI->add(widget, widgetName);
}

void TTGUIView::RemoveWidgetFromGUI(tgui::Widget::Ptr widget)
{
	GUI->remove(widget);
	for (auto it = Widgets.begin(); it != Widgets.end(); it++)
	{
		if ((*it) == widget)
		{
			Widgets.erase(it);
			break;
		}
	}
}

void TTGUIView::RemoveAllWidgetsFromGUI()
{
	for (auto it = Widgets.begin(); it != Widgets.end(); it++)
		GUI->remove(*it);
	Widgets.clear();
}

};	// namespace nel