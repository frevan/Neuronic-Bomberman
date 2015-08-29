#include "nel_scene_tgui.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>



namespace nel {

TTGUIScene::TTGUIScene(std::shared_ptr<tgui::Gui> setGUI, TViewType setType)
:	TScene(setType),
	GUI(setGUI),
	Widgets()
{
}

TTGUIScene::~TTGUIScene()
{
	RemoveAllWidgetsFromGUI();
}

void TTGUIScene::OnAttach(IApplication* setApplication)
{
	TScene::OnAttach(setApplication);

	RemoveAllWidgetsFromGUI();
	CreateWidgets();
}

void TTGUIScene::OnDetach()
{
	RemoveAllWidgetsFromGUI();
	TScene::OnDetach();
}

void TTGUIScene::AddWidgetToGUI(tgui::Widget::Ptr widget, const std::string& widgetName)
{
	Widgets.push_back(widget);
	GUI->add(widget, widgetName);
}

void TTGUIScene::RemoveWidgetFromGUI(tgui::Widget::Ptr widget)
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

void TTGUIScene::RemoveAllWidgetsFromGUI()
{
	for (auto it = Widgets.begin(); it != Widgets.end(); it++)
		GUI->remove(*it);
	Widgets.clear();
}

};	// namespace nel