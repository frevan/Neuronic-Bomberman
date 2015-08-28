#include "nel_scene_tgui.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>



namespace nel {

TTGUIScene::TTGUIScene(std::shared_ptr<tgui::Gui> setGUI, TViewType setType)
:	TScene(setType),
	GUI(setGUI)
{
}

TTGUIScene::~TTGUIScene()
{
}

void TTGUIScene::OnAttach(IApplication* setApplication)
{
	TScene::OnAttach(setApplication);
}

void TTGUIScene::OnDetach()
{
	TScene::OnDetach();
}

};	// namespace nel